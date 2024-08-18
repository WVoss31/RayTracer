# RayTracer

This Ray Tracer is created following TheCherno's Ray Tracing Series, using his custom GUI framework called Walnut.
I have edited this framework to work for Linux using CMake instead of premake like in the tutorial.


## Requirements
- A Valid C++ IDE, I got it working with Visual Studio Code and CLion.
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (preferably a recent version)
- CMake
- C++ Compiler
- If using VSCode, make sure you have the CMake Extension installed

## Getting Started
In CLion, it should prompt you to create a CMake Profile, click the Plus To add 2 extra profiles. These should be the "Release" and "RelwithDebInfo". 
These are already configured. I recommend you run in "Release" mode for best results.

In VSCode, you should also get a prompt with the cmake extension.
This should also auto complete, make sure the build mode is on "Release" and click the play icon near the "Launch" tag in the cmake extension page.

### 3rd party libaries
- [Dear ImGui](https://github.com/ocornut/imgui)
- [GLFW](https://github.com/glfw/glfw)
- [stb_image](https://github.com/nothings/stb)
- [GLM](https://github.com/g-truc/glm) (included for convenience)

### Additional
- Walnut uses the [Roboto](https://fonts.google.com/specimen/Roboto) font ([Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0))