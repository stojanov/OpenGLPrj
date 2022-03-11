#version 400
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aClr;

out vec3 oClr;

void main()
{
    gl_Position = vec4(aPos.xyz, 1.f);
    oClr = aClr;
}