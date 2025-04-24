// dear py: Renderer Backend for DirectX9
// This needs to be used along with a Platform Backend (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'LPDIRECT3DTEXTURE9' as ImTextureID. Read the FAQ about ImTextureID!
//  [X] Renderer: Support for large meshes (64k+ vertices) with 16-bit indices.

// You can use unmodified py_impl_* files in your project. See examples/ folder for examples of using this. 
// Prefer including the entire py/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// If you are new to Dear py, read documentation from the docs/ folder + read the top of py.cpp.
// Read online: https://github.com/ocornut/py/tree/master/docs

#pragma once
#include "py.h"      // py_IMPL_API

struct IDirect3DDevice9;

py_IMPL_API bool     py_ImplDX9_Init(IDirect3DDevice9* device);
py_IMPL_API void     py_ImplDX9_Shutdown();
py_IMPL_API void     py_ImplDX9_NewFrame();
py_IMPL_API void     py_ImplDX9_RenderDrawData(ImDrawData* draw_data);

// Use if you want to reset your rendering device without losing Dear py state.
py_IMPL_API bool     py_ImplDX9_CreateDeviceObjects();
py_IMPL_API void     py_ImplDX9_InvalidateDeviceObjects();
