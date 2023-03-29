# SDL-template
## Flexible SDL-imgui template

### requires:
* SDL2 devel pack

### Setup
SDL2 devel pack must be downloaded somewhere on your machine(its best to put it in Program Files (x86) for Windows)

clone the git:
```
git clone --recursive https://github.com/AnghelusAndrei/SDL-imgui-template.git
cd SDL-template
git submodule update --init --recursive
```

build the project for Windows:
```
mkdir build
cd build

cmake -G "MinGW Makefiles" .. 
cmake --build .
```

build the project for Linux:
```
mkdir build
cd build

cmake .. 
cmake --build .
```
