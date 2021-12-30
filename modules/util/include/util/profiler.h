#include <functional>

#include <string>

#ifndef PROFILER_H
#define PROFILER_H
namespace util {
    void setProfilerTimingFunction (std::function<double(void)> timeFunc);

    void startProfileFrame ();

    void endProfileFrame ();

    void startProfile (const std::string& category);

    void endProfile ();

    double getProfileTime (const std::string& category);

    double getProfileFrameTime ();

}
#endif