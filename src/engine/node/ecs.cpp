#include "ecs.h"
#include "uuid_v4.h"

namespace January::Engine::Node {
    UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;

    template<typename T>
    void JComponentDB<T>::AddComponent(std::string entity, T instance){
        if(components.count(entity)) return;
        components.insert(std::pair<std::string, T>(entity, instance));
    }

    template<typename T>
    void JComponentDB<T>::ReplaceComponent(std::string entity, T instance){
        components.insert_or_assign(std::pair<std::string, T>(entity, instance));
    }

    template<typename T>
    bool JComponentDB<T>::GetComponent(std::string entity, T& data){
        if(!components.count(entity)) return false;
        data = components.at(entity);
        return true;
    }

    template<typename T>
    void JComponentDB<T>::RemoveComponent(std::string entity){

    }

    template<typename T>
    JComponentDB<T>& JSystem::GetComponentDB(){
        for(auto& db : ecs->dbs){
            if(db.type_name == typeid(T).name()){
                return (JComponentDB<T>)db;
            }
        }
    }

    template<typename T> // Component type
    void JECS::RegisterDB(JComponentDB<T> db){
        auto it = std::find(dbs.begin(), dbs.end(), db);
        if(it == dbs.end()) return;
        dbs.push_back(db);
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
        for(auto& db : dbs){
            db.RemoveComponent(id);
        }
        entites.erase(it);
    }

    template<typename T> // Component type
    bool JECS::RegisterComponent(std::string id, T comp){
        return false;
    }

    template<typename T> // Component type
    bool JECS::UnRegisterComponent(std::string id){
        return false;
    }

    void JECS::RegisterSystem(JSystem sys){
        auto it = std::find(systems.begin(), systems.end(), sys);
        if(it == systems.end()) return;
        systems.push_back(sys);
        std::sort(systems.begin(), systems.end(), [](JSystem a, JSystem b){
            return a.weight > b.weight;
        });
    }

    void JECS::ExecuteAll(){
        for(auto& system : systems){
            
        }
    }
}