#include "popupbase.h"
#include <string>
#include <imgui.h>
#include "../../engine/engine.h"
#include "../../engine/struct/context.h"
#include "../../engine/utility/command.h"

namespace January::Engine::View {
    JPopupBase::JPopupBase(std::string _title, int32_t _type, int32_t _subtype, System::JWindow& _win, JEngine& _engine, int32_t _popup_id) : JViewBase(_title, _type, subtype, _win, _engine) {
        popup_id = _popup_id;
    }
    JPopupBase::~JPopupBase() {
    }

    void JPopupBase::OnEnable() {
        popup_flag = 1;
        PushCommand(*jengine.context, "open_popup " + std::to_string(popup_id));
    }

    void JPopupBase::OnDisable() {
        popup_flag = 2;
    }

    bool JPopupBase::PreDraw(){
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
}