// ImGui GLUT binding with OpenGL
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// If your context is GL3/GL3 then prefer using the code in opengl3_example.
// You *might* use this code with a GL3/GL4 context but make sure you disable the programmable pipeline by calling "glUseProgram(0)" before ImGui::Render().
// We cannot do that from GL2 code because the function doesn't exist. 

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

IMGUI_API bool        ImGui_ImplGlut_Init(bool install_callbacks);
IMGUI_API void        ImGui_ImplGlut_Shutdown();
IMGUI_API void        ImGui_ImplGlut_NewFrame();

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void        ImGui_ImplGlut_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_ImplGlut_CreateDeviceObjects();

// GLUT callbacks (installed by default if you enable 'install_callbacks' during initialization)
// Provided here if you want to chain callbacks.
// You can also handle inputs yourself and use those as a reference.
IMGUI_API void        ImGui_ImplGlut_MouseButtonCallback(int button, int state, int x, int y);
IMGUI_API void        ImGui_ImplGlut_MotionCallback(int x, int y);
IMGUI_API void        ImGui_ImplGlut_PassiveMotionCallback(int x, int y);
IMGUI_API void        ImGui_ImplGlut_KeyCallback(unsigned char key, int x, int y);
IMGUI_API void        ImGui_ImplGlut_KeyUpCallback(unsigned char key, int x, int y);
