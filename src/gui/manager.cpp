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
#include "view/console.h"
#include "view/explorer.h"
#include "view/inspector.h"
#include "view/preview.h"
#include "view/profiler.h"
#include "view/script.h"
#include "view/timeline.h"
#include "view/volumn.h"
#include "popup/file_dialog.h"
#include "popup/project_dashboard.h"
#include "../engine/engine.h"
#include "../engine/struct/config.h"
#include "../engine/struct/context.h"
#include "../engine/utility/command.h"
#include "../system/system.h"
#include "../system/window.h"

namespace January::Engine::View {
    void VInit(ViewManager& vm, struct System::JSystem& jsystem){
        vm.blueprint = new JViewBlueprint(std::string("Blueprint##view"), (int32_t)JanuaryViewTypeFlag::GENERAL, (int32_t)JanuaryViewGeneralFlag::BLUEPRINT, *jsystem.window, *jsystem.engine);
        vm.console = new JViewConsole(std::string("Console##view"), (int32_t)JanuaryViewTypeFlag::GENERAL, (int32_t)JanuaryViewGeneralFlag::CONSOLE, *jsystem.window, *jsystem.engine);
        vm.explorer = new JViewExplorer(std::string("Explorer##view"), (int32_t)JanuaryViewTypeFlag::GENERAL, (int32_t)JanuaryViewGeneralFlag::EXPLORER, *jsystem.window, *jsystem.engine);
        vm.inspector = new JViewInspector(std::string("Inspector##view"), (int32_t)JanuaryViewTypeFlag::GENERAL, (int32_t)JanuaryViewGeneralFlag::INSPECTOR, *jsystem.window, *jsystem.engine);
        vm.preview = new JViewPreview(std::string("Preview##view"), (int32_t)JanuaryViewTypeFlag::RENDER, (int32_t)JanuaryViewRenderFlag::PREVIEW, *jsystem.window, *jsystem.engine);
        vm.profiler = new JViewProfiler(std::string("Profiler##view"), (int32_t)JanuaryViewTypeFlag::DEBUG, (int32_t)JanuaryViewDebugFlag::PROFILER, *jsystem.window, *jsystem.engine);
        vm.script = new JViewScript(std::string("Script##view"), (int32_t)JanuaryViewTypeFlag::SCRIPT, (int32_t)JanuaryViewScriptFlag::SCRIPT, *jsystem.window, *jsystem.engine);
        vm.timeline = new JViewTimeline(std::string("Timeline##view"), (int32_t)JanuaryViewTypeFlag::ANIMATION, (int32_t)JanuaryViewAnimationFlag::TIMELINE, *jsystem.window, *jsystem.engine);
        vm.volumn = new JViewVolumn(std::string("Volumn##view"), (int32_t)JanuaryViewTypeFlag::AUDIO, (int32_t)JanuaryViewAudioFlag::VOLUMN, *jsystem.window, *jsystem.engine);

        vm.project_dashboard = new JPopupProjectDashboard(std::string("Project Dashboard##popup"), (int32_t)JanuaryViewTypeFlag::GENERAL | (int32_t)JanuaryViewTypeFlag::POPUP, (int32_t)JanuaryViewGeneralPopupFlag::PROJECT_DASHBOARD, *jsystem.window, *jsystem.engine, 0);
        vm.file_dialog = new JPopupFileDialog(std::string("File Dialog##popup"), (int32_t)JanuaryViewTypeFlag::GENERAL | (int32_t)JanuaryViewTypeFlag::POPUP, (int32_t)JanuaryViewGeneralPopupFlag::FILE_DIALOG, *jsystem.window, *jsystem.engine, 1);
        
        vm.views.push_back(vm.blueprint);
        vm.views.push_back(vm.console);
        vm.views.push_back(vm.explorer);
        vm.views.push_back(vm.inspector);
        vm.views.push_back(vm.preview);
        vm.views.push_back(vm.profiler);
        vm.views.push_back(vm.script);
        vm.views.push_back(vm.timeline);
        vm.views.push_back(vm.volumn);

        vm.popups.push_back(vm.project_dashboard);
        vm.popups.push_back(vm.file_dialog);

        for(auto& c : vm.views){
            c->Init();
        }
        for(auto& c : vm.popups){
            c->Init();
        }
    }

    void VDeInit(ViewManager& vm){
        for(auto& c : vm.views){
            c->DeInit();
            delete c;
        }
        for(auto& c : vm.popups){
            c->DeInit();
            delete c;
        }
        vm.views.clear();
        vm.popups.clear();
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

    void SaveEnableConfig(ViewManager& vm, struct AppConfig& config){
        config.j_views_enable.clear();
        for(auto& view : vm.views){
            config.j_views_enable.push_back(std::pair<int64_t, bool>(
                view->GetID(),
                view->IsEnable()
            ));
        }
    }

    void ApplyEnableConfig(ViewManager& vm, struct AppConfig& config){
        std::unordered_map<int64_t, bool> mmap = std::unordered_map<int64_t, bool>();
        for(auto& ve : config.j_views_enable){
            mmap.insert_or_assign(ve.first, ve.second);
        }
        for(auto& view : vm.views){
            if(mmap.count(view->GetID())){
                view->SetEnable(mmap.at(view->GetID()));
            }
        }
    }

    void VDraw(ViewManager& vm, System::JWindow& jwindow, JEngine& jengine){
        if(ImGui::BeginMainMenuBar()){
            // File bar
            if(ImGui::BeginMenu("File##MainMenuBar")){
                if(ImGui::MenuItem("New Project##MainMenuBar_File", "Ctrl+N")){
                    PushCommand(*jengine.context, "new_project");
                }
                if(ImGui::MenuItem("Open Project##MainMenuBar_File", "Ctrl+O")){
                    PushCommand(*jengine.context, "open_project");
                }
                if(jengine.config->j_recent.size() > 0){
                    if(ImGui::BeginMenu("Open Recent##MainMenuBar_File")){
                        for(auto& recent : jengine.config->j_recent){
                            if(ImGui::MenuItem((recent.j_path + "##MainMenuBar_File_Recent").c_str())){
                                PushCommand(*jengine.context, std::string("open_recent ") + recent.j_path);
                            }
                        }
                        ImGui::EndMenu();
                    }
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Save Project##MainMenuBar_File", "Ctrl+S")){
                    PushCommand(*jengine.context, "save_project");
                }
                if(ImGui::MenuItem("Save Project As##MainMenuBar_File", "Ctrl+Shift+S")){
                    PushCommand(*jengine.context, "save_project_as");
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Setting##MainMenuBar_File")){
                    PushCommand(*jengine.context, "open_setting");
                }
                if(ImGui::MenuItem("Preference##MainMenuBar_File")){
                    PushCommand(*jengine.context, "open_preference");
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Quit##MainMenuBar_File")){
                    jwindow.g_done = true;
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("View##MainMenuBar")){
                if(ImGui::BeginMenu("Workspace##MainMenuBar_View")){
                    if(ImGui::MenuItem("Add Current Workspace")){
                        PushCommand(*jengine.context, "add_current_workspace");
                    }
                    if(ImGui::MenuItem("Edit Workspace")){
                        PushCommand(*jengine.context, "edit_workspace");
                    }
                    if(jengine.config->j_workspace.size() > 0){
                        ImGui::Separator();
                        for(auto& wrokspace : jengine.config->j_workspace){
                            if(ImGui::MenuItem("Open Project##MainMenuBar_View")){
                                PushCommand(*jengine.context, "apply_workspace " + wrokspace);
                            }
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                if(ImGui::BeginMenu("General##MainMenuBar_View")){
                    if(ImGui::MenuItem("Application##MainMenuBar_View_General", NULL, vm.blueprint->IsEnable())){
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    if(ImGui::MenuItem("Blueprint##MainMenuBar_View_General", NULL, vm.blueprint->IsEnable())){
                        vm.blueprint->SetEnable(!vm.blueprint->IsEnable());
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    if(ImGui::MenuItem("Console##MainMenuBar_View_General", NULL, vm.console->IsEnable())){
                        vm.console->SetEnable(!vm.console->IsEnable());
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    if(ImGui::MenuItem("Explorer##MainMenuBar_View_General", NULL, vm.explorer->IsEnable())){
                        vm.explorer->SetEnable(!vm.explorer->IsEnable());
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    if(ImGui::MenuItem("Inspector##MainMenuBar_View_General", NULL, vm.inspector->IsEnable())){
                        vm.inspector->SetEnable(!vm.inspector->IsEnable());
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Audio##MainMenuBar_View")){
                    if(ImGui::MenuItem("Mixer##MainMenuBar_View_Audio", NULL, vm.inspector->IsEnable())){
                        //vm.inspector->SetEnable(!vm.inspector->IsEnable());
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    if(ImGui::MenuItem("Volume##MainMenuBar_View_Audio", NULL, vm.inspector->IsEnable())){
                        //vm.inspector->SetEnable(!vm.inspector->IsEnable());
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Render##MainMenuBar_View")){
                    if(ImGui::MenuItem("Preview##MainMenuBar_View_Render", NULL, vm.inspector->IsEnable())){
                        //vm.inspector->SetEnable(!vm.inspector->IsEnable());
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    if(ImGui::MenuItem("Material##MainMenuBar_View_Render", NULL, vm.inspector->IsEnable())){
                        //vm.inspector->SetEnable(!vm.inspector->IsEnable());
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    if(ImGui::MenuItem("Texture##MainMenuBar_View_Render", NULL, vm.inspector->IsEnable())){
                        //vm.inspector->SetEnable(!vm.inspector->IsEnable());
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Script##MainMenuBar_View")){
                    if(ImGui::MenuItem("Script##MainMenuBar_View_Script", NULL, vm.inspector->IsEnable())){
                        //vm.inspector->SetEnable(!vm.inspector->IsEnable());
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    if(ImGui::MenuItem("Plugin Manager##MainMenuBar_View_Script", NULL, vm.inspector->IsEnable())){
                        //vm.inspector->SetEnable(!vm.inspector->IsEnable());
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Debug##MainMenuBar_View")){
                    if(ImGui::MenuItem("Profiler##MainMenuBar_View_Debug", NULL, vm.inspector->IsEnable())){
                        //vm.inspector->SetEnable(!vm.inspector->IsEnable());
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    ImGui::EndMenu();
                }
                if(ImGui::BeginMenu("Animation##MainMenuBar_View")){
                    if(ImGui::MenuItem("Timeline##MainMenuBar_View_Animation", NULL, vm.inspector->IsEnable())){
                        //vm.inspector->SetEnable(!vm.inspector->IsEnable());
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    if(ImGui::MenuItem("Animator##MainMenuBar_View_Animation", NULL, vm.inspector->IsEnable())){
                        //vm.inspector->SetEnable(!vm.inspector->IsEnable());
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    if(ImGui::MenuItem("Animation##MainMenuBar_View_Animation", NULL, vm.inspector->IsEnable())){
                        //vm.inspector->SetEnable(!vm.inspector->IsEnable());
                        PushCommand(*jengine.context, "config_dirty");
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        for(auto& view : vm.views){
            if(view->IsEnable()){
                view->PreDraw();
                view->Draw();
                view->PostDraw();
            }
        }
        if(vm.popup_orders.size() > 0) VRenderPopup(vm, vm.popup_orders, 0);
    }

    void VUpdate(ViewManager& vm){
        for(auto& view : vm.views){
            if(view->IsEnable()){
                view->Update();
            }
        }
        for(auto& view : vm.popups){
            if(view->IsEnable()){
                view->Update();
            }
        }
    }

    void VRenderPopup(ViewManager& vm, std::vector<int32_t>& arrays, int32_t index){
        if(index >= arrays.size()) return;
        int32_t n = arrays.at(index);
        auto& view = vm.popups.at(n);
        view->PopupEvent(JPopupBase::SIDE::PRE);
        if(view->PreDraw()){
            view->Draw();
            view->PopupEvent(JPopupBase::SIDE::INSIDE);
            VRenderPopup(vm, arrays, ++index);
            view->PostDraw();
        }
        view->PopupEvent(JPopupBase::SIDE::POST);
    }
}
