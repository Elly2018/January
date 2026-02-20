#include "popupbase.h"
#include <string>
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
        ImGui::OpenPopup(title.c_str());
    }

    void JPopupBase::OnDisable() {
        ImGui::CloseCurrentPopup();
    }

    bool JPopupBase::PreDraw(){
        return ImGui::BeginPopupModal(title.c_str());
    }

    void JPopupBase::PostDraw(){
        ImGui::EndPopup();
    }
}