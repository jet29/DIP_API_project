#version 330 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture
uniform sampler2D image;
uniform int kWidth;
uniform int kHeight;

// Fragment Color
out vec4 color;

void main(){
	ivec2 texSize  = textureSize(image, 0);
	ivec2 texIndex = ivec2(vTexPos.xy * vec2(texSize.xy));
	ivec2 pivot = ivec2(kHeight/2,kWidth/2);
	vec3 c_array[49];
	
    int k = 0;
	// Kernel application
	for (int i=0 ;i<kHeight;i++){
		for (int j=0 ; j<kWidth;j++){
		   c_array[k] = texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).rgb;
           k++;
		}
	}
	// Sorting
	vec3 temp;
	for(int i = 0; i < kWidth*kHeight; i++){
		for(int j = 0 ; j < kWidth * kHeight; j++){
			if(c_array[i].r > c_array[j].r){
				temp = c_array[j];
				c_array[i] = c_array[j];
				c_array[j] = temp;
			}
		}
	}
		
	color = vec4(c_array[kWidth*kHeight/2],1.0f);
}
