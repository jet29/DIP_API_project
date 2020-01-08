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
	ivec2 pivot    = ivec2(kWidth/2,kHeight/2);
	vec3 r_gx      = vec3(0.0f);
	vec3 r_gy      = vec3(0.0f);
	// Kernel application
	for (int i=0 ;i<kWidth;i++){
		for (int j=0 ; j<kHeight;j++){
			r_gx += texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).xyz * texelFetch(gx, i * 7 + j,0).r;
			r_gy += texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).xyz * texelFetch(gy, i * 7 + j,0).r;
		}
	}
	vec3 roberts =  sqrt(r_gx*r_gx + r_gy*r_gy);
	roberts = vec3(dot(roberts, vec3(0.2989,0.5870,0.1140)));
	color = vec4(roberts,1.0f);
}