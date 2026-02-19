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

namespace January::Engine {
    void January::Engine::SaveAppConfig(std::weak_ptr<January::Engine::AppConfig> target){
        fs::path home = get_home_directory();
    }

    std::shared_ptr<January::Engine::AppConfig> January::Engine::LoadAppConfig(){
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

        return std::make_shared<January::Engine::AppConfig>(config);
    }

    std::shared_ptr<January::Engine::AppContext> January::Engine::GenerateAppContext(){
        spdlog::debug("Try Generate AppContext");
        AppContext ctx = AppContext();

        auto par_p = January::CLI::GetCMDParam("p", "path");
        if(par_p.has_value()){
            ctx.project_path = par_p.value();
            spdlog::info("App path setup: {}", ctx.project_path);
        }else{
            spdlog::warn("App path not setup");
        }
        return std::make_shared<AppContext>(ctx);
    }

    int32_t EngineInit(JEngine& jengine, JWindow& jwindow){
        spdlog::debug("Engine Init");
        JEngine jengine = JEngine();
        jengine.config = LoadAppConfig();
        jengine.context = GenerateAppContext();

        // We check if page is first time fire
        JPageType page = jengine.config.j_last_open;
        bool firstFire = UIPageFirstTimeFire(jengine, page);
        spdlog::debug("Check first fire page: {}", firstFire);
        if(firstFire){
            std::vector<JViewType> views = GetDefaultViewByPage(page);
            UIGenerateViews(jengine, views);
        }else{
            std::string pageName = jengine.config.j_page_name;
            UILoadPageFromDisk(jengine, pageName);
        }

        if(fs::exists(jengine.context.project_path)){
            std::string title = jengine.context.project_path;
            title = "project: " + title;
            SDL_SetWindowTitle(jwindow.g_window, title.c_str());
        }else{
            std::string title = "January: a real-time interactive multimedia content creator";
            SDL_SetWindowTitle(jwindow.g_window, title.c_str());
        }
        return 0;
    }

    void January::Engine::EngineDeInit(JEngine& jengine){
        for(auto view : jengine.context.views){
            view.reset();
        }
        jengine.context.views.clear();
    }

    void January::Engine::EngineUpdate(JEngine& jengine){
        double current = ImGui::GetTime();
        jengine.context.delta = current - jengine.context.time;
        jengine.context.time = current;

        for(auto view : jengine.context.views){
            January::Engine::View::JViewBase* v = view.get();
            if(v != nullptr){
                v->Update();
            }
        }
    }

    void January::Engine::EngineDraw(JEngine& jengine, JWindow& jwindow){
        UIDraw(jwindow, jengine);
    }
}