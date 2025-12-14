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
        if(ImGui::BeginMenu("View")){
            if(ImGui::MenuItem("World")){
                
            }
            if(ImGui::MenuItem("Cut")){
                
            }
            if(ImGui::MenuItem("Cut")){
                
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
    status_bar();
}