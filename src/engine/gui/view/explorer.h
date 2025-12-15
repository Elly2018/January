#pragma once
#include "viewbase.h"

class JViewExplorer : public JViewBase {
public:
    JViewExplorer(const char* _title, int32_t _type);
    virtual ~JViewExplorer();

    void Init() override;
    void Update() override;
    void Draw() override;
    void DeInit() override;

public:
    char path[512];

private:
    bool changed = false;
};