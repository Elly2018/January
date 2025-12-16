#pragma once
#include "viewbase.h"

class JViewBlueprint : public JViewBase {
    JViewBlueprint(const char* _title, int32_t _type);
    virtual ~JViewBlueprint();

    void Init() override;
    void Update() override;
    void Draw() override;
    void DeInit() override;
};