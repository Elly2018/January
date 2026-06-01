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

namespace January::Engine {
    JAssetFactory::JAssetFactory(std::vector<std::string> _ext, System::JWindow& _win, JEngine& _engine) 
        : extension(_ext), jwindow(_win), jengine(_engine) {}

    JAssetFactory::JAssetFactory(System::JWindow& _win, JEngine& _engine) 
        : jwindow(_win), jengine(_engine) {}

    JAssetFactory::~JAssetFactory(){}

    std::shared_ptr<JAssetBase> JAssetFactory::CreateAsset(fs::path path) {
        auto b = std::make_shared<JAssetBase>(path, jwindow, jengine);
        b->Init();
        return b;
    }

    bool JAssetFactory::CheckExtension(std::string ext) {
        for(auto& i : extension){
            if(i == ext) return true;
        }
        return false;
    }
}