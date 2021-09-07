#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float aDisplacement;
layout (location = 4) in mat4 instanceMatrix;

out vec2 TexCoords;
out float alpha;

uniform float time;
uniform mat4 projection;
uniform mat4 view;
uniform float angle;
uniform float grassAmount;

mat4 rotationX( in float angle ) {
	return mat4(	1.0,		0,			0,			0,
			 		0, 	cos(angle),	-sin(angle),		0,
					0, 	sin(angle),	 cos(angle),		0,
					0, 			0,			  0, 		1);
}

mat4 rotationY( in float angle ) {
	return mat4(	cos(angle),		0,		sin(angle),	0,
			 				0,		1.0,			 0,	0,
					-sin(angle),	0,		cos(angle),	0,
							0, 		0,				0,	1);
}

mat4 rotationZ( in float angle ) {
	return mat4(	cos(angle),		-sin(angle),	0,	0,
			 		sin(angle),		cos(angle),		0,	0,
							0,				0,		1,	0,
							0,				0,		0,	1);
}

void main()
{
	//float angle = 0 + (sin(time/50));
    //mat4 modelMatrix = instanceMatrix*rotationX(angle);
	alpha = aDisplacement/100000;
	mat4 modelMatrix = instanceMatrix*rotationX(angle);
    TexCoords = aTexCoords;
    gl_Position =  projection * view * modelMatrix *vec4(aPos,1.0);
}