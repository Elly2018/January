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
#include "asset.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <spdlog/spdlog.h>
#include <uuid_v4.h>
#include "allasset.h"

namespace January::Engine {
    JAssetWorker::JAssetWorker(System::JWindow& _win, JEngine& _engine) : jwindow(_win), jengine(_engine) {
        loadedFactory.push_back(std::make_shared<JAssetFactory>(jwindow, jengine));
        loadedFactory.push_back(std::make_shared<JTextAssetFactory>(jwindow, jengine));
        loadedFactory.push_back(std::make_shared<JScriptAssetFactory>(jwindow, jengine));
    }

    JAssetWorker::~JAssetWorker(){
        std::lock_guard<std::mutex> lock(lf_mtx);
        std::lock_guard<std::mutex> lock2(la_mtx);
        loadedFactory.clear();
        loadedAssets.clear();
    }

    std::shared_ptr<JAssetBase> JAssetWorker::GetJAssetHandler(fs::path target){
        if(loadedFactory.size() == 0) return nullptr;
        std::string ext = target.extension().string();
        for(auto& f : loadedFactory){
            if(f->CheckExtension(ext)){
                return f->CreateAsset(target);
            }
        }
        return loadedFactory.at(0)->CreateAsset(target);
    }

    bool JAssetWorker::GetJAssetHandlerByUUID(std::string uuid, std::shared_ptr<JAssetBase>& asset){
        if(!IsJAssetHandlerLoaded(uuid)) return false;
        std::lock_guard<std::mutex> lock(la_mtx);
        asset = loadedAssets.at(uuid);
        return true;
    }

    bool JAssetWorker::IsJAssetHandlerLoaded(std::string uuid){
        std::lock_guard<std::mutex> lock(la_mtx);
        return loadedAssets.count(uuid);
    }

    void JAssetWorker::CleanLoadAsset(){
        std::lock_guard<std::mutex> lock(la_mtx);
        loadedAssets.clear();
    }
}