#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in float alpha;

uniform sampler2D diffuseTexture;




void main()
{    
    vec4 texColor = texture(diffuseTexture, TexCoords);
    if(texColor.a < 0.1)
        discard;

    vec3 color = texColor.xyz;
    vec3 grassColor = vec3(0.83,0.15,0.02);
    vec3 finalColour = mix(color,grassColor,alpha);
//    vec3 color2 = vec3(displacement,displacement,displacement);
    FragColor =vec4(finalColour,1.0);
}