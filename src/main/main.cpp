#include <phenyl/entrypoint.h>

int main (int argc, char* argv[]) {
    // TODO: parse cmdline args
    phenyl::PhenylEngine engine;
    phenyl::ApplicationProperties properties;
    phenyl_app_entrypoint(&engine, &properties);
}