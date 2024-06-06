# Cheat Turbine

_Cheat Engine alternative for linux_

## Table of Contents

- [Features](#features)
- [Screenshots](#screenshots)
- [Getting Started](#getting-started)
- [Credits](#credits)
- [License](#license)

## Features

- Value scanner
- Memory editor
- Extremely unstable BFS pointer scanner

_Cheat turbine is currently under active development, im trying to add new features every day_

_A somewhat blurry roadmap can be found in [src/main.cpp](src/main.cpp)_

## Screenshots
![Screenshot1](https://github.com/rednik1337/CheatTurbine/assets/84843838/2f67c9d5-30e8-4d2c-9163-659e51c0f7b6)


![Screenshot2](https://github.com/rednik1337/CheatTurbine/assets/84843838/bec26e7b-b803-4b5c-9f73-df9219d791c9)

## Getting started

### Compiling

Make sure you have the necessary build tools installed:

``` 
cmake gcc g++
```

Install the recommended runtime dependencies:

```
ttf-jetbrains-mono-nerd
```

Clone and build the project (replace -j16 with your number of threads):

```
git clone --recurse-submodules https://github.com/rednik1337/CheatTurbine.git
cd CheatTurbine
cmake -DCMAKE_BUILD_TYPE=Release -B cmake-build-release -S .
cmake --build cmake-build-release -j16
```

### Running

```
xhost +SI:localuser:root
sudo -EH ./CheatTurbine
```

## Credits

- Cheat Turbine uses [ImGui](https://github.com/ocornut/imgui) as its gui library

## License

- [GPL-2.0](LICENSE)
