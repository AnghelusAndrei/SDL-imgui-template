# SDL-template
## Flexible SDL-imgui template

![README](https://user-images.githubusercontent.com/73694888/228629424-d5397c8a-3faa-4ca0-a1a3-2b9d5b0b3df5.png)

### requires:
* SDL2 devel pack
* SDL2_image devel pack
* SDL2_net devel pack

### Setup
* SDL2 devel pack must be downloaded somewhere on your machine(its best to put it in Program Files (x86) for Windows)
* SDL2_image devel pack must be downloaded somewhere on your machine(its best to put it in Program Files (x86) for Windows)
* SDL2_net devel pack must be downloaded somewhere on your machine(its best to put it in Program Files (x86) for Windows)

clone the git:
```
git clone --recursive https://github.com/AnghelusAndrei/SDL-imgui-template.git
cd SDL-template
git submodule update --init --recursive
```

build the project for Windows:

1.build the server:
```
cd server
mkdir build
cd build

cmake -G "MinGW Makefiles" .. 
cmake --build .
```

2.build the client:
```
cd client
mkdir build
cd build

cmake -G "MinGW Makefiles" .. 
cmake --build .
```

build the project for Linux:

1.build the server:
```
cd server
mkdir build
cd build

cmake .. 
cmake --build .
```

2.build the client:
```
cd client
mkdir build
cd build

cmake .. 
cmake --build .
```

##You can find the exeecutable in either of the bin directories##
