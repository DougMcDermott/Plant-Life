// ImGui GLUT binding with OpenGL
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// If your context is GL3/GL3 then prefer using the code in opengl3_example.
// You *might* use this code with a GL3/GL4 context but make sure you disable the programmable pipeline by calling "glUseProgram(0)" before ImGui::Render().
// We cannot do that from GL2 code because the function doesn't exist. 

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include <imgui.h>
#include "imgui_impl_glut.h"
#include <iostream>

// GLUT
#include <GL/glut.h>

// Data
static double       g_Time = 0.0f;
static bool         g_MousePressed[3] = { false, false, false };
static float        g_MouseWheel = 0.0f;
static GLuint       g_FontTexture = 0;

// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
void ImGui_ImplGlut_RenderDrawLists(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // We are using the OpenGL fixed pipeline to make the example code simpler to read!
    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.
    GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box); 
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_TEXTURE_2D);
    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context

    // Setup viewport, orthographic projection matrix
    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Render command lists
    #define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
        const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
        glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)((char*)vtx_buffer + OFFSETOF(ImDrawVert, pos)));
        glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)((char*)vtx_buffer + OFFSETOF(ImDrawVert, uv)));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)((char*)vtx_buffer + OFFSETOF(ImDrawVert, col)));

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
            }
            idx_buffer += pcmd->ElemCount;
        }
    }
    #undef OFFSETOF

    // Restore modified state
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindTexture(GL_TEXTURE_2D, (GLuint)last_texture);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

void ImGui_ImplGlut_MouseButtonCallback(int button, int state, int x, int y)
{
    if (button >= 0 && button < 3) {
        if(state == GLUT_DOWN) {
            g_MousePressed[button] = true;
        } else if (state== GLUT_UP) {
            g_MousePressed[button] = false;
        }
    }
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y); 

    if(button == 4) { 
    g_MouseWheel -= (float)1; // Use fractional mouse wheel, 1.0 unit 5 lines.
    } else if(button == 4) {
    g_MouseWheel += (float)1; // Use fractional mouse wheel, 1.0 unit 5 lines.
    }
}


void ImGui_ImplGlut_KeyCallback(unsigned char key, int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = true;

    int mods = glutGetModifiers();
    io.KeyCtrl = GLUT_ACTIVE_CTRL & mods;
    io.KeyShift = GLUT_ACTIVE_SHIFT & mods;
    io.KeyAlt = GLUT_ACTIVE_ALT & mods;

    io.AddInputCharacter(key);
}

void ImGui_ImplGlut_KeyUpCallback(unsigned char key, int x, int y)
{
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[key] = false;

    int mods = glutGetModifiers();
    io.KeyCtrl = GLUT_ACTIVE_CTRL & mods;
    io.KeyShift = GLUT_ACTIVE_SHIFT & mods;
    io.KeyAlt = GLUT_ACTIVE_ALT & mods;
    //io.AddInputCharacter(key);
}


bool ImGui_ImplGlut_CreateDeviceObjects()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

    // Upload texture to graphics system
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);

    return true;
}

void    ImGui_ImplGlut_InvalidateDeviceObjects()
{
    if (g_FontTexture)
    {
        glDeleteTextures(1, &g_FontTexture);
        ImGui::GetIO().Fonts->TexID = 0;
        g_FontTexture = 0;
    }
}

bool    ImGui_ImplGlut_Init(bool install_callbacks)
{

    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = 9;
    io.KeyMap[ImGuiKey_LeftArrow] = GLUT_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLUT_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLUT_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLUT_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLUT_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLUT_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLUT_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLUT_KEY_END;
    /*
    io.KeyMap[ImGuiKey_Delete] = GLUT_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLUT_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = GLUT_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLUT_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = GLUT_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLUT_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLUT_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLUT_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLUT_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLUT_KEY_Z;
    */
    io.KeyMap[ImGuiKey_Delete] = 127;  // Delete
    io.KeyMap[ImGuiKey_Backspace] = 8;    // Backspace
    io.KeyMap[ImGuiKey_Enter] = 13;   // Enter
    io.KeyMap[ImGuiKey_Escape] = 27;  // Escape
    io.KeyMap[ImGuiKey_A] = 1;   // ctrl-A
    io.KeyMap[ImGuiKey_C] = 3;   // ctrl-C
    io.KeyMap[ImGuiKey_V] = 22;  // ctrl-V
    io.KeyMap[ImGuiKey_X] = 24;  // ctrl-X
    io.KeyMap[ImGuiKey_Y] = 25;  // ctrl-Y
    io.KeyMap[ImGuiKey_Z] = 26;  // ctrl-Z


    io.RenderDrawListsFn = ImGui_ImplGlut_RenderDrawLists;      // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.

    if (install_callbacks)
    {
        glutMouseFunc(ImGui_ImplGlut_MouseButtonCallback);
        glutKeyboardFunc(ImGui_ImplGlut_KeyCallback);
        glutKeyboardUpFunc(ImGui_ImplGlut_KeyUpCallback);
        glutMotionFunc(ImGui_ImplGlut_MotionCallback);
        glutPassiveMotionFunc(ImGui_ImplGlut_PassiveMotionCallback);

    }

    return true;
}


void ImGui_ImplGlut_Shutdown()
{
    ImGui_ImplGlut_InvalidateDeviceObjects();
    ImGui::Shutdown();
}
void ImGui_ImplGlut_MotionCallback(int x, int y) {
    ImGui_ImplGlut_PassiveMotionCallback(x,y);
}
void ImGui_ImplGlut_PassiveMotionCallback(int x, int y) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)x, (float)y);   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
}

void ImGui_ImplGlut_NewFrame()
{
    if (!g_FontTexture)
        ImGui_ImplGlut_CreateDeviceObjects();

    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    w = glutGet(GLUT_WINDOW_WIDTH);
    h = glutGet(GLUT_WINDOW_HEIGHT);
    int display_w = w, display_h = h;
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

    // Setup time step
    double current_time =  glutGet(GLUT_ELAPSED_TIME);
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) / float(1000.0) : (float)(1.0f/60.0f);
    g_Time = current_time;

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())

    for (int i = 0; i < 3; i++)
    {
        io.MouseDown[i] = g_MousePressed[i];    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
    }

    io.MouseWheel = g_MouseWheel;
    g_MouseWheel = 0.0f;

    // Hide OS mouse cursor if ImGui is drawing it
    //glfwSetInputMode(g_Window, GLUT_CURSOR, io.MouseDrawCursor ? GLUT_CURSOR_HIDDEN : GLUT_CURSOR_NORMAL);

    // Start the frame
    ImGui::NewFrame();
}