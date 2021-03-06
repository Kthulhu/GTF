# GTF
**Graphics Tools Framework**
A Framework to create C/C++ desktop tools oriented to 2D/3D graphics manipulation. 
This project started as an extraction of reusable code from the <a href="https://www.youtube.com/watch?v=f0P1ZqO0zOk" target="_blank">TexPainter project</a>. 
The idea is to implement a feature-rich library to empower tools like TexPainter and others with similar needs.

Highlighted Features
--------------------
- Multiple windows with shared OpenGL 3 context on Windows and MacOSX
- Abstract render hardware interface 
- 2D Texture resource & loader (PNG, TGA, JPEG and HDR)
- 3D Mesh resource & loader (OBJ and FBX)
- ImGui integration with additional widgets
- Template class collection for custom node graphs

Coming soon
-----------
- Texture writer (PNG, TGA, HDR) 
- Save/Load dialogs helper
- A docking system based on the LumixEngine implementation
- Improved node graph system

Help needed for...
-----------------------------------------
- API Design, some decisions I made are not the best ones
- Replacing GLFW with something that let us use DirectX in the future
- Replacing tinyfielddialogs, while it is a very useful library I prefer a more native-os-api approach
- Experience with *bgfx* to discuss about it's integration here instead of the current render hardware interface

Build
------
	mkdir build
	cd build
	cmake [-G generator] ../

Examples
--------
**MeshViewer Example (video)**
<br />
<a href="http://www.youtube.com/watch?v=1QZ2a1jzXLc" target="_blank">
<img src="http://img.youtube.com/vi/1QZ2a1jzXLc/0.jpg"/>
</a>

**CalcGraph Example (video)**
<br />
<a href="http://www.youtube.com/watch?v=m6eteEPQ0Lg" target="_blank">
<img src="http://img.youtube.com/vi/m6eteEPQ0Lg/0.jpg"/>
</a>

**NoiseGenerator Example (video)**
<br />
<a href="http://www.youtube.com/watch?v=XCk2LmzZI3A" target="_blank">
<img src="http://img.youtube.com/vi/XCk2LmzZI3A/0.jpg"/>
</a>

Credits
-------
Developed by [David Gallardo](https://twitter.com/galloscript) portalg@gmail.com

Libraries
---------
- GLFW - OpenGL windows
- ImGui - Grahic user interface
- glm - maths
- Autodesk FBX - FBX mesh loading
- tiny_obj_loader - OBJ mesh loading
- stb headers - texture loading/write, text utilites, etc...
- glad - OpenGL functions loader
- khrplatform - khronos types
- mikktspace - Synched normals
- tinyfielddialogs - load/save dialogs

Changelog
---------
** 31 Jul 2016 **
- Texture2D resource with loader (JPEG, PNG, TGA, HDR)
- Improved 3D Mesh viewer example with textures (color and normal map)

** 30 Jul 2016 **
- You can now build GTF with CMAKE in Windows and OSX 
- RHI Refactor (removed shared_ptr usage, added configurable vertex attribute list for VAOs)
- Implemented mesh loading (OBJ and FBX)
- Added 3D Mesh viewer Example

