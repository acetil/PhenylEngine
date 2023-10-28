module;

#include <iostream>
#include <vector>

#include <logging/logging.h>
#include <util/string_help.h>

#include <phenyl/asset.h>
#include <phenyl/level.h>
#include <phenyl/ui/ui.h>
#include "phenyl/application.h"


export module debug_console;

using namespace phenyl;

/*static void handleProfiler (test::TestApp* app, const std::vector<std::string>& args) {
    if (args.empty()) {
        util::logging::log(LEVEL_WARNING, "Missing argument after \"profiler\"");
    } else if (args[0] == "display") {
        if (args.size() == 1 || (args[1] != "on" && args[1] != "off")) {
            util::logging::log(LEVEL_WARNING, R"(Unknown or missing argument after "display". Should be either "on" or "off")");
        } else {
            //bus->raise(event::ProfilerChangeEvent(args[1] == "on"));
            app->updateProfileRender(args[1] == "on");
        }
    } else {
        util::logging::log(LEVEL_WARNING, "Unknown argument: \"{}\"", args[0]);
    }
}

static void handleThemes (test::TestApp* app, std::vector<std::string>& args) {
    if (args.empty()) {
        util::logging::log(LEVEL_WARNING, "Missing argument after \"theme\"!");
    } else if (args[0] == "load") {
        if (args.size() != 2) {
            util::logging::log(LEVEL_WARNING, R"(Unknown argument after "theme".)");
        } else {
            //bus->raise(event::ChangeThemeEvent{args[1]});
            auto theme = phenyl::Assets::Load<phenyl::ui::Theme>(args[1]);
            if (!theme) {
                util::logging::log(LEVEL_ERROR, "Failed to load theme \"{}\"!", args[1]);
            } else {
                app->changeTheme(theme);
            }
        }
    }
}

static void doDebugConsole (test::TestApp* app) {
    std::cout << ">";
    std::string debugInput;
    std::getline(std::cin, debugInput);

    std::vector<std::string> args  = util::stringSplit(debugInput);

    std::string command = std::move(args[0]);
    args.erase(args.begin());

    if (command == "profiler") {
        handleProfiler(app, args);
    } else if (command == "level"){
        if (args.size() == 2 && args[0] == "dump") {
            //bus->raise(event::DumpMapEvent(args[1]));
            app->dumpLevel(args[1]);
        } else if (args.size() == 2 && args[0] == "load") {
            //bus->raise(event::MapLoadRequestEvent(args[1]));
            auto level = phenyl::Assets::Load<phenyl::Level>(args[1]);
            if (level) {
                level->load();
            }
        } else {
            util::logging::log(LEVEL_WARNING, "Unknown arguments for level command: \"{}\"", util::joinStrings(" ", args));
        }
    } else if (command == "theme") {
        handleThemes(app, args);
    } else if (command == "debug_render") {
        if (args.size() == 1) {
            if (args[0] == "true") {
                app->updateDebugRender(true);
            } else if (args[0] == "false") {
                app->updateDebugRender(false);
            } else {
                util::logging::log(LEVEL_WARNING, "Argument for debug_render command must be true or false");
            }
        } else {
            util::logging::log(LEVEL_WARNING, "debug_render command requires one argument: true/false");
        }
    } else if (command == "debug_step") {
        if (args.size() == 1) {
            if (args[0] == "true") {
                app->startStepping();
            } else if (args[0] == "false") {
                app->stopStepping();
            } else {
                util::logging::log(LEVEL_WARNING, "Argument for debug_step command must be true or false");
            }
        } else {
            util::logging::log(LEVEL_WARNING, "debug_step command requires one argument: true/false");
        }
    } else {
        util::logging::log(LEVEL_WARNING, "Unknown debug command: \"{}\"", command);
    }

}*/

namespace test {
    export void doDebugConsole (phenyl::Application* app) {
        //app->pause();
        //::doDebugConsole(app);
        //app->queueResume();
    }
}