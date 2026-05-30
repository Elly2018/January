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

    bool par_v = January::CLI::GetCMDFlag("v", "verbose");
    bool par_vv = January::CLI::GetCMDFlag("vv", "very-verbose");
    if(par_v){
        spdlog::set_level(spdlog::level::debug);
    }
    else if(par_vv){
        spdlog::set_level(spdlog::level::trace);
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
        std::printf("\t\t -vv, --very-verbose \t - Print all the trace detail out. \n");
        exit(0);
    }
}

void January::CLI::InitCMD(int argc, char** argv){
    cmd = argh::parser(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    configure_parser();
}

argh::parser& January::CLI::GetCMD(){
    return cmd;
}

bool January::CLI::GetCMDFlag(std::string title){
    for(auto flag : cmd.flags()){
        if(flag == title) return true;
    }
    return false;
}

bool January::CLI::GetCMDFlag(std::string title, std::string second){
    for(auto flag : cmd.flags()){
        if(flag == title || flag == second) return true;
    }
    return false;
}

std::optional<std::string> January::CLI::GetCMDParam(std::string title){
    for(auto param : cmd.params()){
        if(param.first == title) return param.second;
    }
    return std::nullopt;
}

std::optional<std::string> January::CLI::GetCMDParam(std::string title, std::string second){
    for(auto param : cmd.params()){
        if(param.first == title || param.first == second) return param.second;
    }
    return std::nullopt;
}