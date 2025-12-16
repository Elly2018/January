#include "engine.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include "utility/path.h"
#include "gui/mainframe.h"
#include "../system/cli.h"

using json = nlohmann::json;

// Global window access point
extern JWindow jwindow;
// Global engine access point
JEngine jengine;

#pragma region Private Utility

#pragma endregion

void SaveAppConfig(std::weak_ptr<AppConfig> target){
    fs::path home = get_home_directory();
}

std::shared_ptr<AppConfig> LoadAppConfig(){
    spdlog::debug("Try Load AppConfig");
    AppConfig config = AppConfig();
    fs::path home = get_home_directory();
    home = home.append("january");
    if(!fs::exists(home)){
        spdlog::warn("[home]/january not exist, create one right now");
        fs::create_directory(home);
    }
    home = home.append("config.json");
    if(!fs::exists(home)){
        spdlog::warn("Detect config.json not exist, create default one right now");
        json data = json();
        data["j_FPS"] = config.j_FPS;
        data["j_last_open"] = config.j_last_open;
        data["j_page_name"] = config.j_page_name;
        std::string dataString = data.dump(4) + "\n";
        
        std::ofstream outputFile(home);
        outputFile.write(dataString.c_str(), dataString.size());
        outputFile.close();
    }else{
        std::fstream i(home);
        json data = json::parse(i);
        config.j_FPS = data["j_FPS"];
        config.j_last_open = data["j_last_open"];
        config.j_page_name = data["j_page_name"];
    }

    return std::make_shared<AppConfig>(config);
}

std::shared_ptr<AppContext> GenerateAppContext(){
    spdlog::debug("Try Generate AppContext");
    AppContext ctx = AppContext();

    auto par_p = GetCMDParam("p", "path");
    if(par_p.has_value()){
        ctx.project_path = par_p.value();
        spdlog::info("App path setup: {}", ctx.project_path);
    }else{
        spdlog::warn("App path not setup");
    }
    return std::make_shared<AppContext>(ctx);
}

void EngineInit(){
    spdlog::debug("Engine Init");
    jengine = JEngine();
    jengine.config = LoadAppConfig();
    jengine.context = GenerateAppContext();

    // We check if page is first time fire
    JPageType page = jengine.config.get()->j_last_open;
    bool firstFire = UIPageFirstTimeFire(jengine, page);
    spdlog::debug("Check first fire page: {}", firstFire);
    if(firstFire){
        std::vector<JViewType> views = GetDefaultViewByPage(page);
        UIGenerateViews(jengine, views);
    }else{
        std::string pageName = jengine.config.get()->j_page_name;
        UILoadPageFromDisk(jengine, pageName);
    }

    if(fs::exists(jengine.context.get()->project_path)){
        std::string title = jengine.context.get()->project_path;
        title = "project: " + title;
        SDL_SetWindowTitle(jwindow.g_window, title.c_str());
    }else{
        std::string title = "January: a real-time interactive multimedia content creator";
        SDL_SetWindowTitle(jwindow.g_window, title.c_str());
    }
}

void EngineDeInit(){
    jengine.config.reset();
    for(auto view : jengine.context.get()->views){
        view.reset();
    }
    jengine.context.get()->views.clear();
    jengine.context.reset();
}

void EngineUpdate(){
    double current = ImGui::GetTime();
    jengine.context.get()->delta = current - jengine.context.get()->time;
    jengine.context.get()->time = current;

    for(auto view : jengine.context.get()->views){
        JViewBase* v = view.get();
        if(v != nullptr){
            v->Update();
        }
    }
}

void EngineDraw(){
    UIDraw(jwindow, jengine);
}