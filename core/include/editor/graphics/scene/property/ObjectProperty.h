#ifndef OBJECT_PROPERTY_H
#define OBJECT_PROPERTY_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <memory>
#include <imgui.h>
#include <imgui_internal.h>
#include <functional>

#include "editor/window/WindowManager.h"
#include "editor/window/io/PlatformMouse.h"

class Property {
public:
    Property(const std::string& name) : name(name), imguiID("##" + std::to_string(idCounter++)) {}
    virtual ~Property() = default;
    
    virtual void Render() = 0;
    const std::string& GetName() const { return name; }

    virtual nlohmann::json Serialize() const = 0;
    virtual void Deserialize(const nlohmann::json& j) = 0;

protected:
    void RenderLabel() const {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(name.c_str());
        ImGui::SameLine(100);
        ImGui::PushItemWidth(-10);
        ImGui::SameLine();
    }

    void EndRender() const {
        ImGui::PopItemWidth();
    }

    std::string name;
    std::string imguiID;

private:
    static int idCounter;
};

class StringProperty : public Property {
public:
    StringProperty(const std::string& name, std::string* valuePtr, std::function<void()> callback = nullptr) 
        : Property(name), valuePtr(valuePtr), callback(callback) {}

    void Render() override {
        RenderLabel();

        char buffer[256];
        strncpy(buffer, valuePtr->c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';

        if (ImGui::InputText(imguiID.c_str(), buffer, sizeof(buffer))) {
            *valuePtr = buffer;
            if (callback) callback();
        }
        EndRender();
    }

    nlohmann::json Serialize() const override {
        return *valuePtr;
    }

    void Deserialize(const nlohmann::json& j) override {
        *valuePtr = j.get<std::string>();
    }

private:
    std::string* valuePtr;
    std::function<void()> callback;
};

class FloatProperty : public Property {
public:
    using FilterFunc = std::function<void(float&)>;

    FloatProperty(const std::string& name, float* valuePtr, float step, FilterFunc filterFunc = nullptr, float minVal = -9999999, float maxVal = 9999999) 
        : Property(name), valuePtr(valuePtr), step(step), filterFunc(filterFunc), minVal(minVal), maxVal(maxVal) {}

    void Render() override {
        RenderLabel();
        if (ImGui::DragFloat(imguiID.c_str(), valuePtr, step, minVal, maxVal, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
            if (filterFunc) filterFunc(*valuePtr);
        }
        if (ImGui::IsItemActive())
            WrapMousePos();
        EndRender();
    }

    nlohmann::json Serialize() const override {
        return *valuePtr;
    }

    void Deserialize(const nlohmann::json& j) override {
        *valuePtr = j.get<float>();
    }

private:
    float* valuePtr;
    float step;
    FilterFunc filterFunc;
    float minVal;
    float maxVal;
};

class Vec2FloatProperty : public Property {
public:
    Vec2FloatProperty(const std::string& name, float* valuePtrX, float* valuePtrY, float step, float minVal = -9999999, float maxVal = 9999999) 
        : Property(name), valuePtrX(valuePtrX), valuePtrY(valuePtrY), step(step), minVal(minVal), maxVal(maxVal) {}

    void Render() override {
        RenderLabel();
        ImGui::TextUnformatted("X");
        ImGui::SameLine();

        float availableWidth = ImGui::GetContentRegionAvail().x;
        float itemWidth = (availableWidth - ImGui::CalcTextSize("X").x - ImGui::CalcTextSize("Y").x + 10 - ImGui::GetStyle().ItemSpacing.x * 4) / 2;

        ImGui::PushItemWidth(itemWidth);
        ImGui::DragFloat((imguiID + "X").c_str(), valuePtrX, step, minVal, maxVal, "%.3f", ImGuiSliderFlags_AlwaysClamp);
        if (ImGui::IsItemActive())
            WrapMousePos();
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::TextUnformatted("Y");
        ImGui::SameLine();

        ImGui::PushItemWidth(-10);
        ImGui::DragFloat((imguiID + "Y").c_str(), valuePtrY, step, minVal, maxVal, "%.3f", ImGuiSliderFlags_AlwaysClamp);
        if (ImGui::IsItemActive())
            WrapMousePos();
        EndRender();
    }

    nlohmann::json Serialize() const override {
        return { *valuePtrX, *valuePtrY };
    }

    void Deserialize(const nlohmann::json& j) override {
        *valuePtrX = j[0].get<float>();
        *valuePtrY = j[1].get<float>();
    }

private:
    float* valuePtrX;
    float* valuePtrY;
    float step;
    float minVal;
    float maxVal;
};

class IntProperty : public Property {
public:
    IntProperty(const std::string& name, int* valuePtr) : Property(name), valuePtr(valuePtr) {}
    
    void Render() override {
        RenderLabel();
        ImGui::DragInt(imguiID.c_str(), valuePtr, 1.0f);
        if (ImGui::IsItemActive())
            WrapMousePos();
        EndRender();
    }

    nlohmann::json Serialize() const override {
        return *valuePtr;
    }

    void Deserialize(const nlohmann::json& j) override {
        *valuePtr = j.get<int>();
    }

private:
    int* valuePtr;
};

class BoolProperty : public Property {
public:
    BoolProperty(const std::string& name, bool* valuePtr) : Property(name), valuePtr(valuePtr) {}
    
    void Render() override {
        RenderLabel();
        ImGui::Checkbox(imguiID.c_str(), valuePtr);
        EndRender();
    }

    nlohmann::json Serialize() const override {
        return *valuePtr;
    }

    void Deserialize(const nlohmann::json& j) override {
        *valuePtr = j.get<bool>();
    }

private:
    bool* valuePtr;
};

class GroupProperty : public Property {
public:
    GroupProperty(const std::string& name, bool renderButton, std::function<void()> callback = nullptr) : Property(name), renderButton(renderButton), callback(callback) {}

    void AddProperty(std::shared_ptr<Property> property) {
        properties.push_back(property);
    }

    void Render() override {
        ImGui::SeparatorText(name.c_str());

        if (renderButton) {
            ImVec2 contentRegion = ImGui::GetContentRegionAvail();
            ImGui::SameLine(contentRegion.x - 20);
            if (ImGui::Button("X", ImVec2(25, 0)))
                if (callback) callback();
        }

        // Render properties
        for (auto& property : properties) {
            property->Render();
        }
    }

    nlohmann::json Serialize() const {
        nlohmann::json j;
        for (const auto& property : properties) {
            j[property->GetName()] = property->Serialize();
        }
        return j;
    }

    void Deserialize(const nlohmann::json& j) {
        for (const auto& property : properties) {
            if (j.contains(property->GetName())) {
                property->Deserialize(j[property->GetName()]);
            }
        }
    }

private:
    std::vector<std::shared_ptr<Property>> properties;
    bool renderButton;
    std::function<void()> callback;
};

class Properties {
public:
    void AddProperty(std::shared_ptr<Property> property) {
        properties.push_back(property);
    }

    void Render() const {
        for (const auto& property : properties) {
            property->Render();
        }
    }

    void Clear() {
        properties.clear();
    }

    nlohmann::json Serialize() const {
        nlohmann::json j;
        for (const auto& property : properties) {
            j[property->GetName()] = property->Serialize();
        }
        return j;
    }

    void Deserialize(const nlohmann::json& j) {
        for (const auto& property : properties) {
            if (j.contains(property->GetName())) {
                printf("Deserializing property: %s\n", property->GetName().c_str());
                property->Deserialize(j[property->GetName()]);
            }
        }
    }

private:
    std::vector<std::shared_ptr<Property>> properties;
};

#endif // OBJECT_PROPERTY_H
