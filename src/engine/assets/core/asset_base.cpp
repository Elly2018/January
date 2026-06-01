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
#include "../asset.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <spdlog/spdlog.h>
#include <uuid_v4.h>
#include "../../engine.h"
#include "../../struct/context.h"

namespace January::Engine {
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

    bool JAssetBase::Delete() {
        spdlog::info("Delete asset: {}", target.string());
        fs::path local_target = target;
        fs::path local_meta_target = meta_target;
        if(fs::exists(local_target.string())){
            std::thread([local_target]() {
                if(fs::is_directory(local_target.string())){
                    fs::remove_all(local_target);
                }else{
                    fs::remove(local_target);
                }
                spdlog::info("Delete asset success: {}", local_target.string());
            }).detach();
        }else{
            spdlog::warn("Asset file does not exist: {}", local_target.string());
        }
        if(fs::exists(local_meta_target.string() + ".json")){
            std::thread([local_meta_target]() {
                fs::remove(local_meta_target.string() + ".json");
                spdlog::info("Delete asset meta success: {}", local_meta_target.string() + ".json");
            }).detach();
        }else{
            spdlog::warn("Meta file does not exist: {}", local_meta_target.string() + ".json");
        }
        return true;
    }

    void JAssetBase::Init() {
        if(Vaild()){
            Load_Meta();
            Load_Data();
        }
    }

    bool JAssetBase::IsLoading() {
        return loading.load() || loading_meta.load();
    }

    std::string JAssetBase::Encode(bool pretty){
        std::string r = EncodeHelper().dump(pretty ? 4 : -1);
        // Use copy, so it become thread safe. i guess
        std::thread([=, this](){
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
        return fs::exists(target);
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
}