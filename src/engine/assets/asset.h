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
#include <atomic>
#include <filesystem>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

// Quick way to create asset subclass constructor
#define DEFAULT_ASSET_CTOR(x) \
x (fs::path _target, System::JWindow& _win, JEngine& _engine) :  \
JAssetBase(_target, _win, _engine) \

// Quick way to create asset factory subclass constructor
#define DEFAULT_ASSET_FACTORY_CTOR(x, ...) \
x (System::JWindow& _win, JEngine& _engine) :  \
JAssetFactory(std::vector<std::string>{ __VA_ARGS__ }, _win, _engine) \

namespace January {
    namespace System {
        struct JWindow;
    }
    namespace Engine {
        struct JEngine;

        /**
         * @brief Handle asset related event
         */
        class JAssetEvent final {
        public:
            friend class JAssetWorker;
            JAssetEvent();
            virtual ~JAssetEvent();
            enum class JAssetEventType {
                CLEAN = 0,
                UPDATE = 1,
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
            std::mutex handle_mtx;
        };

        /**
         * @brief The basic handle for the asset
         */
        struct JAssetBase {
            JAssetBase() = delete;
            JAssetBase(fs::path _target, System::JWindow& _win, JEngine& _engine);
            virtual ~JAssetBase();
            /**
             * @brief Trying to load exists meta file4
             * 
             * @return load success or not
             */
            bool Load_Meta();
            bool Save_Meta();
            virtual bool Load_Data();
            virtual bool Save_Data();
            bool Delete();
            void Init();
            bool IsLoading();
            /**
             * @brief This will encode this asset instance into metadata json string
             * Normally this is for store in the .january/Assets folder content
             * 
             * @param pretty Print json pretty
             * @return std::string The json data
             */
            std::string Encode(bool pretty);
            /**
             * @brief Check file exist
             */
            bool Vaild();
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
            fs::path meta_target;
            /**
             * @brief Asset UUID
             */
            std::string uuid;

        protected:
            std::string QuickReadFile(std::string path);

        protected:
            System::JWindow&    jwindow;
            JEngine&            jengine;
            std::atomic_bool    loading = false;
            std::atomic_bool    loading_meta = false;
        };

        class JAssetFactory {
        public:
            JAssetFactory() = delete;
            JAssetFactory(System::JWindow& _win, JEngine& _engine);
            JAssetFactory(std::vector<std::string> _ext, System::JWindow& _win, JEngine& _engine);
            virtual ~JAssetFactory();
            virtual std::shared_ptr<JAssetBase> CreateAsset(fs::path path);
            virtual std::string GetResourceName();
            bool CheckExtension(std::string ext);
        protected:
            std::vector<std::string> extension = std::vector<std::string>();
            System::JWindow&    jwindow;
            JEngine&            jengine;
        };

        /**
         * @brief The entry point for the asset management
         * You could create factory to let engine recognize new asset type
         */
        class JAssetWorker final {
        public:
            JAssetWorker(System::JWindow& _win, JEngine& _engine);
            virtual ~JAssetWorker();
            /**
             * @brief Get the Asset Handler object
             * 
             * @param target The absolute path target from asset
             * @return JAssetBase 
             */
            std::shared_ptr<JAssetBase> GetJAssetHandler(fs::path target);
            /**
             * @brief Get Asset from UUID 
             * This only work when asset is load
             * 
             * @param uuid UUID target
             * @return JAssetBase Target instance
             */
            bool GetJAssetHandlerByUUID(std::string uuid, std::shared_ptr<JAssetBase>& asset);
            /**
             * @brief Check Asset is loaded
             * 
             * @param uuid UUID target
             * @return Asset checker result
             */
            bool IsJAssetHandlerLoaded(std::string uuid);
            void CleanLoadAsset();

            std::vector<std::string> GetAllResourceName();
            size_t GetResourceTypeLength();

            JAssetEvent EventHandler = JAssetEvent();
        protected:
            /**
             * @brief The UUID - Asset instance map
             */
            std::unordered_map<std::string, std::shared_ptr<JAssetBase>> loadedAssets = std::unordered_map<std::string, std::shared_ptr<JAssetBase>>();
            std::mutex la_mtx;
            /**
             * @brief The UUID - Asset factory instance map
             * The first one will be the default asset type
             */
            std::vector<std::shared_ptr<JAssetFactory>> loadedFactory = std::vector<std::shared_ptr<JAssetFactory>>();
            std::mutex lf_mtx;
        private:
            System::JWindow&    jwindow;
            JEngine&            jengine;
        };
    }
}
#endif