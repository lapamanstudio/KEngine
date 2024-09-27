# KEngine - Open Source Game Engine

KEngine is an open-source 2D game creation engine designed to offer a platform for learning the inner workings of game development. This project provides a basic interface to facilitate simple game creation using C#. Our long-term vision is to evolve KEngine into a versatile game engine, drawing inspiration from proprietary software like Unity, but with a commitment to remaining open source.

## Objectives:

- [x] **Basic Interface:**
   - Develop an intuitive and user-friendly interface to streamline the game development process. Focus on simplicity for beginners while offering depth for more advanced users.

- [ ] **2D Game Creation:**
   - Provide a framework for creating 2D games using the power and flexibility of C#. Enable users to unleash their creativity without a steep learning curve.

- [ ] **Sprite Editor:**
   - Implement a robust sprite editor that allows users to easily create, modify, and manage sprites for their games. Ensure compatibility with various image formats.

- [ ] **Plugin Integration:**
   - Enable users to create their own plugins and share them with the community.

- [ ] **3D Game Development (Future Goal):**
   - Plan and integrate features for 3D game development. This includes enhancing the engine's capabilities to support three-dimensional graphics, physics, and interactions.

## Compilation Guide:

To compile KEngine, follow these steps:

1. **Install MSYS:**
   - Set up MSYS with OpenGL libraries, ensuring compatibility with the engine's requirements.

2. **Install Dependencies:**
   - Install GLFW and GLEW to enable OpenGL support. Refer to their respective documentation for detailed installation instructions.

3. **Clone the Repository:**
   - Clone the KEngine repository to your local machine using the following command:
     ```
     git clone https://github.com/lapamanstudio/KEngine
     ```

4. **Build the Project:**
   - Navigate to the project directory and follow the build instructions provided in the project's documentation. This usually involves running build scripts or using an integrated development environment (IDE) that supports the engine.

## Installer Creation Guide:

To create an installer for KEngine, use the following steps:

1. **Compile the Project:**
   - Ensure that the project is successfully compiled and all dependencies are satisfied.

2. **Package the Application:**
   - Use packaging tools to bundle the compiled files, assets, and dependencies into a distributable package.

3. **Create Installer:**
   - Use make to create the installer with msys:
     ```
     make installer
     ```

4. **Documentation:**
   - Include clear documentation with the installer, guiding users through the installation process and any potential troubleshooting steps.

## Libraries used on the project:
- [GLFW](https://github.com/glfw/glfw)
- [GLM](https://github.com/g-truc/glm)
- [FREETYPE](https://github.com/freetype/freetype)
- [IMGUI](https://github.com/ocornut/imgui)
- [JSON](https://github.com/nlohmann/json)

Feel free to reach out for any assistance or clarification. Happy game development with KEngine!
