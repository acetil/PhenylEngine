#include <iostream>
#include <vector>

#include "util/debug_console.h"

#include "logging/logging.h"
#include "common/events/debug/profiler_change.h"
#include "common/events/debug/reload_map.h"
#include "common/events/debug/dump_map.h"
#include "common/events/debug/reload_theme.h"
#include "common/events/theme_change.h"
#include "util/string_help.h"
#include "common/events/debug/debug_render.h"
#include "common/events/debug/debug_pause.h"

#include "common/assets/assets.h"
#include "engine/level/level.h"
#include "common/events/debug/debug_step.h"

using namespace util;


/*std::vector<std::string> stringSplit (std::string& str) {
    std::vector<std::string> substrings;

    std::size_t start = 0;
    auto end = std::string::npos;

    while ((end = str.find(' ', start)) != std::string::npos) {
        substrings.emplace_back(str.substr(start, end - start));
        start = end + 1;
    }
    substrings.emplace_back(str.substr(start));
    return substrings;
}*/

static void handleProfiler (const event::EventBus::SharedPtr& bus, const std::vector<std::string>& args) {
    if (args.empty()) {
        logging::log(LEVEL_WARNING, "Missing argument after \"profiler\"");
    } else if (args[0] == "display") {
        if (args.size() == 1 || (args[1] != "on" && args[1] != "off")) {
            logging::log(LEVEL_WARNING, R"(Unknown or missing argument after "display". Should be either "on" or "off")");
        } else {
            bus->raise(event::ProfilerChangeEvent(args[1] == "on"));
        }
    } else {
        logging::log(LEVEL_WARNING, "Unknown argument: \"{}\"", args[0]);
    }
}

static void handleThemes (const event::EventBus::SharedPtr& bus, std::vector<std::string>& args) {
    if (args.empty()) {
        logging::log(LEVEL_WARNING, "Missing argument after \"theme\"!");
    } else if (args[0] == "reload") {
        bus->raise(event::ReloadThemeEvent{});
    } else if (args[0] == "load") {
        if (args.size() != 2) {
            logging::log(LEVEL_WARNING, R"(Unknown argument after "theme".)");
        } else {
            bus->raise(event::ChangeThemeEvent{args[1]});
        }
    }
}

static void doDebugConsole (event::EventBus::SharedPtr bus) {
    std::cout << ">";
    std::string debugInput;
    std::getline(std::cin, debugInput);

    std::vector<std::string> args  = stringSplit(debugInput);

    std::string command = std::move(args[0]);
    args.erase(args.begin());

    if (command == "profiler") {
        handleProfiler(bus, args);
    } else if (command == "level"){
        /*if (args.size() == 1 && args[0] == "reload") {
            bus->raise(event::ReloadMapEvent());
        } else*/ if (args.size() == 2 && args[0] == "dump") {
            bus->raise(event::DumpMapEvent(args[1]));
        } else if (args.size() == 2 && args[0] == "load") {
            //bus->raise(event::MapLoadRequestEvent(args[1]));
            auto level = common::Assets::Load<game::Level>(args[1]);
            if (level) {
                level->load();
            }
        } else {
            logging::log(LEVEL_WARNING, "Unknown arguments for level command: \"{}\"", util::joinStrings(" ", args));
        }
    } else if (command == "theme") {
        handleThemes(bus, args);
    } else if (command == "debug_render") {
        if (args.size() == 1) {
            if (args[0] == "true") {
                bus->raise(event::DebugRenderEvent{true});
            } else if (args[0] == "false") {
                bus->raise(event::DebugRenderEvent{false});
            } else {
                logging::log(LEVEL_WARNING, "Argument for debug_render command must be true or false");
            }
        } else {
            logging::log(LEVEL_WARNING, "debug_render command requires one argument: true/false");
        }
    } else if (command == "debug_step") {
        if (args.size() == 1) {
            if (args[0] == "true") {
                bus->raise(event::DebugStepEvent{.status=event::DebugStepStatus::ENABLE_STEPPING});
            } else if (args[0] == "false") {
                bus->raise(event::DebugStepEvent{.status=event::DebugStepStatus::DISABLE_STEPPING});
            } else {
                logging::log(LEVEL_WARNING, "Argument for debug_step command must be true or false");
            }
        } else {
            logging::log(LEVEL_WARNING, "debug_step command requires one argument: true/false");
        }
    } else {
        logging::log(LEVEL_WARNING, "Unknown debug command: \"{}\"", command);
    }

}

void util::doDebugConsole (DebugConsoleEvent& event) {
    event::EventBus::SharedPtr eventBus;
    if (!(eventBus = event.eventBus.lock())) {
        logging::log(LEVEL_WARNING, "Debug console event bus pointer invalid!");
    } else {
        eventBus->raise(event::DebugPauseEvent{true});
        ::doDebugConsole(eventBus);
    }
}