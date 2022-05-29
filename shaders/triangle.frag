#version 400

in vec3 oClr;

out vec4 FragClr;

uniform bool use_color;
uniform vec3 color;

void main()
{
    if (use_color)
    {
        FragClr = vec4(color.xyz, 1.f);
    }
    else
    {
        FragClr = vec4(oClr.xyz, 1.f);
    }
}