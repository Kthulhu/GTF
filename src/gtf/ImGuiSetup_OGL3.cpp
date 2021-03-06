//
//  ImGuiSetup_OGL3.c
//  TexComposer
//
//  Created by David Gallardo on 17/05/16.
//
//

#include <stdio.h>
//#import <OpenGL/gl.h>
//#import <OpenGL/glu.h>
#include <imgui.h>
#include <glad.h>
#include "ImGuiSetup.h"

namespace gtf 
{
static GLuint       g_FontTexture = 0;
//static int          g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
//static int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
//static int          g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
//static unsigned int g_VboHandle = 0, g_VaoHandle = 0, g_ElementsHandle = 0;



void ImGuiGL3_CreateFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();
    
    
    // Create OpenGL texture
    if(g_FontTexture == 0)
    {
        // Build texture atlas
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.
        
        glGenTextures(1, &g_FontTexture);
        glBindTexture(GL_TEXTURE_2D, g_FontTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    }
    
    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;
    
    // Cleanup (don't clear the input data if you want to append new fonts later)
    io.Fonts->ClearInputData();
    io.Fonts->ClearTexData();

	glFinish();
}

bool ImGuiGL3_CreateLocalDeviceObjects(ImGuiLocalGLProperties & prop)
{
    // Backup GL state
    const GLchar *vertex_shader =
    "#version 330\n"
    "uniform mat4 ProjMtx;\n"
    "in vec2 Position;\n"
    "in vec2 UV;\n"
    "in vec4 Color;\n"
    "out vec2 Frag_UV;\n"
    "out vec4 Frag_Color;\n"
    "void main()\n"
    "{\n"
    "	Frag_UV = UV;\n"
    "	Frag_Color = Color;\n"
    "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
    "}\n";
    
    const GLchar* fragment_shader =
    "#version 330\n"
    "uniform sampler2D Texture;\n"
    "in vec2 Frag_UV;\n"
    "in vec4 Frag_Color;\n"
    "out vec4 Out_Color;\n"
    "void main()\n"
    "{\n"
    "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
    "}\n";
    
	prop._ShaderHandle = glCreateProgram();
	prop._VertHandle = glCreateShader(GL_VERTEX_SHADER);
	prop._FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(prop._VertHandle, 1, &vertex_shader, 0);
    glShaderSource(prop._FragHandle, 1, &fragment_shader, 0);
    glCompileShader(prop._VertHandle);
    glCompileShader(prop._FragHandle);
    glAttachShader(prop._ShaderHandle, prop._VertHandle);
    glAttachShader(prop._ShaderHandle, prop._FragHandle);
    glLinkProgram(prop._ShaderHandle);
    
	prop._AttribLocationTex = glGetUniformLocation(prop._ShaderHandle, "Texture");
	prop._AttribLocationProjMtx = glGetUniformLocation(prop._ShaderHandle, "ProjMtx");
	prop._AttribLocationPosition = glGetAttribLocation(prop._ShaderHandle, "Position");
	prop._AttribLocationUV = glGetAttribLocation(prop._ShaderHandle, "UV");
	prop._AttribLocationColor = glGetAttribLocation(prop._ShaderHandle, "Color");

	GLint last_texture, last_array_buffer, last_vertex_array;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

	glGenBuffers(1, &prop._VboHandle);
	glGenBuffers(1, &prop._ElementsHandle);

	glGenVertexArrays(1, &prop._VaoHandle);
	glBindVertexArray(prop._VaoHandle);
	glBindBuffer(GL_ARRAY_BUFFER, prop._VboHandle);
	glEnableVertexAttribArray(prop._AttribLocationPosition);
	glEnableVertexAttribArray(prop._AttribLocationUV);
	glEnableVertexAttribArray(prop._AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
	glVertexAttribPointer(prop._AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(prop._AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(prop._AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

	// Restore modified GL state
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindVertexArray(last_vertex_array);
	glFinish();

	glFinish();
    return true;
}

void ImGuiGL3_RenderDrawLists(ImDrawData* draw_data, ImGuiLocalGLProperties & prop)
{
    // Backup GL state
    GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
    GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
    GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
    GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
    GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
    
    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);
    
    // Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
    ImGuiIO& io = ImGui::GetIO();
    float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);
    
    // Setup viewport, orthographic projection matrix
    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    const float ortho_projection[4][4] =
    {
        { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        {-1.0f,                  1.0f,                   0.0f, 1.0f },
    };
    glUseProgram(prop._ShaderHandle);
    glUniform1i(prop._AttribLocationTex, 0);
    glUniformMatrix4fv(prop._AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
    glBindVertexArray(prop._VaoHandle);
    
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;
        
        glBindBuffer(GL_ARRAY_BUFFER, prop._VboHandle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prop._ElementsHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);
        
        for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
        {
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }
    
    // Restore modified GL state
    glUseProgram(last_program);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    glBindVertexArray(last_vertex_array);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFunc(last_blend_src, last_blend_dst);
    if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	glFinish();
}

void ImGuiGL3_InvalidateLocalDeviceObjects(ImGuiLocalGLProperties & prop)
{
	if (prop._VaoHandle) glDeleteVertexArrays(1, &prop._VaoHandle);
	if (prop._VboHandle) glDeleteBuffers(1, &prop._VboHandle);
	if (prop._ElementsHandle) glDeleteBuffers(1, &prop._ElementsHandle);
	prop._VaoHandle = prop._VboHandle = prop._ElementsHandle = 0;

	glDetachShader(prop._ShaderHandle, prop._VertHandle);
	glDeleteShader(prop._VertHandle);
	prop._VertHandle = 0;

	glDetachShader(prop._ShaderHandle, prop._FragHandle);
	glDeleteShader(prop._FragHandle);
	prop._FragHandle = 0;

	glDeleteProgram(prop._ShaderHandle);
	prop._ShaderHandle = 0;
}

void ImGuiGL3_InvalidateSharedDeviceObjects()
{
    if (g_FontTexture)
    {
        glDeleteTextures(1, &g_FontTexture);
        //ImGui::GetIO().Fonts->TexID = 0;
        g_FontTexture = 0;
    }
	glFinish();
}
} //namespace gtf