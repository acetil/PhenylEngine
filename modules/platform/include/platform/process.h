#pragma once

#include "logging/logging.h"

#include <cstdint>
#include <memory>
#include <span>

namespace phenyl::os {
class IProcess {
public:
    virtual ~IProcess () = default;

    virtual std::uint32_t pid () const noexcept = 0;

    virtual std::istream& stdoutStream () = 0;
    virtual std::istream& stderrStream () = 0;

    virtual void wait () = 0;
};

class Process {
public:
    static Process Run (const std::string& executable, std::span<const std::string> argv);

    std::uint32_t pid () const noexcept {
        PHENYL_DASSERT(m_process);
        return m_process->pid();
    }

    std::istream& stdoutStream () const {
        PHENYL_DASSERT(m_process);
        return m_process->stdoutStream();
    }

    std::istream& stderrStream () const {
        PHENYL_DASSERT(m_process);
        return m_process->stderrStream();
    }

    void wait () {
        PHENYL_DASSERT(m_process);
        return m_process->wait();
    }

private:
    explicit Process (std::unique_ptr<IProcess> process) : m_process{std::move(process)} {}

    std::unique_ptr<IProcess> m_process;
};
} // namespace phenyl::os
