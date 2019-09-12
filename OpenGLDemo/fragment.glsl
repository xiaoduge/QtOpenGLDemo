#version 430 core
#ifdef GL_ES
// 将默认精度设置为中等
precision mediump int;
precision mediump float;
#endif

in vec2 textureCoord;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main()
{
    gl_FragColor = mix(texture2D(ourTexture1, textureCoord), texture2D(ourTexture2, textureCoord), 0.2);
}
