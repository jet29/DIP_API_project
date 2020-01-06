#version 330 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture
uniform sampler2D image;
uniform isampler1D kernel;
uniform int kWidth;
uniform int kHeight;

// Fragment Color
out vec4 color;

void main(){
	vec3 texColor  = texture(image,vTexPos).xyz;
	ivec2 texSize  = textureSize(image, 0);
	ivec2 texIndex = ivec2(vTexPos.xy * vec2(texSize.xy));
	vec3 sobel      = vec3(0.0f);
	// Kernel application
	for (int i=0 ;i<kWidth;i++){
		for (int j=0 ; j<kHeight;j++){
			sobel += texelFetch(image, texIndex + ivec2(i, j), 0).xyz * texelFetch(kernel, i * 7 + j,0).r;
		}
	}
	color = vec4(sobel,1.0f);
}