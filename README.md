# sRAT-RT: Spectral Renderer for Atmospheres and other Tasks, in Real Time

A spectral real-time renderer based on OpenGL 4.5 for my Master's Thesis project. For now it is not functional at all, I'm just starting to implement it. I will update the status as I implement new functionalities and make progress. The goal would be to have a general renderer for spectral scenes, and ideally be able to perform some kind of spectral uplifting for RGB textures, as in [Jakob and Hanika, 2019](https://rgl.epfl.ch/publications/Jakob2019Spectral).

I also like the idea of being able to render skies, so I´ll try to incorporate atmospheric rendering and ideally, clouds in real time. Let's see if I can make any of this work.

This project is based on the CMakeLists.txt that can be found in the repo [imgui-glfw-glad](https://github.com/cmmw/imgui-glfw-glad-glm) as a base, with some modifications to also use [assimp](https://github.com/assimp/assimp).

# Compilation instructions

With a terminal, go to the root folder of the repository and execute the following commands:

```shell
cmake -S . -B build
cmake --build build --target sRAT-RT -j4
```

# How to use

TODO (WIP)

# License
 
 TODO (WIP)
