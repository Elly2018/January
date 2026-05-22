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
#include <stdlib.h>
#include <spdlog/spdlog.h>
#include <ini.h>
// Helping find the home path
#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace January::Engine {
    fs::path get_home_directory() {
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

    fs::path get_temp_directory(){
        fs::path p = get_home_directory();
        p /= "january";
        if(!fs::exists(p)) fs::create_directories(p);
        return p;
    }

    bool is_project_path_vaild(const fs::path& path){
        if(!fs::exists(path)) {
            spdlog::warn("Project path not exist: {}", path.c_str());
            return false;
        }
        fs::path manifest = path;
        manifest /= MANIFEST_FILENAME;
        if(!fs::exists(manifest)) {
            spdlog::warn("{} file not exist: {}", MANIFEST_FILENAME, manifest.c_str());
            return false;
        }
        mINI::INIFile mani(manifest);
        mINI::INIStructure ini;
        mani.read(ini);

        std::string& name = ini["header"]["name"];
        std::string& description = ini["header"]["description"];
        std::string& version = ini["header"]["version"];

        spdlog::info("{} loaded: {} {} {}", MANIFEST_FILENAME, name.c_str(), description.c_str(), version.c_str());

        return true;
    }
}