#version 330 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture
uniform sampler2D image;
uniform sampler2D median;
uniform sampler2D sobel;


// Fragment Color
out vec4 color;

void main(){
	vec3 medianColor  = texture(median,vTexPos).xyz;
	vec3 sobelColor   = texture(sobel,vTexPos).xyz;
	vec3 finalColor   = sobelColor.r > 0.5f ? vec3(0.0f) : medianColor;
	color = vec4(finalColor,1.0f);
}