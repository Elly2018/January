#include "ecs.h"
#include "uuid_v4.h"

namespace January::Engine::Node {
    UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;

    template<typename T>
    void JSystem<T>::AddComponent(std::string entity, T instance){
        if(components.count(entity)) return;
        components.insert(std::pair<std::string, T>(entity, instance));
    }

    template<typename T>
    void JSystem<T>::ReplaceComponent(std::string entity, T instance){
        components.insert_or_assign(std::pair<std::string, T>(entity, instance));
    }

    template<typename T>
    bool JSystem<T>::GetComponent(std::string entity, T& data){
        if(!components.count(entity)) return false;
        data = components.at(entity);
        return true;
    }

    template<typename T>
    void JSystem<T>::RemoveComponent(std::string entity){

    }

    template<typename T> // Component type
    void JECS::RegisterSystem(JSystem<T> system){
        auto it = std::find(systems.begin(), systems.end(), system);
        if(it == systems.end()) return;
        systems.push_back(system);
    }

    std::string JECS::CreateEntity() {
        char txt[37];
        UUIDv4::UUID id = uuidGenerator.getUUID();
        id.str(txt);
        std::string result = std::string(txt);
        while(std::find(entites.begin(), entites.end(), result) != entites.end()){
            UUIDv4::UUID id = uuidGenerator.getUUID();
            id.str(txt);
            result = std::string(txt);
        }
        entites.push_back(result);
        return result;
    }

    void JECS::DeleteEntity(const std::string id){
        auto it = std::find(entites.begin(), entites.end(), id);
        if(it == entites.end()) return;
        for(auto& sy : systems){
            sy.RemoveComponent(id);
        }
        entites.erase(it);
    }

    template<typename T> // Component type
    bool JECS::RegisterComponent(std::string id, T comp){

    }

    template<typename T> // Component type
    bool JECS::UnRegisterComponent(std::string id){

    }
}