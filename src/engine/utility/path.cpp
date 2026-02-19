/**
MIT License

Copyright (c) 2026 ZhuElly

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "path.h"

// Helping find the home path
#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

fs::path January::Engine::get_home_directory() {
    const char* homedir = nullptr;

#ifdef _WIN32
    homedir = std::getenv("USERPROFILE"); // Primary variable on Windows
    if (!homedir) {
        // Fallback for older Windows systems or unusual configurations
        const char* drive = std::getenv("HOMEDRIVE");
        const char* path = std::getenv("HOMEPATH");
        if (drive && path) {
            std::string fullpath = std::string(drive) + path;
            return fs::path(fullpath);
        }
    }
#else
    homedir = std::getenv("HOME"); // Common on Unix-like systems (Linux, macOS)
    if (!homedir) {
        // Fallback for cases where HOME environment variable is not set (e.g., some GUI apps on macOS, or specific system configurations)
        struct passwd *pw = getpwuid(getuid());
        if (pw) {
            homedir = pw->pw_dir;
        }
    }
#endif

    if (homedir) {
        return fs::path(homedir);
    }

    throw std::runtime_error("Could not determine home directory");
}