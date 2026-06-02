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
#include "script_asset.h"

namespace January::Engine {
    json JScriptAssetBase::EncodeHelper(){
        json buffer = JAssetBase::EncodeHelper();
        buffer["text"] = text;
        buffer["type"] = (int32_t)type;
        return buffer;
    }

    void JScriptAssetBase::Decode(json json){
        JAssetBase::Decode(json);
        if(json["text"].is_string()){
            text = json["text"].get<std::string>();
        }
        if(json["type"].is_number()){
            type = (ScriptType)json["type"].get<int32_t>();
        }
    }

    void JScriptAssetBase::Open() {

    }

    std::shared_ptr<JAssetBase> JScriptAssetFactory::CreateAsset(fs::path path) {
        auto b = std::make_shared<JScriptAssetBase>(path, jwindow, jengine);
        b->Init();
        return b;
    }

    std::string JScriptAssetFactory::GetResourceName(){
        return "Script";
    }
}