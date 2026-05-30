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
#pragma once
#ifndef ENGINE_ASSETS_ASSET_H
#define ENGINE_ASSETS_ASSET_H
#include <string>
#include <filesystem>
#include <functional>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace January::Engine {
    class JAssetEvent {
    public:
        enum class JAssetEventType {
            CLEAN = 0
        };
        struct RegisteredHandler {
            std::weak_ptr<void> token;
            std::function<void(const JAssetEventType, const std::string&)> callback;
        };

        using Handler = std::function<void(const JAssetEventType, const std::string&)>;

        [[nodiscard]] std::shared_ptr<void> Register(Handler handle);
        void Execute(JAssetEventType ev, const std::string message);
    private:
        std::vector<RegisteredHandler> Handlers = std::vector<RegisteredHandler>();
    };

    /**
     * @brief The basic handle for the asset
     */
    struct JAssetBase {
        /**
         * @brief This will encode this asset instance into metadata json string
         * Normally this is for store in the .january/Assets folder content
         * 
         * @param pretty Print json pretty
         * @return std::string The json data
         */
        std::string Encode(bool pretty);
        /**
         * @brief The inherit support for form json data
         * You can inherit this class and call this to get base json data
         * 
         * @return json The base form json data
         */
        virtual json EncodeHelper();
        /**
         * @brief Dump the json metadata and replace the value under this object
         * 
         * @param json Json to memory data
         */
        virtual void Decode(json json);
        /**
         * @brief Target path will lead you to project/Assets folder
         */
        fs::path target;
        /**
         * @brief Asset UUID
         */
        std::string uuid;
    };

    /**
     * @brief Get the Asset Handler object
     * 
     * @param target The relative path target from asset
     * @return JAssetBase 
     */
    JAssetBase GetJAssetHandler(fs::path target);
    /**
     * @brief Get Asset from UUID 
     * This only work when asset is load
     * 
     * @param uuid UUID target
     * @return JAssetBase Target instance
     */
    bool GetJAssetHandlerByUUID(std::string uuid, JAssetBase& asset);
    /**
     * @brief Check Asset is loaded
     * 
     * @param uuid UUID target
     * @return Asset checker result
     */
    bool IsJAssetHandlerLoaded(std::string uuid);
    void CleanLoadAsset();
}
#endif