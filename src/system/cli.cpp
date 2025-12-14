#include "cli.h"
#include <assert.h>
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

static argh::parser cmd;

void configure_parser() {
    fs::path currentPath = fs::current_path();
    cmd.add_params({ "-p", "--path" });
    if(cmd[{ "-h", "--help" }]){
        std::printf("Show all the command.\n");

        std::printf("\t - Params: \n");
        std::printf("\t\t -p, --path \t - Define where the project path location in your machine. \n");
        std::printf("\t - Flags: \n");
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