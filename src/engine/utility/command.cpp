#include "command.h"
#include <vector>
#include <sstream>
#include <mutex>
#include "spdlog/spdlog.h"
#include "../../system/system.h"
#include "../../system/window.h"
#include "../../gui/manager.h"
#include "../engine.h"
#include "../struct/config.h"
#include "../struct/context.h"

namespace January::Engine {
    std::vector<std::string> split_string_by_space(const std::string& str) {
        std::vector<std::string> words;
        std::stringstream ss(str); // Turn the string into a stream
        std::string word;

        // The extraction operator (>>) automatically uses whitespace as a delimiter
        while (ss >> word) {
            words.push_back(word);
        }

        return words;
    }

    void multi_command(struct System::JSystem& jsystem, std::vector<std::string> cmds){
        
    }

    void single_command(struct System::JSystem& jsystem, std::string cmd){
        if(cmd == "config_dirty"){
            SaveEnableConfig(*jsystem.engine->manager, *jsystem.engine->config);
            SaveAppConfig(*jsystem.engine->config);
            System::SavePreference();
        }
    }

    void ApplyCommand(struct System::JSystem& jsystem, std::string cmd){
        std::vector<std::string> cs = split_string_by_space(cmd);
        if(cs.size() == 1){
            single_command(jsystem, cmd);
        }else{
            multi_command(jsystem, cs);
        }
        spdlog::debug("Apply command: {}", cmd);
    }

    void PushCommand(struct AppContext& ctx, std::string cmd){
        std::lock_guard<std::mutex> lock(ctx.commands_mtx);
        ctx.commands.push(cmd);
        spdlog::debug("Push command: {}", cmd);
    }
}
