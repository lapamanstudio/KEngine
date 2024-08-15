#ifndef OBJECT_PROPERTY_H
#define OBJECT_PROPERTY_H

#include <string>
#include <vector>
#include <memory>
#include <imgui.h>
#include <imgui_internal.h>
#include <functional>

#include "window/WindowManager.h"

class Property {
public:
    Property(const std::string& name) : name(name), imguiID("##" + std::to_string(idCounter++)) {}
    virtual ~Property() = default;
    virtual void Render() = 0;
    const std::string& GetName() const { return name; }

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

    void WrapMousePos() {
        ImGuiContext* g = ImGui::GetCurrentContext();
        ImGuiIO& io = g->IO;
        ImVec2& mousePos = io.MousePos;

        GLFWwindow* window = WindowManager::getInstance().getWindow();
        int windowPosX, windowPosY;
        glfwGetWindowPos(window, &windowPosX, &windowPosY);

        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        // io.MousePos is relative from window, so (0, 0)
        ImVec2 windowMin = ImVec2(0, 0);
        ImVec2 windowMax = ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHeight));

        bool wrapped = false;
        const float offset = 1.0f;

        // Horizontal wrapping
        if (io.MouseDelta.x < 0 && mousePos.x <= windowMin.x) {
            glfwSetCursorPos(window, windowPosX + windowMax.x - offset, windowPosY + mousePos.y);
            mousePos.x = windowMax.x - offset;
            wrapped = true;
        } else if (io.MouseDelta.x > 0 && mousePos.x >= windowMax.x - offset) {
            glfwSetCursorPos(window, windowPosX + windowMin.x + offset, windowPosY + mousePos.y);
            mousePos.x = windowMin.x + offset;
            wrapped = true;
        }

        // Vertical wrapping
        if (io.MouseDelta.y < 0 && mousePos.y <= windowMin.y) {
            glfwSetCursorPos(window, windowPosX + mousePos.x, windowPosY + windowMax.y - offset);
            mousePos.y = windowMax.y - offset;
            wrapped = true;
        } else if (io.MouseDelta.y > 0 && mousePos.y >= windowMax.y - offset) {
            glfwSetCursorPos(window, windowPosX + mousePos.x, windowPosY + windowMin.y + offset);
            mousePos.y = windowMin.y + offset;
            wrapped = true;
        }

        // Update IO if wrapping occurred
        if (wrapped) {
            io.MousePosPrev = mousePos;
            io.MouseDelta = ImVec2(0, 0);
            io.WantSetMousePos = true;
        }
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

    void Clear() {
        properties.clear();
    }

private:
    std::vector<std::shared_ptr<Property>> properties;
};

#endif // OBJECT_PROPERTY_H
