#ifndef OBJECT_COMPONENT_FACTORY_H
#define OBJECT_COMPONENT_FACTORY_H

#include <functional>
#include <unordered_map>
#include <memory>
#include <string>

class ObjectComponent;

using ComponentFactoryMap = std::unordered_map<std::string, std::function<std::shared_ptr<ObjectComponent>(std::weak_ptr<IObject>)>>;

class ObjectComponentFactory {
public:
    static ComponentFactoryMap& GetComponentFactoryMap() {
        static ComponentFactoryMap factoryMap;
        return factoryMap;
    }

    template<typename T>
    static void RegisterComponent(const std::string& componentName) {
        GetComponentFactoryMap()[componentName] = [](std::weak_ptr<IObject> parent) -> std::shared_ptr<ObjectComponent> {
            return std::make_shared<T>(parent);
        };
    }

    static std::shared_ptr<ObjectComponent> CreateComponent(const std::string& componentName, std::weak_ptr<IObject> parent) {
        auto it = GetComponentFactoryMap().find(componentName);
        if (it != GetComponentFactoryMap().end()) {
            return it->second(parent);
        }
        printf("Component %s not found\n", componentName.c_str());
        return nullptr; 
    }
};

#endif // OBJECT_COMPONENT_FACTORY_H
