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
#pragma once
#ifndef ENGINE_NODE_ECS_H
#define ENGINE_NODE_ECS_H
#include <cinttypes>
#include <type_traits>
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace January::Engine::Node {

    class JECS;

    struct JComponent {
        virtual json Serialization() { return json::object(); }
        virtual void DeSerialization(json data) {}
    };

    class JComponentDBBase {
    public:
        JComponentDBBase(std::string _type_name) {
            type_name = _type_name;
        }
        virtual ~JComponentDBBase() {}

        virtual void RemoveComponent(std::string entity) {}
        const std::string GetTypeName() { return type_name; }
    private:
        std::string type_name;
    };

    template<typename T> // Component type
    class JComponentDB : public JComponentDBBase {
    public:
        JComponentDB() : JComponentDBBase(typeid(T).name()){
            static_assert(std::is_base_of_v<JComponent, T>, "T must derive from JComponent");
        }
        virtual ~JComponentDB(){}
        virtual void Update() {}
        void AddComponent(std::string entity, T instance);
        void ReplaceComponent(std::string entity, T instance);
        bool GetComponent(std::string entity, T& data);
        void RemoveComponent(std::string entity) override;

    protected:
        std::unordered_map<std::string, T> components = std::unordered_map<std::string, T>();
    };

    class JSystem {
    public:
        JSystem(int32_t _weight, JECS* _ecs) {
            weight = _weight;
            ecs = _ecs;
        }
        virtual ~JSystem() {}

        virtual void Update(std::vector<std::string> ids) {}
        
        int32_t weight;
    protected:
        template<typename T>
        JComponentDB<T>& GetComponentDB();

    protected:
        JECS* ecs;
    };

    class JECS {
    public:
        template<typename T> // Component type
        void RegisterDB(JComponentDB<T> db);

        std::string CreateEntity();
        void DeleteEntity(const std::string id);

        template<typename T> // Component type
        bool RegisterComponent(std::string id, T comp);

        template<typename T> // Component type
        bool UnRegisterComponent(std::string id);

        void RegisterSystem(JSystem system);

        void ExecuteAll();

    private:
        // All exist entites
        std::vector<std::string> entites = std::vector<std::string>();
        // All component db
        std::vector<JComponentDBBase> dbs = std::vector<JComponentDBBase>();
        // All system
        std::vector<JSystem> systems = std::vector<JSystem>();
    };
}

#endif