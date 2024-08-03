#include "runtime/runtime.h"
#include "runtime/system.h"

using namespace phenyl::runtime;

void IRunnableSystem::run (PhenylRuntime& runtime) {
    run(runtime.manager(), runtime.resourceManager);
}
