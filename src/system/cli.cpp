#include "cli.h"
#include <assert.h>
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

static std::shared_ptr<cli::Parser> cmd;

void configure_parser(cli::Parser& parser) {
    fs::path currentPath = fs::current_path();
	parser.set_optional<std::string>("o", "output", "data", "Strings are naturally included.");
	parser.set_optional<int>("n", "number", 8, "Integers in all forms, e.g., unsigned int, long long, ..., are possible. Hexadecimal and Ocatl numbers parsed as well");
	parser.set_optional<cli::NumericalBase<int, 10>>("t", "temp", 0, "integer parsing restricted only to numerical base 10");
	parser.set_optional<double>("b", "beta", 11.0, "Also floating point values are possible.");
	parser.set_optional<bool>("a", "all", false, "Boolean arguments are simply switched when encountered, i.e. false to true if provided.");
	parser.set_optional<std::string>("p", "path", fs::absolute(currentPath).string(), "Define where the project path location in your machine.").get();
    if(parser.has_help()){
        parser.run_and_exit_if_error();
    }
}

void InitCMD(int argc, char** argv){
    if(cmd.get() != nullptr){
        cmd.reset();
    }
    cmd = std::make_shared<cli::Parser>();
    cli::Parser* cmdref = cmd.get();
    assert(cmdref != nullptr);

    *cmdref = cli::Parser(argc, argv);
    configure_parser(*cmdref);
}

void DeInitCMD(){
    cmd.reset();
}

std::weak_ptr<cli::Parser> GetCMD(){
    return cmd;
}