#pragma once

#include <string>

#include "util/smart_help.h"

namespace phenyl::common {
    class InputSource : util::SmartHelper<InputSource, true> {
    public:
        virtual long getInputNum (const std::string& inputStr) = 0; // -1 on failure
        virtual bool isDown (long inputNum) = 0;
        bool isDown (const std::string& inputStr) {
            return isDown(getInputNum(inputStr));
        };


        virtual void consume (long inputNum) = 0;
        void consume (const std::string& inputStr) {
            consume(getInputNum(inputStr));
        }

        virtual std::size_t getStateNum (long inputNum) = 0;
    };
}