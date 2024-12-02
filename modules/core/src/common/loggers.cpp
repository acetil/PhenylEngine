#include "core/detail/loggers.h"

using namespace phenyl;

Logger core::detail::COMMON_LOGGER{"COMMON", PHENYL_LOGGER};

Logger core::detail::ASSETS_LOGGER{"ASSETS", COMMON_LOGGER};
Logger core::detail::ASSET_SERIALIZER_LOGGER{"ASSET_SERIALIZER", ASSETS_LOGGER};
Logger core::detail::SERIALIZER_LOGGER{"SERIALIZER", COMMON_LOGGER};

Logger core::detail::COMPONENT_LOGGER{"COMPONENT", PHENYL_LOGGER};
//Logger common::detail::SERIALIZER_LOGGER{"COMPONENT_SERIALIZER", COMPONENT_LOGGER};