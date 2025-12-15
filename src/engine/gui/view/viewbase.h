#pragma once
#include <string>

class JViewBase {
public:
    JViewBase(const char* _title) : title(_title) {
        
    }
    virtual ~JViewBase() {}
    virtual void Init() {}
    virtual void Update() {}
    virtual void Draw() {}
    virtual void DeInit() {}

private:
    const char* title;
};