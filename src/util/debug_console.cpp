#include <iostream>
#include <vector>

#include "debug_console.h"

#include "logging/logging.h"
#include "event/events/debug/profiler_change.h"

using namespace util;


std::vector<std::string> stringSplit (std::string& str) {
    std::vector<std::string> substrings;

    std::size_t start = 0;
    auto end = std::string::npos;

    while ((end = str.find(' ', start)) != std::string::npos) {
        substrings.emplace_back(str.substr(start, end - start));
        start = end + 1;
    }
    substrings.emplace_back(str.substr(start));
    return substrings;
}

void handleProfiler (event::EventBus* bus, std::vector<std::string>& args) {
    if (args.empty()) {
        logging::log(LEVEL_WARNING, "Missing argument after \"profiler\"");
    } else if (args[0] == "display") {
        if (args.size() == 1 || (args[1] != "on" && args[1] != "off")) {
            logging::log(LEVEL_WARNING, R"(Unknown or missing argument after "display". Should be either "on" or "off")");
        } else {
            bus->raiseEvent(event::ProfilerChangeEvent(args[1] == "on"));
        }
    } else {
        logging::log(LEVEL_WARNING, "Unknown argument: \"{}\"", args[0]);
    }
}

void util::doDebugConsole (event::EventBus* bus) {
    std::cout << ">";
    std::string debugInput;
    std::getline(std::cin, debugInput);

    std::vector<std::string> args  = stringSplit(debugInput);

    std::string command = std::move(args[0]);
    args.erase(args.begin());

    if (command == "profiler") {
        handleProfiler(bus, args);
    } else {
        logging::log(LEVEL_ERROR, "Unknown debug command: \"{}\"", command);
    }

}