#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "util/profiler.h"
#include "logging/logging.h"

using namespace phenyl;

struct Profiler {
      double lastFrameTime = 0;
      double frameStartTime = 0;

      std::unordered_map<std::string, double> lastProfileTime;

      std::unordered_map<std::string, double> currentProfileTime;

      std::unordered_map<std::string, double> profileStartTime;

      std::vector<std::string> activeCategories;

      std::unordered_set<std::string> activeSet;

      std::function<double(void)> timeFunc;

};

static Profiler profiler;

void util::setProfilerTimingFunction (std::function<double ()> timeFunc) {
    profiler.timeFunc = std::move(timeFunc);
}

void util::startProfileFrame () {
    profiler.frameStartTime = profiler.timeFunc();
}

void util::endProfileFrame () {
    double time = profiler.timeFunc();
    profiler.lastFrameTime = time - profiler.frameStartTime;

    for (auto& i : profiler.activeCategories) {
        profiler.currentProfileTime[i] += time - profiler.profileStartTime[i];
    }

    profiler.lastProfileTime = profiler.currentProfileTime;

    for (auto& i : profiler.currentProfileTime) {
        i.second = 0;
    }

    profiler.activeCategories.clear();
    profiler.activeSet.clear();
}

void util::startProfile (const std::string& category) {
    if (!profiler.activeSet.contains(category)) {
        profiler.activeCategories.emplace_back(category);
        profiler.activeSet.emplace(category);

        profiler.profileStartTime[category] = profiler.timeFunc();
        if (!profiler.currentProfileTime.contains(category)) {
            profiler.currentProfileTime[category] = 0;
        }
    }
}

void util::endProfile () {
    auto time = profiler.timeFunc();
    if (profiler.activeSet.empty()) {
        logging::log(LEVEL_WARNING, "Profiler has not active categories to end!");
    } else {
        auto category = profiler.activeCategories.back();

        profiler.currentProfileTime[category] += time - profiler.profileStartTime[category];

        profiler.activeSet.erase(category);

        profiler.activeCategories.pop_back();
    }
}

double util::getProfileTime (const std::string& category) {
    return profiler.lastProfileTime[category];
}

double util::getProfileFrameTime () {
    return profiler.lastFrameTime;
}