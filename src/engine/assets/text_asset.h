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
#ifndef ENGINE_ASSETS_TEXT_ASSET_H
#define ENGINE_ASSETS_TEXT_ASSET_H
#include "asset.h"

namespace January::Engine {
    /**
     * @brief The handle for the text asset
     */
    struct JTextAssetBase : public JAssetBase {
        /**
         * @brief This will encode this asset instance into metadata json string
         * Normally this is for store in the .january/Assets folder content
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

        std::string text;
    };
}

#endif