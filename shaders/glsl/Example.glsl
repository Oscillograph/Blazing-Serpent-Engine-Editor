// This is an example

#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TextureCoordinates;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TextureCoordinates;

void main(){
	v_TextureCoordinates = a_TextureCoordinates;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec2 v_TextureCoordinates;

uniform sampler2D u_Texture;

void main(){
	// color = vec4(v_TextureCoordinates, 0.0, 1.0);
	color = texture(u_Texture, v_TextureCoordinates);
}