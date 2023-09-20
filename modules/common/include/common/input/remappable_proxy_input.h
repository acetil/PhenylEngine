#pragma once

#include "forward.h"
#include "remappable_input.h"
#include "util/fake_bool.h"

namespace phenyl::common {
    class RemappableProxyInput : public RemappableInput {
    private:
        util::BoolVector isProxy;
    public:
        void addInputSource (const std::shared_ptr<InputSource>& source);
        void addInputSource (const std::shared_ptr<ProxySource>& proxySource);

        void consumeProxyInput (InputAction action);
    };
}
