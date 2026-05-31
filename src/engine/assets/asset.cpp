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
#include "../engine.h"
#include "../struct/context.h"

namespace January::Engine {
    JAssetEvent::JAssetEvent(){

    }

    JAssetEvent::~JAssetEvent(){
        std::lock_guard<std::mutex> lock(handle_mtx);
        for(auto& i : Handlers){
            i.token.reset();
        }
        Handlers.clear();
    }

    std::shared_ptr<void> JAssetEvent::Register(Handler handle){
        std::shared_ptr<void> token = std::make_shared<int>(0);
        RegisteredHandler rh;
        rh.token = token; // weak_ptr implicitly created from shared_ptr
        rh.callback = handle;
        std::lock_guard<std::mutex> lock(handle_mtx);
        Handlers.push_back(rh);
        return token;
    }

    void JAssetEvent::Execute(JAssetEventType ev, const std::string message){
        std::lock_guard<std::mutex> lock(handle_mtx);
        Handlers.erase(
            std::remove_if(Handlers.begin(), Handlers.end(), [](const RegisteredHandler& rh) {
                return rh.token.expired(); 
            }), 
            Handlers.end()
        );
        for (const auto& handler : Handlers) {
            if (!handler.token.expired()) {
                handler.callback(ev, message);
            }
        }
    }

    JAssetBase::JAssetBase(fs::path _target, System::JWindow& _win, JEngine& _engine) : jwindow(_win), jengine(_engine) {
        target = _target;
        fs::path pro = jengine.context->project_path;
        fs::path relative_pro = fs::relative(_target, pro);
        fs::path b = jengine.context->project_path;
        b /= ".january";
        b /= relative_pro;
        meta_target = b;
    }

    JAssetBase::~JAssetBase(){}

    bool JAssetBase::Load_Meta(){
        loading_meta.store(true);
        spdlog::debug("Trying to load asset meta");
        spdlog::debug("\tPath: {}", meta_target.string());
        if(fs::exists(target.string()) && !fs::exists(meta_target.string() + ".json")){
            UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
            UUIDv4::UUID uuid_gen = uuidGenerator.getUUID();
            uuid = uuid_gen.str();
            spdlog::debug("Asset meta file not exists !");
            spdlog::debug("\tGenerate UUID: {}", uuid);
            Save_Meta();
        }
        if(fs::exists((meta_target.string() + ".json"))){
            std::string p = (meta_target.string() + ".json");
            std::ifstream file(p);
            spdlog::debug("Trying to store meta asset !");
            spdlog::debug("\tPath: {}", p);
            if (!file.is_open()) {
                spdlog::error("Could not open: {}", p);
                loading_meta.store(false);
                return false;
            }

            std::string json_text;
            std::string line;
            // Loop continues until EOF (End of File) or an error occurs
            while (std::getline(file, line)) {
                json_text += line;
                json_text += "\n";
            }

            try{
                json j = json::parse(json_text);
                Decode(j);
                loading_meta.store(false);
                return true;
            }
            catch(const json::parse_error& e){
                spdlog::error("JSON parse error in file {}: {} (at byte {})", meta_target.string(), e.what(), e.byte);
                loading_meta.store(false);
                return false;
            }
            catch (const json::exception& e) {
                spdlog::error("JSON data error in file {}: {}", meta_target.string(), e.what());
                loading_meta.store(false);
                return false;
            }
            catch (const std::exception& e) {
                spdlog::error("Standard exception reading meta file {}: {}", meta_target.string(), e.what());
                loading_meta.store(false);
                return false;
            }
        }
        loading_meta.store(false);
        return false;
    }

    bool JAssetBase::Save_Meta(){
        Encode(false);
        return true;
    }

    bool JAssetBase::Load_Data(){
        spdlog::debug("Trying to load asset");
        spdlog::debug("\tPath: {}", target.string());
        return true;
    }

    bool JAssetBase::Save_Data(){
        spdlog::debug("Trying to save asset");
        spdlog::debug("\tPath: {}", target.string());
        return true;
    }

    bool JAssetBase::IsLoading() {
        return loading.load() || loading_meta.load();
    }

    std::string JAssetBase::Encode(bool pretty){
        std::string r = EncodeHelper().dump(pretty ? 4 : -1);
        std::thread([&](){
            try{
                std::string p = (meta_target.string() + ".json");

                if (meta_target.has_parent_path()) {
                    fs::create_directories(meta_target.parent_path());
                }

                std::ofstream file(p);
                if (file.is_open()) {
                    file << r;
                }else{
                    spdlog::error("Meta file output error !");
                    spdlog::error("\tUUID: {}", uuid);
                    spdlog::error("\tPath: {}", target.string());
                    spdlog::error("\tMeta_Path: {}", p);
                    spdlog::error("\tData: {}", r);
                }
            }
            catch (const std::exception& e) {
                spdlog::error("Exception in file writer thread: {}", e.what());
            }
        }).detach();
        return r;
    }

    bool JAssetBase::Vaild(){
        return fs::exists(target) && fs::exists(meta_target);
    }

    json JAssetBase::EncodeHelper() {
        json buffer = json::object();
        buffer["uuid"] = uuid;
        return buffer;
    }

    void JAssetBase::Decode(json json){
        if(json["uuid"].is_string()){
            uuid = json["uuid"].get<std::string>();
        }
    }

    std::string JAssetBase::QuickReadFile(std::string path) {
        std::string result = "";
        std::fstream file(path);

        if(!file.is_open()){
            spdlog::error("Could not open: {}", path);
            loading.store(false);
            return "";
        }

        std::string line;
        // Loop continues until EOF (End of File) or an error occurs
        while (std::getline(file, line)) {
            result += line;
            result += "\n";
        }
        return result;
    }

    JAssetFactory::JAssetFactory(std::vector<std::string> _ext, System::JWindow& _win, JEngine& _engine) 
        : extension(_ext), jwindow(_win), jengine(_engine) {}

    JAssetFactory::JAssetFactory(System::JWindow& _win, JEngine& _engine) 
        : jwindow(_win), jengine(_engine) {}

    JAssetFactory::~JAssetFactory(){}

    std::shared_ptr<JAssetBase> JAssetFactory::CreateAsset(fs::path path) {
        auto b = std::make_shared<JAssetBase>(path, jwindow, jengine);
        b->Load_Meta();
        b->Load_Data();
        return b;
    }

    bool JAssetFactory::CheckExtension(std::string ext) {
        for(auto& i : extension){
            if(i == ext) return true;
        }
        return false;
    }

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