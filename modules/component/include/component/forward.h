#pragma once

#include <cstddef>

#ifndef PHENYL_MAX_COMPONENTS
#define PHENYL_MAX_COMPONENTS 64
#endif

namespace util {
    class DataValue;
}

namespace component {
    template <std::size_t MaxComponents>
    class ComponentManager;
    template <std::size_t MaxComponents>
    class ComponentView;
    template <std::size_t MaxComponents, typename ...Args>
    class ConstrainedEntityView;
    template <std::size_t MaxComponents, typename ...Args>
    class ConstrainedView;

    template <size_t MaxComponents>
    class ObjectSerialiser;
    template <size_t MaxComponents>
    class ObjectComponentFactory;

    using EntityComponentManager = ComponentManager<PHENYL_MAX_COMPONENTS>;
    using EntityView = ComponentView<PHENYL_MAX_COMPONENTS>;

    using EntitySerialiser = ObjectSerialiser<PHENYL_MAX_COMPONENTS>;
    using EntityComponentFactory = ObjectComponentFactory<PHENYL_MAX_COMPONENTS>;
}