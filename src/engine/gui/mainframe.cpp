#include "mainframe.h"
#include <imgui.h>
#include "../utility/path.h"
#include "view/explorer.h"

void status_bar(const JWindow& win, JEngine& engine){
    if(ImGui::BeginMainMenuBar()){
        if(ImGui::BeginMenu("File")){
            if(ImGui::MenuItem("New Project")){

            }
            if(ImGui::MenuItem("Open Project")){
                
            }
            ImGui::Separator();
            if(ImGui::MenuItem("Save")){
                
            }
            if(ImGui::MenuItem("Save As")){
                
            }
            ImGui::Separator();
            if(ImGui::BeginMenu("Preference")){
                if(ImGui::MenuItem("Setting")){
                
                }
                if(ImGui::MenuItem("Extension")){
                    
                }
                if(ImGui::MenuItem("Shortcut")){
                    
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if(ImGui::MenuItem("Exit")){
                
            }

            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Edit")){
            if(ImGui::MenuItem("Undo")){
                
            }
            if(ImGui::MenuItem("Redo")){
                
            }
            ImGui::Separator();
            if(ImGui::MenuItem("Cut")){
                
            }
            if(ImGui::MenuItem("Copy")){
                
            }
            if(ImGui::MenuItem("Paste")){
                
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Page")){
            if(ImGui::MenuItem("Resource")){
                
            }
            if(ImGui::MenuItem("Compositing")){
                
            }
            if(ImGui::MenuItem("Scene")){
                
            }
            if(ImGui::MenuItem("Material")){
                
            }
            if(ImGui::MenuItem("Audio")){
                
            }
            if(ImGui::MenuItem("Logic")){
                
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("View")){
            if(ImGui::BeginMenu("Audio")){
                if(ImGui::MenuItem("Volumn")){
                
                }
                if(ImGui::MenuItem("Audio Mixer")){
                
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("3D")){
                if(ImGui::MenuItem("Material")){
                
                }
                if(ImGui::MenuItem("Hierarchy")){
                
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Logic")){
                if(ImGui::MenuItem("Blueprint")){
                
                }
                if(ImGui::MenuItem("Script")){
                    
                }
                if(ImGui::MenuItem("Console")){
                    
                }
                if(ImGui::MenuItem("Debugger")){
                    
                }
                ImGui::EndMenu();
            }
            if(ImGui::MenuItem("Explorer")){
                
            }
            if(ImGui::MenuItem("Preview")){
                
            }
            if(ImGui::MenuItem("Timeline")){
                
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Help")){
            if(ImGui::MenuItem("Release Note")){
                
            }
            if(ImGui::MenuItem("Documentation")){
                
            }
            ImGui::Separator();
            if(ImGui::MenuItem("About Us")){
                
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void UIDraw(const JWindow& win, JEngine& engine){
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    status_bar(win, engine);

    AppContext* appcontext = engine.context.get();
    if(appcontext == nullptr) return;

    for(auto view : appcontext->views){
        JViewBase* vbase = view.get();
        if(vbase == nullptr) continue;
        vbase->Draw();
    }
}

void UIUpdate(JWindow& win, JEngine& engine){
    for(auto view : engine.context.get()->views){
        view.get()->Update();
    }
}

bool UIPageFirstTimeFire(JEngine& engine, JPageType page){
    if(page == JPageType::JPAGETYPE_CUSTOM) return false;
    fs::path home = get_home_directory();
    home = home.append("january");
    if(!fs::exists(home)){
        fs::create_directory(home);
    }
    home = home.append(GetPageTypeString(page) + ".json");
    return !fs::exists(home);
}

template<Derived<JViewBase> T>
bool UITryAdd(JEngine& engine, const char* title, JViewType type) {
    bool find = false;
    for(auto inst : engine.context.get()->views){
        if(inst.get()->type == type){
            find = true;
            break;
        }
    }
    if(!find){
        std::shared_ptr<T> exp = std::make_shared<T>(title, type);
        exp.get()->Init();
        engine.context.get()->views.push_back(exp);
        return true;
    }
    return false;
}

void UIGenerateViews(JEngine& engine, std::vector<JViewType> views){
    for(JViewType type : views){
        switch(type){
            case JViewType::JVIEWTYPE_EXPLORER:
                UITryAdd<JViewExplorer>(engine, "Explorer", type);
                break;
        }
    }
}

void UILoadPageFromDisk(JEngine& engine, std::string name){
    fs::path home = get_home_directory();
    home = home.append("january");
    if(!fs::exists(home)){
        fs::create_directory(home);
    }
    home = home.append(name + ".json");
    ImGui::LoadIniSettingsFromDisk(home.c_str());
}