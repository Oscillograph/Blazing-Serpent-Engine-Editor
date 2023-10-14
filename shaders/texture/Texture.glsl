// This is a texture plus flat color shader combination

#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TextureCoordinates;
layout(location = 3) in float a_TextureIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityID;

layout (std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
} u_Camera;

struct VertexOutput
{
	vec4 Color;
	vec2 TextureCoordinates;
	float TextureIndex;
	float TilingFactor;
};

layout (location = 0) out VertexOutput Output;
layout (location = 4) out int v_EntityID;


void main(){
	
	Output.Color = a_Color;
	Output.TextureCoordinates = a_TextureCoordinates;
	Output.TextureIndex = a_TextureIndex;
	Output.TilingFactor = a_TilingFactor;
	v_EntityID = a_EntityID;

	gl_Position = u_Camera * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
layout(location = 0) out vec4 color;
layout(location = 1) out int entity;

struct VertexOutput
{
	vec4 Color;
	vec2 TextureCoordinates;
	float TextureIndex;
	float TilingFactor;
};

layout (location = 0) in VertexOutput Input;
layout (location = 4) in int v_EntityID;

layout (binding = 0) uniform sampler2D u_Textures[32];

void main(){
	color =  texture(u_Textures[int(Input.TextureIndex)], Input.TextureCoordinates * Input.TilingFactor) * Input.Color;
	entity = v_EntityID;
}
