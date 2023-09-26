#include "breakout.h"

#include <phenyl/engine.h>

int main (int argc, char* argv[]) {
    phenyl::PhenylEngine engine;
    engine.run<BreakoutApp>();
}