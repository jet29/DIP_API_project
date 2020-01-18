#version 330 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture
uniform sampler2D image;
uniform sampler1D gx;
uniform sampler1D gy;
uniform int kWidth;
uniform int kHeight;

// Fragment Color
out vec4 color;

void main(){
	vec3 texColor  = texture(image,vTexPos).xyz;
	//float grayScale = dot(texColor, vec3(0.2989,0.5870,0.1140));
	ivec2 texSize  = textureSize(image, 0);
	ivec2 texIndex = ivec2(vTexPos.xy * vec2(texSize.xy));
	ivec2 pivot    = ivec2(kHeight/2,kWidth/2);
	vec3 p_gx      = vec3(0.0f);
	vec3 p_gy      = vec3(0.0f);
	// Kernel application
	for (int i=0 ;i<kHeight;i++){
		for (int j=0 ; j<kWidth;j++){
			p_gx += texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).xyz * texelFetch(gx, i * kWidth + j,0).r;
			p_gy += texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).xyz * texelFetch(gy, i * kWidth + j,0).r;
		}
	}
	vec3 prewitt =  sqrt(p_gx*p_gx + p_gy*p_gy);
	prewitt = vec3(dot(prewitt, vec3(0.2989,0.5870,0.1140)));
	color = vec4(prewitt,1.0f);
}