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
    struct JComponent {
        virtual json Serialization() { return json::object(); }
        virtual void DeSerialization(json data) {}
    };

    class JSystemBase {
    public:
        JSystemBase(std::string _type_name) {
            type_name = _type_name;
        }
        virtual ~JSystemBase() {}

        virtual void RemoveComponent(std::string entity);
        const std::string GetTypeName() { return type_name; }
    private:
        std::string type_name;
    };

    template<typename T> // Component type
    class JSystem : public JSystemBase {
    public:
        JSystem() : JSystemBase(typeid(T).name()){
            static_assert(std::is_base_of_v<JComponent, T>, "T must derive from JComponent");
        }
        virtual ~JSystem(){}
        virtual void Update() {}
        void AddComponent(std::string entity, T instance);
        void ReplaceComponent(std::string entity, T instance);
        bool GetComponent(std::string entity, T& data);
        void RemoveComponent(std::string entity) override;

    protected:
        std::unordered_map<std::string, T> components = std::unordered_map<std::string, T>();
    };

    class JECS {
    public:
        template<typename T> // Component type
        void RegisterSystem(JSystem<T> system);

        std::string CreateEntity();
        void DeleteEntity(const std::string id);

        template<typename T> // Component type
        bool RegisterComponent(std::string id, T comp);

        template<typename T> // Component type
        bool UnRegisterComponent(std::string id);

    private:
        std::vector<std::string> entites = std::vector<std::string>();
        std::vector<JSystemBase> systems = std::vector<JSystemBase>();
    };
}

#endif