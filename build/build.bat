@echo off
cmake .. -G "MinGW Makefiles" -Wdev -Werror
mingw32-make
