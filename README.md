# Luadio

![Luadio](./examples/Luadio.png)

This application lets you generate audio on the fly using Lua.

# Building
- `git clone https://github.com/japajoe/luadio`
- `cd luadio`
- `git submodule update --init --recursive`
- `mkdir build`
- `cd build`
On linux:
- `cmake -DMINIAUDIOEX_BUILD_PLATFORM=linux_x86_64 ..`
On Windows:
- `cmake -DMINIAUDIOEX_BUILD_PLATFORM=windows ..`
On Mac
- `cmake -DMINIAUDIOEX_BUILD_PLATFORM=mac ..`
- `cmake --build .`
