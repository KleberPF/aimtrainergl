#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;
uniform vec3 color;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    FragColor = vec4(color, 1.0) * sampled;
}