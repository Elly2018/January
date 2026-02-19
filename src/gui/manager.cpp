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
#include "manager.h"
#include "view/blueprint.h"
#include "view/explorer.h"
#include "../engine/engine.h"
#include "../system/system.h"
#include "../system/window.h"

namespace January::Engine::View {
    void VInit(ViewManager& vm, struct System::JSystem& jsystem){
        vm.explorer = new JViewExplorer(std::string("Explorer##view"), (int32_t)JanuaryViewTypeFlag::GENERAL, (int32_t)JanuaryViewGeneralFlag::EXPLORER, *jsystem.window, *jsystem.engine);
        vm.blueprint = new JViewBlueprint("Blueprint##view", (int32_t)JanuaryViewTypeFlag::GENERAL, (int32_t)JanuaryViewGeneralFlag::BLUEPRINT, *jsystem.window, *jsystem.engine);
        vm.views.push_back(vm.explorer);
        vm.views.push_back(vm.blueprint);
    }

    void VDeInit(ViewManager& vm){
        for(auto& view : vm.views){
            delete view;
        }
        vm.views.clear();
    }

    void VSetEnable(ViewManager& vm, int32_t type, int32_t subtype, bool value){
        int64_t p = 0;
        int32_t* pt = (int32_t*)(&p);
        pt[0] = type;
        pt[1] = subtype;
        for(auto& view : vm.views){
            if(view->GetID() == p){
                view->SetEnable(value);
            }
        }
    }

    void VDraw(struct System::JWindow& win, struct JEngine& engine){

    }

    void VUpdate(struct System::JWindow& win, struct JEngine& engine){

    }
}
