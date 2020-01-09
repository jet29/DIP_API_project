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

void bubbleSort(inout vec3 arr[49], int n);

void main(){
	vec3 texColor  = texture(image,vTexPos).xyz;
	ivec2 texSize  = textureSize(image, 0);
	ivec2 texIndex = ivec2(vTexPos.xy * vec2(texSize.xy));
	ivec2 pivot = ivec2(kWidth/2,kHeight/2);
	vec3 c_array[49];
	for(int i=0;i<49;i++){
		c_array[i] = texColor;
	}
	vec3 median      = vec3(0.0f);
    int k = 0;
	// Kernel application
//	for (int i=0 ;i<kWidth;i++){
//		for (int j=0 ; j<kHeight;j++){
////			median += texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).xyz * texelFetch(kernel, i * 7 + j,0).r;
//			c_array[k] = texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).rgb;
//            k++;
//		}
//	}
	//bubbleSort(c_array, kWidth*kHeight);
	color = vec4(c_array[kWidth*kHeight/2],1.0f);
}

void bubbleSort(inout vec3 arr[49], int n){
	bool sorted = false;
	vec3 temp;
	while(!sorted){
		sorted = true;
		for(int i=0;i<n;i++){
			if(arr[i].x<arr[i-1].x){
				arr[i] = arr[i-1];
				arr[i-1] = temp;
				sorted = false;
			}
		}
	}
}