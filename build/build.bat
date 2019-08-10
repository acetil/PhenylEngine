@echo off
cmake .. -G "MinGW Makefiles" -Wdev -Werror=dev -DCMAKE_BUILD_TYPE=Debug
mingw32-make
