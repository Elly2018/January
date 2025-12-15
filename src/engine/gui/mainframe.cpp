#include "mainframe.h"
#include <imgui.h>

void status_bar(){
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
            if(ImGui::MenuItem("RESOURCE")){
                
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
            if(ImGui::MenuItem("Explorer")){
                
            }
            if(ImGui::MenuItem("Preview")){
                
            }
            if(ImGui::MenuItem("Timeline")){
                
            }
            if(ImGui::MenuItem("Blueprint")){
                
            }
            if(ImGui::MenuItem("Script")){
                
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Help")){
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void UIDraw(JWindow& win, JEngine& engine){
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    status_bar();

    int32_t padding_top = 50;
    ImVec2 wsize = io.DisplaySize;
    wsize.y - padding_top;
    ImGuiWindowFlags frame_flag = 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoNav | 
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoScrollbar | 
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowPos(ImVec2(0, padding_top));
    ImGui::SetNextWindowSize(wsize);
    if(ImGui::Begin("MainFrame", nullptr, frame_flag)){
        ImGui::End();
    }
}