#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <uuid_v4.h>
#include <FileWatch.hpp>
#include <mutex>
#include "viewbase.h"

namespace fs = std::filesystem;

struct JFileContent {
    UUIDv4::UUID uuid;
    std::string title;
    fs::path path;
    bool is_dir;
    uintmax_t filesize;
};

class JViewExplorer : public JViewBase {
public:
    JViewExplorer(const char* _title, int32_t _type);
    virtual ~JViewExplorer();

    void Init() override;
    void Update() override;
    void Draw() override;
    void DeInit() override;

protected:
    void DrawLeftSide();
    void DrawRightSide();

public:
    std::string path = "";

private:
    bool changed = false;
    bool init = false;
    float leftWidth = 0;
    float rightWidth = 0;
    float imgSize = 0.5f;
    int32_t selection = 0;
    filewatch::FileWatch<std::string>* watcher = nullptr;
    std::mutex mtx;
    std::vector<JFileContent> files = std::vector<JFileContent>();
};