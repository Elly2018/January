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
#ifndef ENGINE_ASSETS_SCRIPT_ASSET_H
#define ENGINE_ASSETS_SCRIPT_ASSET_H

#include "asset.h"

namespace January::Engine {
    /**
     * @brief The handle for the text asset
     */
    struct JScriptAssetBase : public JAssetBase {
    public:
        DEFAULT_ASSET_CTOR(JScriptAssetBase) {}

        /**
         * @brief The script type
         * Affect how engine execute it
         */
        enum class ScriptType {
            /**
             * @brief Factory type 
             * Contain two method
             * @param FACTORY Custom factory worker
             * @param ASSET Pure asset, can be execute directly
             * @param ECS System-Component type of script
             */
            FACTORY = 0,
            ASSET = 1,
            ECS = 2,
        };

        static inline const char* ScriptTypeString[3] = {
            "Factory",
            "Asset",
            "ECS"
        };

        /**
         * @brief This will encode this asset instance into metadata json string
         * Normally this is for store in the ".january/Assets" folder content
         * 
         * @param pretty Print json pretty
         * @return std::string The json data
         */
        json EncodeHelper() override;
        /**
         * @brief Dump the json metadata and replace the value under this object
         * 
         * @param json Json to memory data
         */
        void Decode(json json) override;
        void Open() override;

        std::string text = "";
        ScriptType type = ScriptType::ASSET;
    };

    class JScriptAssetFactory : public JAssetFactory {
    public:
        DEFAULT_ASSET_FACTORY_CTOR(JScriptAssetFactory, ".gravity") {}

        std::shared_ptr<JAssetBase> CreateAsset(fs::path path) override;
        std::string GetResourceName() override;
    };
}

#endif