#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;
uniform sampler2D background;

uniform float exposure;
uniform bool bloom;

const float gamma = 2.2;

void main()
{
    vec3 hdr  = texture(screenTexture, TexCoords).rgb;
    vec3 bloomCol = texture(bloomBlur, TexCoords).rgb;

    if(bloom)
        hdr += bloomCol;

    // tone map
    vec3 result = vec3(1.0) - exp(-hdr * exposure);
    // gamma correction
    result = pow(result, vec3(1.0 / gamma));
    FragColor.rgb = result;
}