#version 330 core

uniform vec4 u_ballColor;

in vec2 v_texCoord;

out vec4 FragColor;

void main()
{
    float dist = distance(v_texCoord, vec2(0.0));
    float delta = fwidth(dist);
    float alpha = 1.0 - smoothstep(1-delta, 1, dist);
    FragColor = alpha * u_ballColor;
}