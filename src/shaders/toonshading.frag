#version 330 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture
uniform sampler2D image;
uniform sampler1D gx;
uniform sampler1D gy;
uniform int borderRadius;
uniform int medianRadius;

// Fragment Color
out vec4 color;

vec3 sobel();
vec3 median();

void main(){

	vec3 sobel = sobel();
	vec3 median = median();


	color = sobel.r > 0.5f ? vec4(sobel,1.0f) : vec4(median,1.0f);

}

vec3 sobel(){
	vec3 texColor  = texture(image,vTexPos).xyz;
	ivec2 texSize  = textureSize(image, 0);
	ivec2 texIndex = ivec2(vTexPos.xy * vec2(texSize.xy));
	ivec2 pivot    = ivec2(borderRadius/2,borderRadius/2);
	vec3 s_gx      = vec3(0.0f);
	vec3 s_gy      = vec3(0.0f);
	// Sobel Kernel application
	for (int i=0 ;i<borderRadius;i++){
		for (int j=0 ; j<borderRadius;j++){
			s_gx += texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).xyz * texelFetch(gx, i * borderRadius + j,0).r;
			s_gy += texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).xyz * texelFetch(gy, i * borderRadius + j,0).r;
		}
	}
	vec3 sobel =  sqrt(s_gx*s_gx + s_gy*s_gy);
	return vec3(dot(sobel, vec3(0.2989,0.5870,0.1140)));
}

vec3 median(){
	vec3 texColor  = texture(image,vTexPos).xyz;
	ivec2 texSize  = textureSize(image, 0);
	ivec2 texIndex = ivec2(vTexPos.xy * vec2(texSize.xy));
	ivec2 pivot = ivec2(medianRadius/2,medianRadius/2);
	vec3 c_array[49];
	int k = 0;

	// Median Kernel application
	for (int i=0 ;i<medianRadius;i++){
		for (int j=0 ; j<medianRadius;j++){
			c_array[k] = texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).rgb;
			k++;
		}
	}

	// Sorting
	vec3 temp;
	for(int i = 0; i < medianRadius*medianRadius; i++){
		for(int j = 0 ; j < medianRadius*medianRadius; j++){
			if(c_array[i].r > c_array[j].r){
				temp = c_array[j];
				c_array[i] = c_array[j];
				c_array[j] = temp;
			}
		}
	}

	return c_array[medianRadius*medianRadius/2];
}