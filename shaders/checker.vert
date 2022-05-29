#version 400
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec3 oClr;
out vec2 uv;

void main()
{
    gl_Position = vec4(aPos.xyz, 1.f);
    uv = aTex;
}