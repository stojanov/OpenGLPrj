#version 400

in vec2 uv;

out vec4 FragClr;

uniform float check_size;

void main()
{
    float cx = floor(check_size * uv.x);
    float cy = floor(check_size * uv.y);

    float r = mod(cx + cy, 2.0);
    float c = sign(r);
    
    FragClr = vec4(c, c, c, 1.0);
}