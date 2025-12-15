#pragma once
#include "viewbase.h"

class JViewExplorer : public JViewBase {
public:
    void Init() override;
    void Update() override;
    void Draw() override;
    void DeInit() override;
};