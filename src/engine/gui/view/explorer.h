#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include "viewbase.h"

namespace fs = std::filesystem;

struct JFileContent {
    std::string title;
    fs::path path;
    int64_t filesize;
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
    char path[512] = "";

private:
    bool changed = false;
    bool init = false;
    float leftWidth = 0;
    float rightWidth = 0;
    std::vector<JFileContent> files = std::vector<JFileContent>();
};