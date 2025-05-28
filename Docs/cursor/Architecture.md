# RightEngine Architecture

RightEngine is modular game engine based on Vulkan 1.2 for rendering and entt library for ECS for data managment. It uses component-based system for all interactions. Diffrenet systems can be run on diffrent threads ensuring better performance. Engine supports C++ to JSON serialization/deserialization using rttr and simdjson/nlohmannjson libs. It uses EASTL instead of STL for containers and iterators. It uses ImGui to draw editor.

Engine consists of 3 main modules: Core (src/Core), Engine (src/Engine), RHI (src/RHI):
- Core consits of basic files like math, hash, string operations. Also it has important bindings for rttr and eastl.
- Engine consists of engine code, like asset system or rendering bindings
- RHI abstaracts rendering APIs (currently only Vulkan) and handles all iteractions with GPU.

**Engine architrecture description**

There are 2 main parts of the engine - Systems and Services. 
- Services are global objects that can be accessed anywhere and they are used to handle all the interactions with other world, like assets, GPU, ImGui. Services are updated in main thread sequentially.
- Systems are much smaller and are binded to a certain world. Systems may be updated in parallel, so users can register the order of systems update, to prevent data races. Systems have access to a components where all the data is stored.

## TECHNOLOGY STACK

- **Programming Languages**: C++, Python
- **Build System**: CMake
- **Graphics**: Custom rendering engine (Vulkan 1.2)
- **Data Storage**: Custom serialization system
- **Version Control**: Git with LFS for large assets
- **CI/CD**: Github Actions
- **Scripting**: C++ using ECS system
- **Testing**: doctest

## DESIGN DECISIONS

### ECS

The game uses a component-based architecture which store only data, all game logic are written in separate systems that updates in parallel. System update oreder can be configured in code.

### Editor Integration

The game includes a comprehensive level editor that allows designers to create and test levels without developer intervention. The editor supports:

- Placing game elements in the field

## GETTING STARTED

To get started with the Homescapes codebase:

1. **Clone the repository**:
   ```
   git clone https://github.com/RGameStudio/RightEngine.git
   git lfs install
   git lfs pull
   ```

2. **Install dependencies**:
   - CMake (3.19 or higher)
   - Python (3.9 or higher)
   - Poetry (2.1.3 or higher)

3. **Prepare the environment**:
   ```
   cd homescapes
   ./prepare_env.sh for macos or ./prepare_env.bat for windows
   ```

4. **Generate solution**:
   ```
   cmake --preset debug
   ```

5. **Build the engine**:
   ```
   cmake --build --preset engine-win-debug
   ```

## ENTRY POINTS

The main entry points to the codebase are:

1. **Src/Engine/Engine/Engine.cpp**: Contains the engine intialization and engine main update loop
2. **Src/Engine/Engine/Service**: All engine services
3. **Src/Engine/Engine/System**: Engine systems
4. **Src/Engine/Engine/Tests**: All engine unit tests
