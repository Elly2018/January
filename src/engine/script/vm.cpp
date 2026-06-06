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
#include "vm.h"
#include <spdlog/spdlog.h>
#include "../engine.h"
#include "../struct/context.h"
#include "../utility/logger.h"
#include "../utility/command.h"
#include <scriptbuilder/scriptbuilder.h>
#include <scriptstdstring/scriptstdstring.h>

January::Engine::AngelVM* avm = nullptr;

namespace January::Engine {
    namespace VM {
        void LogInfo(const std::string& msg){
            if(avm == nullptr) return;
            avm->LogInfo(msg);
        }

        void LogWarning(const std::string& msg){
            if(avm == nullptr) return;
            avm->LogWarning(msg);
        }

        void LogError(const std::string& msg){
            if(avm == nullptr) return;
            avm->LogError(msg);
        }
    }
    AngelVM::AngelVM(System::JWindow& _win, JEngine& _engine) : jwindow(_win), jengine(_engine){
        engine = std::unique_ptr<asIScriptEngine, ASEngineDeleter>(asCreateScriptEngine(ANGELSCRIPT_VERSION));
        engine->SetMessageCallback(asMETHOD(AngelVM, ScriptMessageCallback), this, asCALL_THISCALL);
        RegisterStdString(engine.get());
        avm = this;
    }

    AngelVM::~AngelVM(){
        engine.release();
        avm = nullptr;
    }

    bool AngelVM::IsCompiling() {
        return compiling;
    }

    void AngelVM::RunEditorScript(std::string path) {
        if(!fs::exists(path)){
            spdlog::error("Module not found in the path: {}", path);
            return;
        }
        std::string rela = fs::relative(path, jengine.context->project_path).string();
        asIScriptModule* mod = engine->GetModule(rela.c_str(), asGM_ONLY_IF_EXISTS);
        if(mod == nullptr){
            spdlog::info("Module not found. Compiling fresh: {}", path);
            PrepareGlobal();
            CompileSingle(path);
            mod = engine->GetModule(rela.c_str(), asGM_ONLY_IF_EXISTS);
        }

        if(mod == nullptr) {
            spdlog::error("Error happen during compile stage", path);
            return;
        }

        asIScriptFunction* func = mod->GetFunctionByDecl("void main()");
        if (func == nullptr) {
            spdlog::warn("Function 'void main()' not found in script: {}", path);
            return;
        }

        asIScriptContext* ctx = engine->CreateContext();
        if (ctx == nullptr) {
            spdlog::error("Failed to allocate an execution context thread.");
            return;
        }

        ctx->Prepare(func);
        int32_t executionResult = ctx->Execute();

        if (executionResult != asEXECUTION_FINISHED) {
            if (executionResult == asEXECUTION_EXCEPTION) {
                spdlog::error("Script runtime exception in [{}]: line {}: {}", 
                            path,
                            ctx->GetExceptionLineNumber(), 
                            ctx->GetExceptionString());
            } else {
                spdlog::error("Script execution failed with status internal code: {}", executionResult);
            }
        } else {
            spdlog::info("Script [{}] executed flawlessly.", path);
        }

        ctx->Release();
    }

    void AngelVM::UpdateVMContent() {
        if(compiling){
            spdlog::error("It is compiling currently, Please wait...");
            return;
        }
        fs::path p_path = "";
        {
            JLOCK(jengine.context->project_path, 1)
            p_path = jengine.context->project_path;
        }
        if(!fs::exists(p_path)){
            spdlog::error("Cannot update vm, Project is not load: {}", p_path.string());
            return;
        }
        p_path /= "Assets";
        if(!fs::exists(p_path)){
            spdlog::error("Cannot update vm, Asset folder does not exist: {}", p_path.string());
            return;
        }
        std::thread([&](){
            compiling = true;
            Compile();
            compiling = false;
        }).detach();
    }

    void AngelVM::PrepareGlobal() {
        int32_t r = engine->RegisterGlobalFunction("void LogInfo(const string &in msg)", asFUNCTION(VM::LogInfo), asCALL_CDECL);
        if (r < 0) spdlog::error("Failed to register global function 'LogInfo'.");
        r = engine->RegisterGlobalFunction("void LogWarning(const string &in msg)", asFUNCTION(VM::LogWarning), asCALL_CDECL);
        if (r < 0) spdlog::error("Failed to register global function 'LogWarning'.");
        r = engine->RegisterGlobalFunction("void LogError(const string &in msg)", asFUNCTION(VM::LogError), asCALL_CDECL);
        if (r < 0) spdlog::error("Failed to register global function 'LogError'.");
    }

    void AngelVM::Compile() {
        {
            JLOCK(modules, 1)
            for(auto& m : modules){
                asIScriptModule* oldMod = engine->GetModule(m.c_str(), asGM_ONLY_IF_EXISTS);
                if (oldMod) {
                    oldMod->Discard(); 
                    spdlog::debug("Stale script module discarded successfully.");
                }
            }
            modules.clear();
        }
        std::vector<fs::path> files = GetAllScriptPath();
        PrepareGlobal();
        for(auto& file : files){
            CompileSingle(file);
        }
    }

    void AngelVM::CompileSingle(fs::path file) {
        CScriptBuilder builder;
        std::string file_path = file.string();
        std::string rela = fs::relative(file, jengine.context->project_path).string();
        int32_t r = builder.StartNewModule(engine.get(), rela.c_str());
        if (r < 0) {
            spdlog::error("Failed to allocate fresh script module shell. {}", file_path);
            return;
        }

        r = builder.AddSectionFromFile(file_path.c_str());
        if (r < 0) {
            spdlog::error("Failed to read script entry file: {}", file_path);
            return;
        }

        r = builder.BuildModule();
        if (r < 0) {
            spdlog::error("Hot-reload compilation failed! Keeping engine state empty. {}", file_path);
            return;
        }

        spdlog::info("Script environment re-allocated and compiled successfully. {}", file_path);

        {
            JLOCK(modules, 1)
            modules.push_back(rela);
        }
    }

    std::vector<fs::path> AngelVM::GetAllScriptPath(){
        std::vector<fs::path> r;
        fs::path root = jengine.context->project_path;
        root /= "Assets";
        if (!fs::exists(root) || !fs::is_directory(root)) {
            return r;
        }
        auto iterOptions = fs::directory_options::skip_permission_denied;
        for (const auto& entry : fs::recursive_directory_iterator(root, iterOptions)) {
            if (entry.is_regular_file() && entry.path().extension() == ".as") {
                r.push_back(entry.path());
            }
        }
        return r;
    }

    void AngelVM::ScriptMessageCallback(const asSMessageInfo* msg) {
        // Format the error trace location accurately
        std::string location = fmt::format("{}:({},{})", msg->section, msg->row, msg->col);

        // Route message types cleanly to your engine's logging system
        if (msg->type == asMSGTYPE_ERROR) {
            spdlog::error("[AngelScript Compiling Exception] {} : {}", location, msg->message);
        } 
        else if (msg->type == asMSGTYPE_WARNING) {
            spdlog::warn("[AngelScript Compilation Warn] {} : {}", location, msg->message);
        } 
        else {
            spdlog::info("[AngelScript Compiler Trace] {} : {}", location, msg->message);
        }
    }

    void AngelVM::LogInfo(const std::string& msg){
        jengine.context->logger->script_logger->logger->info(msg);
    }

    void AngelVM::LogWarning(const std::string& msg){
        jengine.context->logger->script_logger->logger->warn(msg);
    }

    void AngelVM::LogError(const std::string& msg){
        jengine.context->logger->script_logger->logger->error(msg);
    }
}