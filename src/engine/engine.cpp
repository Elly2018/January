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
#include <chrono>
#include <thread>
#include <imgui.h>
#include <imgui_notify.h>
#include <tahoma.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include "struct/config.h"
#include "struct/context.h"
#include "utility/path.h"
#include "../system/cli.h"
#include "../system/window.h"
#include "../gui/manager.h"
#include "../engine/utility/logger.h"
#include "../engine/assets/asset.h"

using json = nlohmann::json;

namespace January::Engine {

    std::chrono::steady_clock::time_point clock_start;
    std::chrono::steady_clock::time_point clock_now;
    std::chrono::steady_clock::time_point clock_last;

    fs::path get_config_path(const char* path){
        fs::path p = get_home_directory();
        p = p.append("january");
        spdlog::debug("Try Load AppConfig: {}", p.string());
        if(!fs::exists(p)){
            spdlog::warn("[home]/january not exist, create one right now");
            fs::create_directory(p);
        }
        p = p.append(path);
        return p;
    }

    void SaveAppConfig(struct AppConfig& target){
        fs::path p = get_config_path("config.json");
        json data = json::object();
        data["j_FPS"] = target.j_FPS;
        data["j_last_open"] = target.j_last_open;
        data["j_recent"] = json::array();
        for(auto& v : target.j_recent){
            json buffer = json::object();
            buffer["j_path"] = v.j_path;
            buffer["j_last_open"] = v.j_last_open;
            data["j_recent"].push_back(buffer);
        }
        data["j_views_enable"] = json::array();
        for(auto& v : target.j_views_enable){
            json buffer = json::object();
            buffer["id"] = v.first;
            buffer["enable"] = v.second;
            data["j_views_enable"].push_back(buffer);
        }
        std::string dataString = data.dump(4) + "\n";
        std::ofstream outputFile(p);
        spdlog::debug("SaveAppConfig: {}", p.string());
        outputFile.write(dataString.c_str(), dataString.size());
        outputFile.close();
    }

    void LoadAppConfig(struct AppConfig& config){
        fs::path p = get_config_path("config.json");
        if(!fs::exists(p)){
            spdlog::warn("Detect config.json not exist, create default one right now");
            SaveAppConfig(config);
        }else{
            std::fstream i(p);
            spdlog::debug("LoadAppConfig: {}", p.string());
            json data = json::parse(i);
            if(data["j_FPS"].is_number_integer()){
                config.j_FPS = data["j_FPS"].get<int32_t>();
            }
            if(data["j_last_open"].is_string()){
                config.j_last_open = data["j_last_open"].get<std::string>();
            }
            if(data["j_recent"].is_array()){
                config.j_recent.clear();
                for(int32_t i = 0; i < data["j_recent"].size(); i++){
                    json buffer = data["j_recent"].at(i);
                    if(buffer.is_object()){
                        if(buffer["j_path"].is_string() && buffer["j_last_open"].is_number_integer()){
                            AppConfigRecent recent;
                            recent.j_path = buffer["j_path"].get<std::string>();
                            recent.j_last_open = buffer["j_last_open"].get<long>();
                            config.j_recent.push_back(recent);
                        }
                    }
                }
            }
            if(data["j_views_enable"].is_array()){
                config.j_views_enable.clear();
                for(int32_t i = 0; i < data["j_views_enable"].size(); i++){
                    json buffer = data["j_views_enable"].at(i);
                    if(buffer.is_object()){
                        if(buffer["id"].is_number_integer() && buffer["enable"].is_boolean()){
                            config.j_views_enable.push_back(
                                std::pair<int64_t, bool>(
                                    buffer["id"].get<int64_t>(),
                                    buffer["enable"].get<bool>()
                                )
                            );
                        }
                    }
                }
            }
        }
    }

    void GenerateAppContext(struct AppContext& ctx){
        spdlog::debug("Try Generate AppContext");

        auto par_p = January::CLI::GetCMDParam("p", "path");
        if(par_p.has_value()){
            ctx.project_path = par_p.value();
            spdlog::info("App path setup: {}", ctx.project_path);
        }else{
            spdlog::warn("App path not setup");
        }
    }

    JEngine::JEngine() = default;
    JEngine::~JEngine() = default;

    int32_t EngineInit(JEngine& jengine, struct System::JWindow& jwindow, struct System::JSystem& system){
        spdlog::debug("Engine Init");
        jengine.config = std::make_unique<AppConfig>(); 
        jengine.context = std::make_unique<AppContext>();
        jengine.manager = std::make_unique<View::ViewManager>();
        jengine.context->logger = std::make_unique<JLogger>();
        jengine.context->asset = std::make_unique<JAssetWorker>(jwindow, jengine);
        LoadAppConfig(*jengine.config);
        GenerateAppContext(*jengine.context);

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImFontConfig font_cfg;
        font_cfg.FontDataOwnedByAtlas = false;
        std::string save_path = Engine::get_config_path("imgui.ini").string();
        io.IniFilename = save_path.c_str();
        jengine.context->text_font = io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 16.0f, &font_cfg);
        ImGui::MergeIconsWithLatestFont(16.f, false);
        jengine.context->icon_font = io.Fonts->AddFontFromFileTTF("icons.ttf", 16.0f, &font_cfg);
        ImGui::MergeIconsWithLatestFont(16.f, false);
        jengine.context->emoji_font = io.Fonts->AddFontFromFileTTF("NotoEmoji-VariableFont_wght.ttf", 16.0f, &font_cfg);
        ImGui::MergeIconsWithLatestFont(16.f, false);
#ifdef _WIN32
        io.Fonts->AddFontFromFileTTF("SourceHanSans-Medium.otf", 0.0f, NULL, io.Fonts->GetGlyphRangesDefault());
        io.Fonts->AddFontFromFileTTF("SourceHanSansK-Medium.otf", 0.0f, NULL, io.Fonts->GetGlyphRangesKorean());
        io.Fonts->AddFontFromFileTTF("SourceHanSansTC-Medium.otf", 0.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
#else
        if(fs::exists("SourceHanSans-Medium.otf")) io.Fonts->AddFontFromFileTTF("SourceHanSans-Medium.otf", 0.0f, NULL, io.Fonts->GetGlyphRangesDefault());
        else io.Fonts->AddFontFromFileTTF("/usr/share/fonts/truetype/SourceHanSans-Medium.otf", 0.0f, NULL, io.Fonts->GetGlyphRangesDefault());
        
        if(fs::exists("SourceHanSansK-Medium.otf")) io.Fonts->AddFontFromFileTTF("SourceHanSansK-Medium.otf", 0.0f, NULL, io.Fonts->GetGlyphRangesDefault());
        else io.Fonts->AddFontFromFileTTF("/usr/share/fonts/truetype/SourceHanSansK-Medium.otf", 0.0f, NULL, io.Fonts->GetGlyphRangesDefault());

        if(fs::exists("SourceHanSansTC-Medium.otf")) io.Fonts->AddFontFromFileTTF("SourceHanSansTC-Medium.otf", 0.0f, NULL, io.Fonts->GetGlyphRangesDefault());
        else io.Fonts->AddFontFromFileTTF("/usr/share/fonts/truetype/SourceHanSansTC-Medium.otf", 0.0f, NULL, io.Fonts->GetGlyphRangesDefault());

        {
            ImFontConfig font_cfg;
            font_cfg.FontDataOwnedByAtlas = false;
            io.Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 17.f, &font_cfg);

            // Initialize notify
            ImGui::MergeIconsWithLatestFont(16.f, false);
        }
#endif

        // Initialize notify
        io.Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 17.f, &font_cfg);
        ImGui::MergeIconsWithLatestFont(16.f, false);
        ImGuiToast toast = ImGuiToast(ImGuiToastType_Success, 3000);
        toast.set_title("Hello World!");
        toast.set_content("We can also format here:)");
        ImGui::InsertNotification(toast);

        VInit(*jengine.manager, system);
        spdlog::info("Views init finished");

        if(fs::exists(jengine.context->project_path)){
            std::string title = jengine.context->project_path;
            title = "project: " + title;
            SDL_SetWindowTitle(jwindow.g_window, title.c_str());
        }else{
            std::string title = "January: a real-time interactive multimedia content creator";
            SDL_SetWindowTitle(jwindow.g_window, title.c_str());
        }

        clock_start = std::chrono::steady_clock::now();
        clock_last = clock_start;
        return 0;
    }

    void EngineDeInit(JEngine& jengine){
        SaveEnableConfig(*jengine.manager, *jengine.config);
        SaveAppConfig(*jengine.config);
        System::SavePreference();

        VDeInit(*jengine.manager);
    }

    void EngineUpdate(JEngine& jengine){
        clock_now = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> total = clock_now - clock_start;
        std::chrono::duration<double, std::milli> elapsed = clock_now - clock_last;
        clock_last = clock_now;

        jengine.context->delta.store(elapsed.count(), std::memory_order_release);
        jengine.context->time.store(total.count(), std::memory_order_release);

        VUpdate(*jengine.manager);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    void AddRecent(JEngine& jengine, std::string& path){
        int32_t exist = -1;
        for(int32_t i = 0; i < jengine.config->j_recent.size(); i++){
            if(jengine.config->j_recent.at(i).j_path == path){
                exist = i;
                break;
            }
        }

        if(exist != -1){
            AppConfigRecent acr = AppConfigRecent();
            acr.j_path = jengine.config->j_recent.at(exist).j_path;
            jengine.config->j_recent.erase(jengine.config->j_recent.begin() + exist);
            jengine.config->j_recent.insert(jengine.config->j_recent.begin(), acr);
        }else{
            AppConfigRecent acr = AppConfigRecent();
            acr.j_path = path;
            jengine.config->j_recent.insert(jengine.config->j_recent.begin(), acr);
        }
        SaveAppConfig(*jengine.config);
    }
    void RemoveRecent(JEngine& jengine, std::string& path){
        int32_t exist = -1;
        for(int32_t i = 0; i < jengine.config->j_recent.size(); i++){
            if(jengine.config->j_recent.at(i).j_path == path){
                exist = i;
                break;
            }
        }
        if(exist != -1){
            jengine.config->j_recent.erase(jengine.config->j_recent.begin() + exist);
            SaveAppConfig(*jengine.config);
        }
    }
}
