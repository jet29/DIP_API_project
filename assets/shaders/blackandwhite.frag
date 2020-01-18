#version 330 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture
uniform sampler2D image;
uniform float f_threshold;
// Fragment Color
out vec4 color;

void main(){
	vec3 texColor = texture(image,vTexPos).xyz;
	float grayScale = dot(texColor, vec3(0.2989,0.5870,0.1140));
	float blackandwhite = grayScale > f_threshold  ? 1.0f : 0.0f;
	color = vec4(vec3(blackandwhite),1.0f);
}
