# SDL-template
## Flexible SDL-imgui template

![README](https://user-images.githubusercontent.com/73694888/228629424-d5397c8a-3faa-4ca0-a1a3-2b9d5b0b3df5.png)

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
