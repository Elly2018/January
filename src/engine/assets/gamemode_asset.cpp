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
#include "gamemode_asset.h"
#include <fstream>
#include <spdlog/spdlog.h>

namespace January::Engine {
    bool JGamemodeAssetBase::Load_Data(){
        JAssetBase::Load_Data();
        loading.store(true);
        if(fs::exists(target)){
            std::string text = QuickReadFile(target.string());
            json j = json::parse(text);
            if(j["auto_start"].is_boolean()){
                auto_start.store(j["auto_start"].get<bool>());
            }
            if(j["auto_sync"].is_boolean()){
                auto_sync.store(j["auto_sync"].get<bool>());
            }
            loading.store(true);
            return true;
        }
        loading.store(false);
        return false;
    }

    bool JGamemodeAssetBase::Save_Data(){
        JAssetBase::Save_Data();
        json j;
        j["auto_start"] = auto_start.load();
        j["auto_sync"] = auto_sync.load();

        std::thread([&](){
            std::string p = target.string();

            if (target.has_parent_path()) {
                fs::create_directories(target.parent_path());
            }

            std::ofstream file(p);
            if (file.is_open()) {
                file << j.dump().c_str();
            }else{
                spdlog::error("Gamemode file output error !");
                spdlog::error("\tUUID: {}", uuid);
                spdlog::error("\tPath: {}", target.string());
                spdlog::error("\tMeta_Path: {}", meta_target.string());
            }
        }).detach();
        return true;
    }

    std::shared_ptr<JAssetBase> JGamemodeAssetFactory::CreateAsset(fs::path path) {
        auto b = std::make_shared<JGamemodeAssetBase>(path, jwindow, jengine);
        b->Load_Meta();
        b->Load_Data();
        return b;
    }
}