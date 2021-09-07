//totally based on https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/shadow_mapping.cpp

// WE NEED HDR, BLOOM AND POST PROCESSING FX

#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor; 

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform bool isModel;
uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;
uniform float shininess;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float matspecular;
uniform float far_plane;
uniform bool shadows;
uniform bool isTexture;
uniform bool isWindow;
uniform bool isGrass;
uniform float time;

uniform vec3 specularVec;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

void main()
{     
    if(!isWindow && !isGrass)
    {
       float border_width = 0.05f;
       float maxX = 1.0 - border_width;
       float minX = border_width;
       float maxY = maxX;
       float minY = minX;
       vec3 color = isTexture? texture(diffuseTexture, fs_in.TexCoords).rgb : vec3(0,1,0);

       if(isModel){
            if (fs_in.TexCoords.x < maxX && fs_in.TexCoords.x > minX &&fs_in.TexCoords.y < maxY && fs_in.TexCoords.y > minY) {
            color = color;
            } 
           else {
           if (fs_in.TexCoords.x < 1 + cos(time)*0.93 && fs_in.TexCoords.x > 0 + cos(time)*0.93)
                if (fs_in.TexCoords.y < 1 + sin(time)*0.93 && fs_in.TexCoords.y > 0 + sin(time)*0.93)
                    color =vec3(0,1,0);
            }
       }
        vec3 normal = normalize(fs_in.Normal);
        vec3 lightColor = vec3(0.3);
        // ambient
        vec3 ambient = 0.3 * color;
        // diffuse
        vec3 lightDir = normalize(lightPos - fs_in.FragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = diff * lightColor;

        // specular
        vec3 viewDir = normalize(viewPos - fs_in.FragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = 0;
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
        vec3 specular = lightColor * spec * vec3(1, 1, 1);
    
        // calculate shadow
        float shadow = shadows ? ShadowCalculation(fs_in.FragPos) : 0.0;                      
        vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    

        // render to bloom buffer?
        float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));

        if(brightness > 1.0)
            BrightColor = vec4(result, 1.0);
        else
            BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
        FragColor = vec4(result, 1.0);
     }
     else if(isWindow){
        FragColor = texture(diffuseTexture, fs_in.TexCoords);
     }
     else if (isGrass){
        vec4 texColor = texture(diffuseTexture, fs_in.TexCoords);
        if(texColor.a < 0.1)
            discard;
        FragColor = texColor;
     }
        
}