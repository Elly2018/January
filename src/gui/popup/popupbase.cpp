#include "popupbase.h"
#include <string>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "../../engine/engine.h"
#include "../../engine/struct/context.h"
#include "../../engine/utility/command.h"

namespace January::Engine::View {
    JPopupBase::JPopupBase(std::string _title, int32_t _type, int32_t _subtype, System::JWindow& _win, JEngine& _engine) : 
        JViewBase(_title, _type, subtype, _win, _engine) {
        }
    JPopupBase::~JPopupBase() {
    }

    void JPopupBase::OnEnable() {
        popup_flag = 1;
        startup = true;
    }

    void JPopupBase::OnDisable() {
        popup_flag = 2;
    }

    bool JPopupBase::PreDraw(){
        ImVec2 screenSize = GetScreenSize();
        ImVec2 actualSize = popup_size;
        if(ratio){
            actualSize = ImVec2(screenSize.x * popup_size.x, screenSize.y * popup_size.y);
        }
        if(actualSize != ImVec2(0, 0)) ImGui::SetNextWindowSize(actualSize);
        
        if(!free_float || startup){
            ImGui::SetNextWindowPos( (screenSize - actualSize)/2.0f );
            startup = false;
        }
        return ImGui::BeginPopupModal(title.c_str(), NULL, window_flag);
    }

    void JPopupBase::PostDraw(){
        ImGui::EndPopup();
    }

    void JPopupBase::PopupEvent(SIDE side){
        if(popup_flag == 1 && side == SIDE::POST){
            ImGui::OpenPopup(title.c_str());
            popup_flag = 0;
        }else if(popup_flag == 2 && side == SIDE::INSIDE){
            ImGui::CloseCurrentPopup();
            popup_flag = 0;
        }
    }

    ImVec2 JPopupBase::GetScreenSize(){
        return ImGui::GetMainViewport()->Size;
    }

    void JPopupBase::SetPopupSize(ImVec2 size, bool useRatio, bool use_free_float){
        popup_size = size;
        ratio = useRatio;
        free_float = use_free_float;
    }
}