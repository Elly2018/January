#include "cli.h"
#include <assert.h>
#include <filesystem>
#include <memory>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

static argh::parser cmd;

void configure_parser() {
    cmd.add_params({"-p", "--path"});
    cmd.add_params({"-v", "--verbose"});

    bool par_v = GetCMDFlag("v", "verbose");
    if(par_v){
        spdlog::set_level(spdlog::level::debug);
    }

    spdlog::debug("Verbose enable");
    spdlog::debug("\tParams:");
    for(auto param : cmd.params()){
        spdlog::debug("\t\tParam {} : {}", param.first.c_str(), param.second.c_str());
    }
    spdlog::debug("\tFlags:");
    for(auto flag : cmd.flags()){
        spdlog::debug("\t\tFlag {}", flag.c_str());
    }

    if(cmd[{ "-h", "--help" }]){
        std::printf("Show all the command.\n");

        std::printf("\t - Params: \n");
        std::printf("\t\t -p, --path \t - Define where the project path location in your machine. \n");
        std::printf("\t - Flags: \n");
        std::printf("\t\t -v, --verbose \t - Print all the debug detail out. \n");
        exit(0);
    }
}

void InitCMD(int argc, char** argv){
    cmd = argh::parser(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    configure_parser();
}

argh::parser& GetCMD(){
    return cmd;
}

bool GetCMDFlag(std::string title){
    for(auto flag : cmd.flags()){
        if(flag == title) return true;
    }
    return false;
}

bool GetCMDFlag(std::string title, std::string second){
    for(auto flag : cmd.flags()){
        if(flag == title || flag == second) return true;
    }
    return false;
}

std::optional<std::string> GetCMDParam(std::string title){
    for(auto param : cmd.params()){
        if(param.first == title) return param.second;
    }
    return std::nullopt;
}

std::optional<std::string> GetCMDParam(std::string title, std::string second){
    for(auto param : cmd.params()){
        if(param.first == title || param.first == second) return param.second;
    }
    return std::nullopt;
}