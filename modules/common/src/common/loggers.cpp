#include "common/detail/loggers.h"

using namespace phenyl;

Logger common::detail::COMMON_LOGGER{"COMMON"};

Logger common::detail::ASSETS_LOGGER{"ASSETS", COMMON_LOGGER};
Logger common::detail::ASSET_SERIALIZER_LOGGER{"ASSET_SERIALIZER", ASSETS_LOGGER};
Logger common::detail::SERIALIZER_LOGGER{"SERIALIZER", COMMON_LOGGER};