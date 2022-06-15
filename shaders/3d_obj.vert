#version 400
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aClr;

out vec3 oClr;

uniform mat4 transform;
uniform mat4 proj;
uniform mat4 view;

void main()
{
    vec4 worldPos = proj * view * transform * vec4(aPos, 1.f);
    gl_Position = worldPos;
    oClr = aClr;
}