#pragma once
#include <string>

class JViewBase {
public:
    JViewBase(const char* _title, int32_t _type) : title(_title), type(_type) {
        
    }
    virtual ~JViewBase() {}
    virtual void Init() {}
    virtual void Update() {}
    virtual void Draw() {}
    virtual void DeInit() {}

public:
    const char* title;
    int32_t type;
};