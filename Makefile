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

# Debug output
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
    LIBS = $(shell pkg-config --libs glfw3) -lopengl32 -lglfw3 -lglew32 -lfreetype -ld3d11
    ICON_RES = icon.res
    ICON_RES_FLAG = $(OUT_DIR)/$(ICON_RES)
    INNO_SETUP_COMPILER = installer/ISCC.exe
    INNO_SETUP_SCRIPT = installer/installer.iss
else
    $(error Unsupported OS: $(OS))
endif

DATAFILES_DIR = $(OUT_DIR)/datafiles
RESOURCE_DIR = resources

# Source files
ENGINE_SRCS := $(shell find core/src/engine -type f \( -name "*.cpp" -o -name "*.c" \))
ENGINE_SRCS_NO_MAIN := $(filter-out core/src/engine/main.cpp, $(ENGINE_SRCS))
EDITOR_SRCS := $(shell find core/src/editor -type f \( -name "*.cpp" -o -name "*.c" \))

# ImGui sources
IMGUI_SRCS := .deps/imgui/imgui.cpp \
              .deps/imgui/imgui_draw.cpp \
              .deps/imgui/imgui_tables.cpp \
              .deps/imgui/imgui_widgets.cpp \
              .deps/imgui/backends/imgui_impl_glfw.cpp \
              .deps/imgui/backends/imgui_impl_opengl3.cpp

# Combine all sources for the editor
EDITOR_SRCS := $(EDITOR_SRCS) $(IMGUI_SRCS)

# Object files
EDITOR_OBJS := $(EDITOR_SRCS:%.cpp=$(OUT_DIR)/%.o)
EDITOR_OBJS := $(EDITOR_OBJS:%.c=$(OUT_DIR)/%.o)
ENGINE_OBJS := $(ENGINE_SRCS:%.cpp=$(OUT_DIR)/%.o)
ENGINE_OBJS := $(ENGINE_OBJS:%.c=$(OUT_DIR)/%.o)
ENGINE_OBJS_NO_MAIN := $(filter-out $(OUT_DIR)/core/src/engine/main.o, $(ENGINE_OBJS))

# Create necessary directories from object file paths
DIRS := $(sort $(dir $(EDITOR_OBJS) $(ENGINE_OBJS)))

# Ensure directories exist
$(DIRS):
	@mkdir -p $@

# Default target
all: $(OUT_DIR) $(OUT_DIR)/KEngine $(OUT_DIR)/test copy-datafiles

# Ensure output directory exists
$(OUT_DIR):
	@mkdir -p $(OUT_DIR)

# Build the KEngine executable (editor)
$(OUT_DIR)/KEngine: $(EDITOR_OBJS) $(ENGINE_OBJS_NO_MAIN) $(ICON_RES_FLAG)
	$(CC) $(EDITOR_OBJS) $(ENGINE_OBJS_NO_MAIN) $(LIBS) -o $@ $(ICON_RES_FLAG)

# Build the test executable (engine standalone)
$(OUT_DIR)/test: $(ENGINE_OBJS) $(ICON_RES_FLAG)
	$(CC) $(ENGINE_OBJS) $(LIBS) -o $@ $(ICON_RES_FLAG)

# Rule to build icon.res
$(ICON_RES_FLAG): icon.rc | $(OUT_DIR)
ifeq ($(OS),Windows)
	@echo "Compiling icon resource..."
	windres icon.rc -O coff -o $@
else
	@echo "Icon resource compilation skipped on $(OS)."
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
$(OUT_DIR)/%.o: %.cpp | $(DIRS)
	$(CC) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

$(OUT_DIR)/%.o: %.c | $(DIRS)
	$(CC) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

# Special rules for ImGui sources in subdirectories
$(OUT_DIR)/.deps/imgui/%.o: .deps/imgui/%.cpp | $(DIRS)
	$(CC) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

$(OUT_DIR)/.deps/imgui/backends/%.o: .deps/imgui/backends/%.cpp | $(DIRS)
	$(CC) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(OUT_DIR)

# Run the editor (KEngine)
run-editor: $(OUT_DIR)/KEngine copy-datafiles
	$(RUN_ENV) ./$(OUT_DIR)/KEngine

# Run the engine (test)
run-engine: $(OUT_DIR)/test
	$(RUN_ENV) ./$(OUT_DIR)/test

# Debug target
debug: $(OUT_DIR)/KEngine
	@if ! command -v gdb >/dev/null; then \
		echo "Error: gdb not found. Please make sure gdb is installed and in your PATH."; \
		exit 1; \
	fi
	gdb ./$(OUT_DIR)/KEngine

.PHONY: all clean run-editor run-engine debug copy-datafiles
