# Rocketbox2D and Engine

## Note for "vulkan" branch on GitHub
This branch is a test and is semi-duck-taped together, so don't expect it to compile on your machine.
IF YOU DO WANT TO, HOWEVER:
* You must install Vulkan SDK and have it available as an environment variable ($VULKAN_SDK or platform equivalent)
* After building, a "slang.spv" file will be created in "{build_dir}/VKTestApp/shaders/slang.spv". Copy this file to "{build_dir}/bin/VKTestApp/shaders/slang.spv" to see the demo in action.
    * I haven't put in the time to properly deploy compiled shaders to the right directory (I might compile them at runtime)

Rocketbox2D and Engine are in development.
Engine is not production ready.
