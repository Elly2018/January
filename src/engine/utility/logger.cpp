#include "logger.h"

namespace January::Engine {
    JLoggerWorker::JLoggerWorker(const char* name, bool global, enum spdlog::level::level_enum _level){
        isglobal = global;
        level = _level;
        callback_sink = std::make_shared<spdlog::sinks::callback_sink_mt>([&](const spdlog::details::log_msg &msg) {
            // for example you can be notified by sending an email to yourself
            JConsoleLog cl = JConsoleLog();
            cl.level = msg.level;
            cl.messages = std::string(msg.payload.data(), msg.payload.size());;
            cl.id = id_counter;
            changed = true;
            id_counter++;
            std::lock_guard<std::mutex> lock(log_mtx);
            logs.push_back(cl);
        });
        callback_sink->set_level(spdlog::level::trace);
        if(global){
            std::vector<spdlog::sink_ptr>& sinks = spdlog::default_logger()->sinks();
            sinks.push_back(callback_sink);
        }
        logger = new spdlog::logger(name, callback_sink);
    }

    JLoggerWorker::~JLoggerWorker(){
        {
            std::lock_guard<std::mutex> lock(log_mtx);
            logs.clear();
        }
        if(isglobal){
            std::vector<spdlog::sink_ptr>& sinks = spdlog::default_logger()->sinks();
            auto it = std::find(sinks.begin(), sinks.end(), callback_sink);
            if(it != sinks.end()){
                sinks.erase(it);
            }
        }
        delete logger;
    }

    bool JLoggerWorker::IsChanged(bool flip){
        bool s = changed;
        if(flip) changed = false;
        return s;
    }

    bool JLoggerWorker::IsGlobal(){
        return isglobal;
    }

    void JLoggerWorker::Clear(){
        id_counter = 0;
        logs.clear();
    }

    JLogger::JLogger(){
        logger = new JLoggerWorker("Engine", true, spdlog::level::trace);
        runtime_logger = new JLoggerWorker("Runtime", false, spdlog::level::trace);
        script_logger = new JLoggerWorker("Script", false, spdlog::level::trace);
    }

    JLogger::~JLogger(){
        delete logger;
        delete runtime_logger;
        delete script_logger;
    }
}