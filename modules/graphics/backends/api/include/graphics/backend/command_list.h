#pragma once

namespace phenyl::graphics {
class ICommandList {
public:
    virtual ~ICommandList () = default;
};

class CommandList {
public:
    explicit CommandList (ICommandList& list) : m_list{list} {}

    [[nodiscard]] ICommandList& getUnderlying () {
        return m_list;
    }

private:
    ICommandList& m_list;
};
} // namespace phenyl::graphics
