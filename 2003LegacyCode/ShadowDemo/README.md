                =======================================
                       Shadows and Mesh Loading
                             Jason Dudash

                CS5984 - Computer Graphics, Spring 2003

                            22 April 2003
                =======================================

* This document has been moved to "[install dir]/ShadowDemo/docs/html/index.html"
  this version is not guarenteed to be up to date.

--------------------------------------------------------------------------------
Project Overview:
--------------------------------------------------------------------------------
  This project demonstrates real-time shadowing techniques as well as an
example of loading 3D geometry information from a file.  These topics are
explored in more detail in the accompanying paper: "Shadows and Mesh Loading".


--------------------------------------------------------------------------------
Features:
--------------------------------------------------------------------------------
- Texture Mapping, Mipmapping, Skinabble Meshes
- Point Lights
- Planar Projected Shadows
- Smooth Shading (Gouraud)
- Mesh Loading (from .x files)
- Double Buffering
- OpenGL Display Lists (for improved rendering speeds)
- Scene Based Rendering


--------------------------------------------------------------------------------
How to Build & Dependencies:
--------------------------------------------------------------------------------
  This project was developed in Microsoft Visual Studio 6.0.  It requires that
you have installed GLUT.  If you are attempting to bulid this project in another
development environment make sure you link in: glaux.lib.

  Dependencies include:
    -# tank.x
    -# greentank.bmp
    -# browntank.bmp
    -# fireball.bmp
    -# grass.bmp

--------------------------------------------------------------------------------
How To Run:
--------------------------------------------------------------------------------
Goto "[install dir]/ShadowDemo/Release" and double-click "ShadowDemo.exe".
Or from a command prompt type "[install dir]/ShadowDemo/Release/ShadowDemo.exe"    