#include "component/detail/loggers.h"

using namespace phenyl;

Logger component::detail::COMPONENT_LOGGER{"COMPONENT", PHENYL_LOGGER};
Logger component::detail::COMPONENT_SET_LOGGER{"COMPONENT_SET", COMPONENT_LOGGER};
Logger component::detail::ENTITY_LOGGER{"ENTITY", COMPONENT_LOGGER};
Logger component::detail::MANAGER_LOGGER{"COMPONENT_MANAGER", COMPONENT_LOGGER};
Logger component::detail::PREFAB_LOGGER{"PREFAB", COMPONENT_LOGGER};
Logger component::detail::QUERY_LOGGER{"COMPONENT_QUERY", COMPONENT_LOGGER};
Logger component::detail::SERIALIZER_LOGGER{"COMPONENT_SERIALIZER", COMPONENT_LOGGER};