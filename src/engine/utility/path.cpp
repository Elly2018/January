#include "path.h"

// Helping find the home path
#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

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