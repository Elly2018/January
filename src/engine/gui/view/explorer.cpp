#include "explorer.h"
#include <filesystem>
#include <thread>
#include <spdlog/spdlog.h>
#include <imgui.h>
#include "../../engine.h"

namespace fs = std::filesystem;

// Global engine access point
extern JEngine jengine;

JViewExplorer::JViewExplorer(const char* _title, int32_t _type) : JViewBase(_title, _type) {

}

JViewExplorer::~JViewExplorer(){
    
}

void JViewExplorer::Init() {
    spdlog::info("Loaded View: Explorer");
}

void JViewExplorer::Update() {
    if(changed){
        spdlog::debug("Detect explorer update");
        fs::path pp(jengine.context.get()->project_path.c_str());
        if(fs::exists(pp)){
            if(path != "/" || path != "") pp /= path.c_str();
            if(!fs::exists(pp)){
                spdlog::warn("\tPath does not exist: {}, So we change back to project root instead", pp.c_str());
                pp = fs::path(jengine.context.get()->project_path.c_str());
                path = "/";
            }
            if(watcher != nullptr){
                delete watcher;
                spdlog::debug("\tRemove explorer file watcher");
            }
            spdlog::debug("\tTry register watcher: {}", pp.c_str());
            watcher = new filewatch::FileWatch<std::string>(
                pp.string(),
                [this](const std::string& path, const filewatch::Event change_type){
                    changed = true;
                }
            );
            spdlog::debug("\tCreate explorer file watcher");

            files.clear();
            spdlog::debug("\tClean file data");
            UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
            {
                std::lock_guard<std::mutex> guard(mtx);
                spdlog::debug("\tStart fetch files...");
                for(auto entry : fs::directory_iterator(pp)){
                    spdlog::debug("\t\tDetect entry: {}", entry.path().c_str());
                    JFileContent file = JFileContent();
                    file.uuid = uuidGenerator.getUUID();
                    file.is_dir = entry.is_directory();
                    file.path = entry.path();
                    file.title = entry.path().filename();
                    if(!entry.is_directory()){
                        file.filesize = entry.file_size();
                    }
                    files.push_back(file);
                }
            }
        }else{
            spdlog::debug("\tSkip update because project path not exist");
        }
        changed = false;
    }
}

void JViewExplorer::Draw() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::Begin(title);
        if(!init){
            changed = true;
            init = true;
        }

        float w = std::max(ImGui::GetWindowWidth(), 20.f);
        leftWidth = w * (1.f / 3.f);
        rightWidth = w * (2.f / 3.f);

        AppContext* context = jengine.context.get();
        if(context == nullptr){
            spdlog::error("context is nullptr");
        }

        if(context != nullptr && fs::exists(context->project_path)){
            bool change = ImGui::InputText("Path", path.data(), 512, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue);
            if(change) changed = true;
            ImGui::SliderFloat("size", &imgSize, 0.f, 1.f, "%.1f");
            ImGui::BeginChild("ViewExplorer_Left", ImVec2(leftWidth - (style.DisplayWindowPadding.x / 1.5f), 0), true);
                DrawLeftSide();
            ImGui::EndChild();
            ImGui::SameLine();
            ImGui::BeginChild("ViewExplorer_Right", ImVec2(rightWidth - (style.DisplayWindowPadding.x / 1.5f), 0), true);
                DrawRightSide();
            ImGui::EndChild();
        }else{
            ImGui::Text("You need to choose path fist");
        }
    ImGui::End();
}

void JViewExplorer::DeInit() {

}

void JViewExplorer::DrawLeftSide(){
    ImGui::Text("Left");
}

void JViewExplorer::DrawRightSide(){
    if(imgSize == 0){
        ImGui::BeginListBox("Explorer_Right_Listbox", ImVec2(rightWidth, 0));
        int32_t c = 0;
        for(auto file : files){
            ImGui::Selectable(file.title.c_str(), c == selection);
            c++;
        }
        ImGui::EndListBox();
    }else{
        ImGui::BeginGroup();
        int32_t c = 0;
        int32_t row = std::floor<int32_t>(rightWidth / std::max<float>(imgSize * 500, 50));
        for(auto file : files){
            ImGui::Button(file.title.c_str(), ImVec2(std::max<float>(imgSize * 500, 50), std::max<float>(imgSize * 500, 50)));
            if((c + 1) % row != 0){
                ImGui::SameLine();
            }
            c++;
        }
        ImGui::EndGroup();
    }
}