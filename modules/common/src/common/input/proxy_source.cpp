#include "common/input/proxy_source.h"
#include "util/map.h"

#include <utility>

using namespace common;

namespace common::detail {
    class Proxy : public InputSource {
    private:
        std::shared_ptr<InputSource> inputSource;
        util::Map<long, std::size_t> consumed;
    public:
        explicit Proxy (std::shared_ptr<InputSource> _inputSource) : inputSource{std::move(_inputSource)} {}
        long getInputNum(const std::string &inputStr) override {
            return inputSource->getInputNum(inputStr);
        }

        bool isDown(long inputNum) override {
            if (consumed.contains(inputNum) && inputSource->getStateNum(inputNum) != consumed[inputNum]) {
                consumed.remove(inputNum);
            }

            return !consumed.contains(inputNum) && inputSource->isDown(inputNum);
        }

        void consume(long inputNum) override {
            consumed[inputNum] = inputSource->getStateNum(inputNum);
        }

        std::size_t getStateNum(long inputNum) override {
            return inputSource->getStateNum(inputNum);
        }
    };
}

ProxySource::ProxySource (const std::shared_ptr<InputSource>& _inputSource) : inputSource{_inputSource}, proxy{std::make_shared<detail::Proxy>(_inputSource)} {}

long ProxySource::getInputNum (const std::string& inputStr) {
    return inputSource->getInputNum(inputStr);
}

bool ProxySource::isDown (long inputNum) {
    return inputSource->isDown(inputNum);
}

void ProxySource::consume (long inputNum) {
    inputSource->consume(inputNum);
}

void ProxySource::consumeForProxies (long inputNum) {
    proxy->consume(inputNum);
}

void ProxySource::consumeForProxies (const std::string& inputStr) {
    consumeForProxies(getInputNum(inputStr));
}

std::size_t ProxySource::getStateNum (long inputNum) {
    return inputSource->getStateNum(inputNum);
}

std::shared_ptr<InputSource> ProxySource::getProxy () {
    return proxy;
}

ProxySource::~ProxySource () = default;
