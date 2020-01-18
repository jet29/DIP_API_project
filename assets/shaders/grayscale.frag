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
	float grayScale = dot(texColor, vec3(0.2989,0.5870,0.1140));
    color = vec4(vec3(grayScale),1.0f);
}