#version 330 core
out vec4 FragColor;

uniform vec4 u_ballColor;

in vec2 v_texCoord;

void main()
{
    float dist = distance(v_texCoord, vec2(0.5));
    float delta = fwidth(dist);
    float alpha = 1.0 - smoothstep(0.46-delta, 0.46, dist);
    FragColor = alpha * u_ballColor;
}