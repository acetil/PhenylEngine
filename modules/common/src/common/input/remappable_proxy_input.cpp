#include "common/input/remappable_proxy_input.h"
#include "common/input/proxy_source.h"

#include "logging/logging.h"

using namespace phenyl::common;

static phenyl::Logger LOGGER{"REMAPPABLE_PROXY_INPUT"};

void RemappableProxyInput::addInputSource (const std::shared_ptr<InputSource>& source) {
    RemappableInput::addInputSource(source);
    isProxy.emplace_back(false);
}

void RemappableProxyInput::addInputSource (const std::shared_ptr<ProxySource>& proxySource) {
    RemappableInput::addInputSource((const std::shared_ptr<InputSource>&) proxySource);
    isProxy.emplace_back(true);
}

void RemappableProxyInput::consumeProxyInput (InputAction action) {
    if (!action || getActionIndex(action) > actions.size()) {
        PHENYL_LOGE(LOGGER, "Invalid input action!");
        return;
    }

    detail::SourceAction sourceAction = actions[getActionIndex(action) - 1];

    if (isProxy[sourceAction.sourceIndex]) {
        auto* proxySource = (common::ProxySource*)inputSources[sourceAction.sourceIndex].get();
        proxySource->consumeForProxies(sourceAction.actionIndex);
    } else {
        PHENYL_LOGE(LOGGER, "Cannot consume proxied input for non-proxy source!");
    }
}