#ifndef OBJECT_PROPERTY_H
#define OBJECT_PROPERTY_H

#include "window/io/PlatformMouse.h"

#include <string>
#include <vector>
#include <memory>
#include <imgui.h>
#include <windows.h>

class Property {
public:
    Property(const std::string& name) : name(name), imguiID("##" + std::to_string(idCounter++)) {}
    virtual ~Property() = default;
    virtual void Render() = 0;
    const std::string& GetName() const { return name; }

protected:
    void RenderLabel() const {
        ImGui::AlignTextToFramePadding();
        ImGui::Text(name.c_str());
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
    StringProperty(const std::string& name, std::string* valuePtr) 
        : Property(name), valuePtr(valuePtr) {}

    void Render() override {
        RenderLabel();
        ImGui::InputText(imguiID.c_str(), &(*valuePtr)[0], 32);
        EndRender();
    }

private:
    std::string* valuePtr;
};

class FloatProperty : public Property {
public:
    FloatProperty(const std::string& name, float* valuePtr, float step) : Property(name), valuePtr(valuePtr), step(step) {}

    void Render() override {
        RenderLabel();
        ImGui::DragFloat(imguiID.c_str(), valuePtr, step);
        EndRender();
    }

private:
    float* valuePtr;
    float step;
};

class Vec2FloatProperty : public Property {
public:
    Vec2FloatProperty(const std::string& name, float* valuePtrX, float* valuePtrY, float step) : Property(name), valuePtrX(valuePtrX), valuePtrY(valuePtrY), step(step) {}

    void Render() override {
        RenderLabel();
        ImGui::Text("X");
        ImGui::SameLine();

        float availableWidth = ImGui::GetContentRegionAvail().x;
        float itemWidth = (availableWidth - ImGui::CalcTextSize("X").x - ImGui::CalcTextSize("Y").x + 10 - ImGui::GetStyle().ItemSpacing.x * 4) / 2;

        ImGui::PushItemWidth(itemWidth);
        ImGui::DragFloat((imguiID + "X").c_str(), valuePtrX, 1.0f);
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::Text("Y");
        ImGui::SameLine();

        ImGui::PushItemWidth(-10);
        ImGui::DragFloat((imguiID + "Y").c_str(), valuePtrY, 1.0f);
        EndRender();
    }

private:
    float* valuePtrX;
    float* valuePtrY;
    float step;
};

class IntProperty : public Property {
public:
    IntProperty(const std::string& name, int* valuePtr) : Property(name), valuePtr(valuePtr) {}
    void Render() override {
        RenderLabel();
        ImGui::DragInt(imguiID.c_str(), valuePtr, 1.0f);
        EndRender();
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

private:
    bool* valuePtr;
};

class GroupProperty : public Property {
public:
    GroupProperty(const std::string& name) : Property(name) {}

    void AddProperty(std::shared_ptr<Property> property) {
        properties.push_back(property);
    }

    void Render() override {
        ImGui::SeparatorText(name.c_str());
        for (auto& property : properties) {
            property->Render();
        }
        ImGui::SeparatorText("");
    }

private:
    std::vector<std::shared_ptr<Property>> properties;
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

private:
    std::vector<std::shared_ptr<Property>> properties;
};

#endif // OBJECT_PROPERTY_H
