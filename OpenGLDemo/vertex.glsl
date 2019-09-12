#version 430 core
#ifdef GL_ES
// 将默认精度设置为中等
precision mediump int;
precision mediump float;
#endif

layout (location = 0) in vec3 v3Pos;
layout (location = 1) in vec2 v2TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 textureCoord;

void main()
{
    textureCoord = v2TexCoord;
    gl_Position = projection * view * model * vec4(v3Pos, 1.0);
}
  
