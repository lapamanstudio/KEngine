# Detect the OS
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    OS := Linux
else ifeq ($(UNAME_S),Darwin)
    OS := macOS
else ifneq ($(findstring MINGW,$(UNAME_S)),)
    OS := Windows
else ifneq ($(findstring MSYS,$(UNAME_S)),)
    OS := Windows
else ifneq ($(findstring CYGWIN,$(UNAME_S)),)
    OS := Windows
else
    OS := Unknown
endif

# For debugging purposes, print the detected OS
$(info Detected OS: $(OS))

# Compiler and flags
CC = g++
CXXFLAGS = -Wall -g $(shell pkg-config --cflags glfw3)

# Include directories
INC_DIRS = -Icore/include -I.deps/imgui -I.deps/glm -I.deps/glfw/include -I.deps/glut/include -I.deps/freetype/include -I.deps/json/include

OUT_DIR = out

# Libraries and ICON_RES_FLAG
ifeq ($(OS),Linux)
    LIBS = $(shell pkg-config --libs glfw3 glew freetype2) -lGL -lX11
    ICON_RES_FLAG =
else ifeq ($(OS),macOS)
    LIBS = -framework OpenGL $(shell pkg-config --libs glfw3 glew freetype2)
    ICON_RES_FLAG =
else ifeq ($(OS),Windows)
    LIBS = $(shell pkg-config --libs glfw3) -lopengl32 -lglfw3 -lglew32 -lfreetype
    ICON_RES = icon.res
    ICON_RES_FLAG = $(OUT_DIR)/$(ICON_RES)
    INNO_SETUP_COMPILER = installer/ISCC.exe
    INNO_SETUP_SCRIPT = installer/installer.iss
else
    $(error Unsupported OS: $(OS))
endif

DATAFILES_DIR = $(OUT_DIR)/datafiles
RESOURCE_DIR = resources

# Find all .cpp and .c files for the project
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
DIRS := $(sort $(dir $(OBJS)))

# Rule to make directories
$(shell mkdir -p $(DIRS))

# Default target
all: $(OUT_DIR)/KEngine copy-datafiles

# Determine the dependencies and linker flags based on the OS
ifeq ($(ICON_RES_FLAG),)
    KENGINE_DEPS := $(OBJS)
    KENGINE_LDFLAGS :=
else
    KENGINE_DEPS := $(OBJS) $(ICON_RES_FLAG)
    KENGINE_LDFLAGS := $(ICON_RES_FLAG)
endif

# Build the KEngine executable
$(OUT_DIR)/KEngine: $(KENGINE_DEPS)
	$(CC) $(OBJS) $(LIBS) -o $@ $(KENGINE_LDFLAGS)

# Rule for creating the icon resource file (Windows only)
ifeq ($(OS),Windows)
$(ICON_RES_FLAG): icon.rc
	windres icon.rc -O coff -o $@
endif

# Installer target (Windows only)
installer: all
ifeq ($(OS),Windows)
	$(INNO_SETUP_COMPILER) $(INNO_SETUP_SCRIPT)
else
	@echo "Installer not supported on $(OS)."
endif

# Copy data files (e.g., images) to the out/datafiles/ directory
copy-datafiles: $(DATAFILES_DIR)
	@echo "Copying data files to $(DATAFILES_DIR)..."
	cp -r $(RESOURCE_DIR)/* $(DATAFILES_DIR)

# Create the datafiles directory
$(DATAFILES_DIR):
	@echo "Creating datafiles directory at $(DATAFILES_DIR)..."
	mkdir -p $(DATAFILES_DIR)

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

# Run target
run: $(OUT_DIR)/KEngine
	$(RUN_ENV) ./$(OUT_DIR)/KEngine

# Debug target
debug: $(OUT_DIR)/KEngine
	@if ! command -v gdb >/dev/null; then \
		echo "Error: gdb not found. Please make sure gdb is installed and in your PATH."; \
		exit 1; \
	fi
	gdb ./$(OUT_DIR)/KEngine

.PHONY: all clean tests run debug copy-datafiles
