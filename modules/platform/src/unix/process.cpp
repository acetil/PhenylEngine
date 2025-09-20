#include "platform/process.h"

#include "../../../util/include/util/range_utils.h"
#include "util/string_help.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <istream>
#include <sys/wait.h>
#include <unistd.h>

using namespace phenyl::os;

static phenyl::Logger LOGGER{"UNIX_PROCESS", phenyl::PHENYL_LOGGER};

namespace {
class UnixIstreambuf : public std::streambuf {
public:
    UnixIstreambuf (int fd) : m_fd{fd} {
        setg(m_buf.data(), m_buf.data(), m_buf.data());
    }

    UnixIstreambuf (const UnixIstreambuf&) = delete;

    UnixIstreambuf (UnixIstreambuf&& other) noexcept : m_fd{other.m_fd}, m_buf{other.m_buf} {
        other.m_fd = -1;
        setg(m_buf.data(), m_buf.data() + (other.gptr() - other.eback()),
            m_buf.data() + (other.egptr() - other.eback()));
    }

    UnixIstreambuf& operator= (const UnixIstreambuf&) = delete;

    UnixIstreambuf& operator= (UnixIstreambuf&& other) noexcept {
        if (m_fd >= 0) {
            close(m_fd);
        }
        m_fd = other.m_fd;
        m_buf = other.m_buf;
        setg(m_buf.data(), m_buf.data() + (other.gptr() - other.eback()),
            m_buf.data() + (other.egptr() - other.eback()));

        other.m_fd = -1;
        return *this;
    }

    ~UnixIstreambuf () override {
        if (m_fd >= 0) {
            close(m_fd);
        }
    }

    std::streambuf::int_type underflow () override {
        char* curr = gptr();
        if (curr < egptr()) {
            return *curr;
        }

        auto remainingSpace = m_buf.size() - (curr - m_buf.data());
        if (remainingSpace == 0) {
            // At end of buffer
            curr = m_buf.data();
            remainingSpace = m_buf.size();
        }

        auto bytesRead = read(m_fd, curr, remainingSpace);
        if (bytesRead <= 0) {
            return traits_type::eof();
        }

        char* end = curr + bytesRead;
        setg(m_buf.data(), curr, end);
        return *curr;
    }

private:
    int m_fd;
    std::array<char, 1024> m_buf;
};

class UnixIstream : public std::istream {
public:
    static UnixIstream Make (int fd) {
        auto streamBuf = std::make_unique<UnixIstreambuf>(fd);
        return UnixIstream{std::move(streamBuf)};
    }

private:
    explicit UnixIstream (std::unique_ptr<UnixIstreambuf> buf) : std::istream{buf.get()}, m_streamBuf{std::move(buf)} {}

    std::unique_ptr<UnixIstreambuf> m_streamBuf;
};

class UnixProcess : public IProcess {
public:
    UnixProcess (pid_t pid, int stdoutFd, int stderrFd) :
        m_pid{pid},
        m_stdout{UnixIstream::Make(stdoutFd)},
        m_stderr{UnixIstream::Make(stderrFd)} {}

    std::uint32_t pid () const noexcept override {
        return m_pid;
    }

    std::istream& stdoutStream () override {
        return m_stdout;
    }

    std::istream& stderrStream () override {
        return m_stderr;
    }

    void wait () override {
        int status;
        waitpid(m_pid, &status, 0);
    }

private:
    pid_t m_pid;
    UnixIstream m_stdout;
    UnixIstream m_stderr;
};
} // namespace

Process Process::Run (const std::string& executable, std::span<const std::string> argv) {
    if (argv.empty()) {
        PHENYL_LOGD(LOGGER, "Executing \"{}\"", executable, util::JoinStrings(" ", argv));
    } else {
        PHENYL_LOGD(LOGGER, "Executing \"{} {}\"", executable, util::JoinStrings(" ", argv));
    }

    std::vector<const char*> cArgv;
    cArgv.reserve(argv.size() + 2);
    cArgv.emplace_back(executable.c_str());
    std::ranges::transform(argv, std::back_inserter(cArgv), [] (const auto& s) { return s.c_str(); });
    cArgv.emplace_back(nullptr);

    int stdoutFds[2] = {};
    int stderrFds[2] = {};
    if (pipe2(stdoutFds, 0) < 0) {
        PHENYL_ABORT("Failed to call pipe2(): {}", strerror(errno));
    }
    if (pipe2(stderrFds, 0) < 0) {
        PHENYL_ABORT("Failed to call pipe2(): {}", strerror(errno));
    }

    pid_t pid = fork();
    PHENYL_ASSERT_MSG(pid >= 0, "Failed to fork process: {}", std::strerror(errno));
    if (pid == 0) {
        // Child process
        close(stdoutFds[0]);
        close(stderrFds[0]);

        if (dup2(stdoutFds[1], STDOUT_FILENO) < 0) {
            std::cerr << std::format("Failed to replace stdout in child pid={}: {}", getpid(), strerror(errno));
            std::terminate();
        }

        if (dup2(stderrFds[1], STDERR_FILENO) < 0) {
            std::cerr << std::format("Failed to replace stderr in child pid={}: {}", getpid(), strerror(errno));
            std::terminate();
        }

        if (execv(cArgv[0], const_cast<char**>(cArgv.data())) < 0) {
            std::cerr << std::format("Failed to execv() {} in child pid={}: {}", executable, getpid(), strerror(errno));
            std::terminate();
        }
        // Unreachable
        std::terminate();
    }

    // Parent process
    close(stdoutFds[1]);
    close(stderrFds[1]);
    PHENYL_LOGD(LOGGER, "Successfully started subprocess pid={}", pid);
    return Process{std::make_unique<UnixProcess>(pid, stdoutFds[0], stderrFds[0])};
}
