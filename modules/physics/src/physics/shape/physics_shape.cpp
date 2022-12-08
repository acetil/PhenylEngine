#include <string>

#include "logging/logging.h"
#include "physics/shape/physics_shape.h"


using namespace physics;

void ShapeData::errExit (const std::string& msg) {
    logging::log(LEVEL_FATAL, msg);
    exit(1);
}
