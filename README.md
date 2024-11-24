# Cheat Turbine
_Cheat Engine alternative for linux_

## Table of Contents
- [Features](#features)
- [Screenshots](#screenshots)
- [Getting started](#getting-started)
    - [Building from source (recommended)](#building-from-source-recommended)
    - [Downloading the binary](#downloading-the-binary)
    - [Running](#running)
- [Credits](#credits)
- [License](#license)

## Features
- Value scanner
- Memory editor
- Structure dissector
- Extremely unstable BFS pointer scanner

_Cheat turbine is currently under active development, so expect this list to grow_

_A somewhat blurry roadmap can be found in [src/main.cpp](src/main.cpp)_

## Screenshots
![Screenshot1](https://github.com/rednik1337/CheatTurbine/assets/84843838/2f67c9d5-30e8-4d2c-9163-659e51c0f7b6)


![Screenshot2](https://github.com/rednik1337/CheatTurbine/assets/84843838/bec26e7b-b803-4b5c-9f73-df9219d791c9)


![Pasted image (2)](https://github.com/rednik1337/CheatTurbine/assets/84843838/6c5f9b98-83f7-4cca-bbda-65d9587513b8)


## Getting started
### Building from source (recommended)
Make sure you have the necessary build tools and dependencies installed:
```
cmake gcc g++ libxinerama libxcursor xorg glu
```

Clone and build the project:
```
git clone --recurse-submodules https://github.com/rednik1337/CheatTurbine.git
cd CheatTurbine
cmake -DCMAKE_BUILD_TYPE=Release -B cmake-build-release -S .
cmake --build cmake-build-release -j$(nproc)
```

If successful, the binary will be located at `cmake-build-release/CheatTurbine`

### Downloading the binary
Latest artifacts can be found in [github actions](https://github.com/rednik1337/CheatTurbine/actions)

It is [recommened](https://github.com/KhronosGroup/Vulkan-Loader/issues/1538#issuecomment-2297283853) to use the non appimage version of the binary if you can

### Running
```
xhost +SI:localuser:root
sudo -EH ./CheatTurbine
```

## Credits
- Cheat Turbine uses [ImGui](https://github.com/ocornut/imgui) as its gui library

## License
- [GPL-2.0](LICENSE)
