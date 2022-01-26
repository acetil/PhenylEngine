#pragma once

#include <functional>

#include <string>

namespace util {
    void setProfilerTimingFunction (std::function<double(void)> timeFunc);

    void startProfileFrame ();

    void endProfileFrame ();

    void startProfile (const std::string& category);

    void endProfile ();

    double getProfileTime (const std::string& category);

    double getProfileFrameTime ();

}
