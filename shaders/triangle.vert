#version 400
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aClr;

out vec3 oClr;

uniform mat4 transform;

void main()
{
    vec4 worldPos = transform * vec4(aPos.xyz, 1.f);
    gl_Position = worldPos;
    oClr = aClr;
}