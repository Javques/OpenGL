//totally based on https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/shadow_mapping.cpp
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
out vec3 VertColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float u_time;

void main()
{
	gl_Position = projection * view * (model * (vec4(0, 0.5, 0, 1) + vec4(aPos, 1.0f)));
	VertColor = aColor;
}