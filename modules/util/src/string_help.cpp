#include "util/string_help.h"

#include "logging/logging.h"

#include <cctype>
#include <sstream>

using namespace phenyl;

std::string_view util::StringTrim (std::string_view s) {
    auto startIt = s.begin();
    auto endIt = s.end();

    while (startIt != endIt && std::isspace(*startIt)) {
        ++startIt;
    }

    while (endIt != startIt && std::isspace(*std::prev(endIt))) {
        --endIt;
    }

    return std::string_view{startIt, endIt};
}

util::detail::StringSplitter util::StringSplit (std::string_view s, std::string_view delim, std::size_t maxSplits) {
    return detail::StringSplitter{s, delim, maxSplits};
}

static_assert(std::ranges::range<util::detail::StringSplitter>);

util::detail::StringSplitter::StringSplitter (std::string_view s, std::string_view delim, std::size_t maxSplits) :
    m_str{s},
    m_delim{delim},
    m_maxSplits{maxSplits} {}

util::detail::StringSplitter::iterator util::detail::StringSplitter::begin () const {
    return Iterator{this};
}

util::detail::StringSplitter::sentinel util::detail::StringSplitter::end () const {
    return Sentinel{};
}

util::detail::StringSplitter::Iterator::Iterator (const StringSplitter* splitter) : m_splitter{splitter} {
    findNext();
}

std::string_view util::detail::StringSplitter::Iterator::operator* () const {
    return *m_curr;
}

util::detail::StringSplitter::Iterator& util::detail::StringSplitter::Iterator::operator++ () {
    findNext();
    return *this;
}

util::detail::StringSplitter::Iterator util::detail::StringSplitter::Iterator::operator++ (int) {
    auto copy = *this;
    ++*this;

    return copy;
}

bool util::detail::StringSplitter::Iterator::operator== (const Iterator& other) const noexcept {
    return m_splitter == other.m_splitter && m_pos == other.m_pos;
}

bool util::detail::StringSplitter::Iterator::operator== (const Sentinel& other) const noexcept {
    return m_pos == std::string_view::npos && !m_curr;
}

void util::detail::StringSplitter::Iterator::findNext () {
    PHENYL_DASSERT(m_splitter);
    if (m_pos == std::string_view::npos || m_pos == m_splitter->m_str.size()) {
        m_curr = std::nullopt;
        return;
    }

    if (m_numSplits + 1 == m_splitter->m_maxSplits) {
        m_curr = m_splitter->m_str.substr(m_pos);
        m_pos = std::string_view::npos;
    } else {
        auto end = m_splitter->m_str.find(m_splitter->m_delim, m_pos);

        if (end != std::string_view::npos) {
            m_curr = m_splitter->m_str.substr(m_pos, end - m_pos);
            m_pos = end + m_splitter->m_delim.size();
        } else {
            m_curr = m_splitter->m_str.substr(m_pos);
            m_pos = std::string_view::npos;
        }
    }

    m_numSplits++;
}
