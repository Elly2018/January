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
#include <unordered_map>
#include <mutex>

namespace January::Engine {
    /**
     * @brief The UUID - Asset instance map
     */
    std::unordered_map<std::string, JAssetBase> loadedAssets = std::unordered_map<std::string, JAssetBase>();
    std::mutex la_mtx;

    std::shared_ptr<void> JAssetEvent::Register(Handler handle){
        std::shared_ptr<void> token = std::make_shared<int>(0);
        RegisteredHandler rh;
        rh.token = token; // weak_ptr implicitly created from shared_ptr
        rh.callback = handle;
        Handlers.push_back(rh);
        return token;
    }

    void JAssetEvent::Execute(JAssetEventType ev, const std::string message){
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

    std::string JAssetBase::Encode(bool pretty){
        return EncodeHelper().dump(pretty ? 4 : -1);
    }

    json JAssetBase::EncodeHelper() {
        json buffer = json::object();
        buffer["uuid"] = uuid;
        return buffer;
    }

    void JAssetBase::Decode(json json){
        if(json["uuid"].is_string()){
            uuid = json.get<std::string>();
        }
    }

    JAssetBase GetJAssetHandler(fs::path target){
        std::string ext = target.extension().string();
        if(ext == ".txt"){

        }
    }

    bool GetJAssetHandlerByUUID(std::string uuid, JAssetBase& asset){
        if(!IsJAssetHandlerLoaded(uuid)) return false;
        std::lock_guard<std::mutex> lock(la_mtx);
        asset = loadedAssets.at(uuid);
        return true;
    }

    bool IsJAssetHandlerLoaded(std::string uuid){
        std::lock_guard<std::mutex> lock(la_mtx);
        return loadedAssets.count(uuid);
    }

    void CleanLoadAsset(){
        std::lock_guard<std::mutex> lock(la_mtx);
        loadedAssets.clear();
    }
}