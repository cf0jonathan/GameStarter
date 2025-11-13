# SDL2 + Box2D + tinyxml2 Starter Project

## 📺 Setup Video Tutorial

**Watch the setup tutorial:**

[![Setup Tutorial](https://img.youtube.com/vi/smN-sd1F7pU/0.jpg)](https://youtu.be/smN-sd1F7pU)

*Click the thumbnail above to watch the full tutorial on YouTube*

## 📁 Repository

**GitHub Repository:** [https://github.com/NicholasSeward/CPSI-27703-Intro-to-Game-Programming-Fall-2025-Starter/](https://github.com/NicholasSeward/CPSI-27703-Intro-to-Game-Programming-Fall-2025-Starter/)

## ⚠️ Important Notice

**This code is NOT an example of good programming practices for this class.** This project serves as a minimal demonstration to verify that you can successfully:

- Set up a C++ project with SDL2, Box2D, and tinyxml2 using vcpkg
- Build and run a basic physics simulation
- Configure your development environment (VS Code/Cursor) for C++ development

The code itself is not meant to be taken as an exemplar

## What This Project Does

This is a simple falling blocks physics simulation. The following libraries are used:

**Directly demonstrated in the code:**
- SDL2: window creation and rendering
- Box2D: 2D physics simulation
- tinyxml2: XML configuration file loading

**Included and linked for your convenience in CMake (not directly demonstrated):**
- SDL2_ttf: font rendering
- SDL2_image: image loading
- SDL2_mixer: audio playback
- yaml-cpp: YAML configuration file loading
- nlohmann-json: JSON data handling

Blocks spawn at configurable intervals with random colors, fall with realistic physics, and bounce off the ground. The spawn rate and box size can be configured in `assets/config.xml`.

## Available Libraries

This starter project includes a comprehensive set of libraries for game development:

### **Graphics & Rendering**
- **SDL2**: Core window management and 2D rendering
- **SDL2 TTF**: Text rendering with custom fonts
- **SDL2 Image**: Load PNG, JPG, BMP, GIF, TGA images

### **Audio**
- **SDL2 Mixer**: Sound effects and music playback

### **Physics**
- **Box2D**: Professional 2D physics simulation

### **Data Formats**
- **tinyxml2**: XML configuration files
- **yaml-cpp**: YAML configuration files (human-readable)
- **nlohmann-json**: JSON data exchange (web standard)

### **Development**
- **vcpkg**: Cross-platform package manager
- **CMake**: Build system configuration

## Prerequisites for Windows

Before cloning this project, you need to install the following on your Windows machine:

### 1. Git (Required)
- Download from: https://git-scm.com/download/win
- Used for cloning the repository and vcpkg submodule management

### 2. VS Code or Cursor (Recommended)
- VS Code: https://code.visualstudio.com/
- Cursor: https://cursor.sh/
- Install the **C/C++ Extension Pack** for IntelliSense support
- Install the **CMake Tools** extension for building and configuring CMake projects

### 3. Everything Else (Auto-Installed)
The setup script will automatically install:
- **MinGW-w64** (GCC, G++, GDB) via Chocolatey
- **CMake** via Chocolatey
- **Chocolatey** (if not already installed)

## Setup Instructions

### Option 1: Automated Setup (Recommended)
1. **Clone the Repository**
   ```bash
   git clone https://github.com/NicholasSeward/CPSI-27703-Intro-to-Game-Programming-Fall-2025-Starter.git
   cd CPSI-27703-Intro-to-Game-Programming-Fall-2025-Starter
   ```

2. **Run the Setup Script**
   ```bash
   setup.bat
   ```
   
   The script will automatically:
   - Verify all prerequisites are installed
   - **Auto-install Chocolatey (if missing)**
   - **Auto-install MinGW-w64 and CMake via Chocolatey (if missing)**
   - Initialize vcpkg submodule
   - Bootstrap vcpkg
   - Configure CMake
   - Build the project
   - Provide clear success/error messages

### Option 2: Manual Setup
If you prefer to run the commands manually:

1. **Clone the Repository**
   ```bash
   git clone https://github.com/NicholasSeward/CPSI-27703-Intro-to-Game-Programming-Fall-2025-Starter.git
   cd CPSI-27703-Intro-to-Game-Programming-Fall-2025-Starter
   ```

2. **Initialize vcpkg Submodule**
   ```bash
   git submodule update --init --recursive
   ```

3. **Bootstrap vcpkg**
   ```bash
   cd external/vcpkg
   ./bootstrap-vcpkg.bat
   cd ../..
   ```

4. **Configure CMake**
   ```bash
   cmake --preset windows-mingw-debug
   ```

5. **Build the Project**
   ```bash
   cmake --build build/win-mingw-debug
   ```

6. **Run the Application**
   ```bash
   .\build\win-mingw-debug\demo.exe
   ```

## VS Code/Cursor Configuration

### Install Extensions
Install these extensions for the best development experience:
- **C/C++ Extension Pack** (Microsoft)
- **CMake Tools** (Microsoft)

### Ready to Use!
The project includes pre-configured VS Code settings:
- **IntelliSense** - Configured for MinGW with SDL2, Box2D, and tinyxml2
- **Build Tasks** - Press `Ctrl+Shift+P` → "Tasks: Run Task" → "build-mingw-debug"
- **Debug Launch** - Press `F5` to build and run with GDB debugging

All configuration files are already included in the `.vscode/` directory.

## Project Structure

```
├── assets/
│   └── config.xml          # Configuration file
├── build/                  # Build output directory
├── external/
│   └── vcpkg/             # vcpkg package manager
├── src/
│   ├── Game.h             # Game class header
│   ├── Game.cpp           # Game class implementation
│   └── main.cpp           # Entry point
├── .vscode/               # VS Code configuration
│   ├── c_cpp_properties.json  # IntelliSense settings (MinGW)
│   ├── tasks.json         # Build tasks (MinGW only)
│   ├── launch.json        # Debug configuration (GDB)
│   └── settings.json      # CMake settings
├── CMakeLists.txt         # CMake configuration
├── CMakePresets.json      # CMake presets
├── vcpkg.json            # Package dependencies
├── setup.bat             # Automated setup script
├── .gitignore            # Git ignore rules
└── README.md             # This file
```

## Package Management

This project uses **vcpkg** to automatically download, compile, and link all dependencies. The `vcpkg.json` file specifies exactly which libraries are needed, and vcpkg handles the rest.

## Troubleshooting

### Common Issues

1. **"vcpkg not found"**
   - Make sure you've run `git submodule update --init --recursive`
   - Ensure you've bootstrapped vcpkg with `./bootstrap-vcpkg.bat`

2. **"CMake not found" or "MinGW not found"**
   - The setup script should have installed these automatically
   - Try running `setup.bat` again
   - If still failing, install manually:
     - CMake: https://cmake.org/download/
     - MinGW-w64: https://www.mingw-w64.org/downloads/
   - Restart your terminal/VS Code after installation

3. **Build errors**
   - Try cleaning: `cmake --build build/win-mingw-debug --target clean`
   - Reconfigure: `cmake --preset windows-mingw-debug`

### Getting Help

If you encounter issues:
1. Check that Git is installed and in your PATH
2. Try running `setup.bat` again (it should install everything automatically)
3. Ensure you're running commands from the project root directory
4. Check the build output for specific error messages

## Next Steps

Once you have this project building and running successfully, you're ready to:
- Start your actual game development assignments
- Use this as a reference for setting up SDL2 + Box2D projects
- Experiment with different physics behaviors
- Add images, sounds, and text to your games
- Create configuration files in XML, YAML, or JSON formats
- Add your own game features and mechanics

## Example Usage

With the included libraries, you can easily:

```cpp
// Load images
SDL_Surface* image = IMG_Load("assets/sprite.png");

// Render text
TTF_Font* font = TTF_OpenFont("assets/font.ttf", 24);
SDL_Surface* text = TTF_RenderText_Solid(font, "Hello World!", color);

// Play sounds
Mix_Chunk* sound = Mix_LoadWAV("assets/sound.wav");
Mix_PlayChannel(-1, sound, 0);

// Parse JSON
nlohmann::json config = nlohmann::json::parse(file_content);
float spawnRate = config["spawnRate"];

// Parse YAML
YAML::Node config = YAML::LoadFile("config.yaml");
float gravity = config["gravity"].as<float>();
```

Remember: This starter project is just to verify your development environment works. Your actual assignments should follow proper C++ programming practices!
