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
    JAssetEvent::JAssetEvent() = default;
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
}