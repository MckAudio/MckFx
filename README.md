# MckDelay

![MckDelay Logo](Logo/mckdelay_logo.png)

## Description

Small delay plugin to learn some JUCE.

## Dependencies

### Debian / Ubuntu / Rasperry PI OS
```
sudo apt install libasound2-dev libjack-jackd2-dev ladspa-sdk libfreetype6-dev \
    libx11-dev libxcomposite-dev libxcursor-dev libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev libxrender-dev \
    libglu1-mesa-dev mesa-common-dev
```

## Build using CMake

### Linux & MacOS Make
```bash
git submodule update --init --recursive
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
```

### MacOS XCode
```bash
git submodule update --init --recursive
cmake -B build -G Xcode
cmake --build build --config Release
```