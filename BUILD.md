# How to build project

## Requirements

Conan2 is required

## Prepare

With Conan at first we need to make sure that our profile is configured:
```bash
conan profile detect --force
```

## Build

```bash
conan install . --output-folder=build --build=missing
cmake --preset conan-default
cmake --build --preset conan-release
```
