# sRAT-RT: Spectral Renderer for Atmospheres and other Tasks, in Real Time

A spectral real-time renderer based on OpenGL 4.5 for my Master's Thesis project. For now it is not functional at all, I'm just starting to implement it. I will update the status as I implement new functionalities and make progress. The goal would be to have a general renderer for spectral scenes, and ideally be able to perform some kind of spectral uplifting for RGB textures, as in [Jakob and Hanika, 2019](https://rgl.epfl.ch/publications/Jakob2019Spectral).

I also like the idea of being able to render skies, so I´ll try to incorporate atmospheric rendering and ideally, clouds in real time. Let's see if I can make any of this work.
\[UPDATE March 11th\]: for now I'll limit myself to real time spectral uplifting, I'll change the name of the repo when the project is more mature

This project is based on the CMakeLists.txt that can be found in the repo [imgui-glfw-glad](https://github.com/cmmw/imgui-glfw-glad-glm) as a base, with some modifications to also use [assimp](https://github.com/assimp/assimp).

# Compilation instructions

With a terminal, go to the root folder of the repository and execute the following commands:

```shell
cmake -S . -B build
cmake --build build --target sRAT-RT -j4
```

It is possible to substitute the first command with a call to ```cmake``` or ```cmake-gui```.
The second command can also be replaced by a regular compilation in Visual Studio (tested with the 2019 version)

# How to use

TODO (WIP)

# License
 
 TODO (WIP)
