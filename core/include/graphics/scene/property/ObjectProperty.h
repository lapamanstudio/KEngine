#ifndef OBJECT_PROPERTY_H
#define OBJECT_PROPERTY_H

#include <string>
#include <imgui.h>

class Property {
public:
    Property(const std::string& name) : name(name), imguiID(name + "##" + std::to_string(idCounter++)) {}
    virtual ~Property() = default;
    virtual void Render() = 0;
    const std::string& GetName() const { return name; }

protected:
    std::string name;
    std::string imguiID;

private:
    static int idCounter;
};

class FloatProperty : public Property {
public:
    FloatProperty(const std::string& name, float* valuePtr) : Property(name), valuePtr(valuePtr) {}

    void Render() override {
        ImGui::DragFloat(imguiID.c_str(), valuePtr, 1.0f);
    }

    float GetValue() const { return *valuePtr; }
    void SetValue(float newValue) { *valuePtr = newValue; }

private:
    float* valuePtr;
};

class IntProperty : public Property {
public:
    IntProperty(const std::string& name, int value) : Property(name), value(value) {}
    void Render() override {
        ImGui::DragInt(imguiID.c_str(), &value, 1.0f);
    }
    int GetValue() const { return value; }
    void SetValue(int newValue) { value = newValue; }

private:
    int value;
};

class BoolProperty : public Property {
public:
    BoolProperty(const std::string& name, bool value) : Property(name), value(value) {}
    void Render() override {
        ImGui::Checkbox(imguiID.c_str(), &value);
    }
    bool GetValue() const { return value; }
    void SetValue(bool newValue) { value = newValue; }

private:
    bool value;
};

#endif // OBJECT_PROPERTY_H
