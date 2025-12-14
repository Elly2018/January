#include "engine.h"
#include <filesystem>
#include <memory>
#include "../system/cli.h"
#include "gui/mainframe.h"

// Helping find the home path
#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace fs = std::filesystem;

static JEngine jengine = JEngine();

#pragma region Private Utility
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
#pragma endregion

void SaveAppConfig(std::weak_ptr<AppConfig> target){
    fs::path home = get_home_directory();
}

std::shared_ptr<AppConfig> LoadAppConfig(){
    fs::path home = get_home_directory();
    home = home.append("january");
    if(!fs::exists(home)){
        fs::create_directory(home);
    }
    home = home.append("config.json");
    if(!fs::exists(home)){
        fs::create_directory(home);
    }

    return std::make_shared<AppConfig>();
}

std::shared_ptr<AppContext> GenerateAppContext(){
    AppContext ctx = AppContext();
    auto cli = GetCMD();
    if(cli.params().contains("p")){
        ctx.project_path = cli.params().at("p");
    }
    else if (cli.params().contains("path")){
        ctx.project_path = cli.params().at("path");
    }
    return std::make_shared<AppContext>(ctx);
}

void EngineInit(){
    jengine.config = LoadAppConfig();
    jengine.context = GenerateAppContext();
}

void EngineDeInit(){
    jengine.config.reset();
    jengine.context.reset();
}

void EngineLoop(){

}

void EngineDraw(JWindow& win){
    UIDraw(win, jengine);
}