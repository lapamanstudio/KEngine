CC = g++
CXXFLAGS = -Wall -g $(shell pkg-config --cflags glfw3)
LIBS = $(shell pkg-config --libs glfw3) -lopengl32 -lglfw3 -lglew32
INC_DIRS = $(shell find core/include -type d | sed 's/^/-I/') -Icore/include -I.deps/imgui -I.deps/glm -I.deps/glfw/include -I.deps/glut/include
ICON_RES = icon.res

# Innosetup compiler
INNO_SETUP_COMPILER = installer/ISCC.exe
INNO_SETUP_SCRIPT = installer/installer.iss

OUT_DIR = out

# Find all .cpp files for the project and the ImGui source files
CORE_SRCS := $(shell find core/src -type f \( -name "*.cpp" -o -name "*.c" \))
IMGUI_SRCS := .deps/imgui/imgui.cpp \
              .deps/imgui/imgui_draw.cpp \
              .deps/imgui/imgui_tables.cpp \
              .deps/imgui/imgui_widgets.cpp \
              .deps/imgui/backends/imgui_impl_glfw.cpp \
              .deps/imgui/backends/imgui_impl_opengl3.cpp

# Combine all sources
SRCS := $(CORE_SRCS) $(IMGUI_SRCS)

# Create object file paths
OBJS := $(SRCS:%.cpp=$(OUT_DIR)/%.o)
OBJS := $(OBJS:%.c=$(OUT_DIR)/%.o)

# Create necessary directories from object file paths
DIRS := $(dir $(OBJS))

# Rule to make directories
$(foreach dir,$(DIRS),$(shell mkdir -p $(dir)))

# Default target
all: $(OUT_DIR)/KEngine

pre-build:
	@echo "Checking binaries and libraries..."
	@if [ -z "$${PKG_CONFIG_PATH}" ] || [[ ":$${PKG_CONFIG_PATH}:" != *":/c/msys64/mingw64/lib/pkgconfig:"* ]]; then \
		echo 'You must do: `export PKG_CONFIG_PATH=/c/msys64/mingw64/lib/pkgconfig:$${PKG_CONFIG_PATH}`'; \
		exit 1; \
	fi
	@if ! PKG_CONFIG_PATH=$$PKG_CONFIG_PATH pkg-config --exists glfw3; then \
		echo "Error: glfw3 package not found. Please make sure glfw3 and its development files are installed."; \
		echo "You can install it with: `pacman -S mingw-w64-x86_64-glfw`"; \
		exit 1; \
	fi
	@echo "GLFW3 found."
	@if ! command -v g++ >/dev/null; then \
		if [ -x "/mingw64/bin/g++" ]; then \
			echo 'You must do: `export PATH=/mingw64/bin:$${PATH}`'; \
			exit 1; \
		else \
			echo "Error: g++ not found. Please make sure a C++ compiler is installed."; \
			echo "You can install it with: `pacman -S mingw-w64-x86_64-gcc`"; \
			exit 1; \
		fi; \
	fi
	@echo "g++ found."
	@echo "Preparing dependencies..."
	@if [ ! -d ".deps" ]; then \
		mkdir -p .deps; \
	fi
	@if [ ! -d ".deps/imgui" ]; then \
		git clone --branch docking https://github.com/ocornut/imgui.git .deps/imgui && \
		echo "Cloned ImGui repository to .deps folder."; \
	fi
	@if [ ! -d ".deps/glfw" ]; then \
		git clone https://github.com/glfw/glfw.git .deps/glfw && \
		echo "Cloned GLFW repository to .deps folder."; \
	fi
	@if [ ! -d ".deps/glm" ]; then \
		git clone https://github.com/g-truc/glm.git .deps/glm && \
		echo "Cloned GLM repository to .deps folder."; \
	fi
	@if [ ! -d ".deps/glut" ]; then \
		git clone https://github.com/markkilgard/glut.git .deps/glut && \
		echo "Cloned GLUT repository to .deps folder."; \
	fi
	@echo "Dependencies ready."

installer: all
	$(INNO_SETUP_COMPILER) $(INNO_SETUP_SCRIPT)

# Link the application
$(OUT_DIR)/KEngine: $(OBJS)
	$(CC) $^ $(LIBS) -o $@ $(ICON_RES)

# Compilation rules for source files
$(OUT_DIR)/%.o: %.cpp
	$(CC) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

$(OUT_DIR)/%.o: %.c
	$(CC) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

# Special rules for ImGui sources in subdirectories
$(OUT_DIR)/.deps/imgui/%.o: .deps/imgui/%.cpp
	$(CC) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

$(OUT_DIR)/.deps/imgui/backends/%.o: .deps/imgui/backends/%.cpp
	$(CC) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(OUT_DIR)

# Test target
tests: test/opengl/grid/main
	./$<

test/opengl/grid/main: test/opengl/grid/main.cpp
	$(CC) $(CXXFLAGS) $(INC_DIRS) $< -o $@ $(LIBS)

.PHONY: all clean tests
