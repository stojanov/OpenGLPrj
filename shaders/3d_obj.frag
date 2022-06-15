#version 400

in vec3 oClr;

out vec4 FragClr;

void main()
{
    FragClr = vec4(oClr, 1.f);
}