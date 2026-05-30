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
#include "text_asset.h"
#include <fstream>
#include <spdlog/spdlog.h>

namespace January::Engine {
    bool JTextAssetBase::Load_Data(){
        loading.store(true);
        if(fs::exists(target)){
            std::string p = target.string();
            std::fstream file(p);

            if(!file.is_open()){
                spdlog::error("Could not open: {}", p);
                loading.store(false);
                return false;
            }

            text.clear();
            std::string line;
            // Loop continues until EOF (End of File) or an error occurs
            while (std::getline(file, line)) {
                text += line;
                text += "\n";
            }
            loading.store(true);
            return true;
        }
        loading.store(false);
        return false;
    }

    bool JTextAssetBase::Save_Data(){
        std::thread([&](){
            std::string p = target.string();

            if (target.has_parent_path()) {
                fs::create_directories(target.parent_path());
            }

            std::ofstream file(p);
            if (file.is_open()) {
                file << text.c_str();
            }else{
                spdlog::error("Text file output error !");
                spdlog::error("\tUUID: {}", uuid);
                spdlog::error("\tPath: {}", target.string());
                spdlog::error("\tMeta_Path: {}", meta_target.string());
            }
        }).detach();
        return true;
    }

    std::shared_ptr<JAssetBase> JTextAssetFactory::CreateAsset(fs::path path) {
        return std::make_shared<JTextAssetBase>(path, jwindow, jengine);
    }
}