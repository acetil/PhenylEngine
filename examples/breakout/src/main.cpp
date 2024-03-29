#include "breakout.h"

#include <phenyl/engine.h>

using namespace breakout;

int main (int argc, char* argv[]) {
    phenyl::PhenylEngine engine;
    engine.run<BreakoutApp>(phenyl::ApplicationProperties{}
        .withLogFile("debug.log")
        .withRootLogLevel(LEVEL_DEBUG)
        .withResolution(800, 600)
        .withWindowTitle("Breakout!")
        .withVsync(false));
}