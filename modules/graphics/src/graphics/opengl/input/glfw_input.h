#pragma once

#include <vector>

#include "common/input/input_source.h"
#include "util/map.h"

namespace graphics {
    namespace detail {
        struct ButtonState;
    }
    class GLFWInput : public common::InputSource {
    private:
        util::Map<std::string, long> buttonNameMap;
        util::Map<int, long> buttonCodeMap;
        std::vector<detail::ButtonState> buttons;
    protected:
        void addButton (const std::string& button, int buttonCode);
    public:
        GLFWInput ();
        virtual ~GLFWInput();
        long getInputNum(const std::string &inputStr) override;
        bool isDown(long inputNum) override;
        void consume(long inputNum) override;
        std::size_t getStateNum(long inputNum) override;

        void onButtonChange (int code, int action, int mods);
    };
}