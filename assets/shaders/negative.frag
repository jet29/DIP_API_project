#version 330 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture
uniform sampler2D image;

// Fragment Color
out vec4 color;

void main(){
	vec3 texColor = texture(image,vTexPos).xyz;
	vec3 negColor = vec3(1.0f-texColor.r,1.0f-texColor.g,1.0f-texColor.b);
	color = vec4(negColor,1.0f);
}