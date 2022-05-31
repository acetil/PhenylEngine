#pragma once

#include "input_source.h"

namespace common {
    namespace detail {
        class Proxy;
    }
    class ProxySource : public InputSource {
    private:
        std::shared_ptr<InputSource> inputSource;
        std::shared_ptr<detail::Proxy> proxy;
    public:
        ProxySource (const std::shared_ptr<InputSource>& inputSource);
        ~ProxySource();
        long getInputNum(const std::string &inputStr) override;
        bool isDown(long inputNum) override;
        void consume(long inputNum) override;

        void consumeForProxies (long inputNum);

        void consumeForProxies (const std::string& inputStr);

        std::size_t getStateNum(long inputNum) override;

        std::shared_ptr<InputSource> getProxy ();
    };
}