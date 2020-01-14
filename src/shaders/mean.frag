#version 330 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture
uniform sampler2D image;
uniform sampler1D kernel;
uniform int kWidth;
uniform int kHeight;

// Fragment Color
out vec4 color;

void main(){
	vec3 texColor  = texture(image,vTexPos).xyz;
	ivec2 texSize  = textureSize(image, 0);
	ivec2 texIndex = ivec2(vTexPos.xy * vec2(texSize.xy));
	ivec2 pivot = ivec2(kHeight/2,kWidth/2);
	vec3 mean      = vec3(0.0f);
	// Kernel application
	for (int i=0 ;i<kHeight;i++){
		for (int j=0 ; j<kWidth;j++){
			mean += texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).xyz * texelFetch(kernel, i * kWidth + j,0).r;
		}
	}
	mean /= kWidth*kHeight;
	color = vec4(mean,1.0f);
}