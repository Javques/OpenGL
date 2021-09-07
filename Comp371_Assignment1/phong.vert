//totally based on https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/shadow_mapping.cpp

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform bool reverse_normals;

//Instancing
uniform bool isGrass;
uniform vec3 offsets[100];


void main()
{
    
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    if(reverse_normals) // a slight hack to make sure the outer large cube displays lighting from the 'inside' instead of the default 'outside'.
        vs_out.Normal = transpose(inverse(mat3(model))) * (-1.0 * aNormal);
    else
        vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    if(isGrass)
    {
        vec3 offset = offsets[gl_InstanceID];
        vec3 pos = aPos + offset;
        gl_Position = projection * view * model * vec4(pos, 1.0);
    }
    else
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
}