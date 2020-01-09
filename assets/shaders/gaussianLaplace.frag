#version 330 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture
uniform sampler2D image;
uniform sampler1D gx;
uniform int kWidth;
uniform int kHeight;

// Fragment Color
out vec4 color;

void main(){
	vec3 texColor  = texture(image,vTexPos).xyz;
	//float grayScale = dot(texColor, vec3(0.2989,0.5870,0.1140));
	ivec2 texSize  = textureSize(image, 0);
	ivec2 texIndex = ivec2(vTexPos.xy * vec2(texSize.xy));
	ivec2 pivot    = ivec2(kWidth/2,kHeight/2);
	vec3 LoG  = vec3(0.0f);
	// Kernel application
	for (int i=0 ;i<kWidth;i++){
		for (int j=0 ; j<kHeight;j++){
			LoG += texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).xyz * texelFetch(gx, i * 7 + j,0).r;
		}
	}
	color = vec4(LoG,1.0f);
}