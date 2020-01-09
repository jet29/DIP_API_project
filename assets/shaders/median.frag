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

void merge(vec3 arr[49], int l, int m, int r);
void mergeSort(vec3 arr[49], int n);

void main(){
	vec3 texColor  = texture(image,vTexPos).xyz;
	ivec2 texSize  = textureSize(image, 0);
	ivec2 texIndex = ivec2(vTexPos.xy * vec2(texSize.xy));
	ivec2 pivot = ivec2(kWidth/2,kHeight/2);
	vec3 c_array[49];
	vec3 median      = vec3(0.0f);
    int k = 0;
	// Kernel application
	for (int i=0 ;i<kWidth;i++){
		for (int j=0 ; j<kHeight;j++){
//			median += texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).xyz * texelFetch(kernel, i * 7 + j,0).r;
			c_array[k] = texelFetch(image, texIndex + ivec2(i-pivot.x, j-pivot.y), 0).rgb;
            k++;
		}
	}
	mergeSort(c_array, kWidth*kHeight);
	color = vec4(c_array[ kWidth*kHeight/2],1.0f);
}

void mergeSort(vec3 arr[49], int n){
	int curr_size;

	int left_start;
	for(curr_size = 1;curr_size<=n-1;curr_size = 2*curr_size){
		for(left_start=0; left_start<n-1; left_start += 2*curr_size){
			int mid = min(left_start + curr_size - 1, n-1);
			int right_end = min(left_start + 2*curr_size - 1, n-1);

			merge(arr, left_start, mid, right_end);
		}
	}
}

void merge(vec3 arr[49], int l, int m, int r){
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 =  r - m; 
  
    /* create temp arrays */
    vec3 L[49], R[49]; 
  
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1+ j]; 
  
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; 
    j = 0; 
    k = l; 
    while (i < n1 && j < n2) 
    { 
        if (L[i].x <= R[j].x) 
        { 
            arr[k] = L[i]; 
            i++; 
        } 
        else
        { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  
    /* Copy the remaining elements of L[], if there are any */
    while (i < n1) 
    { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    /* Copy the remaining elements of R[], if there are any */
    while (j < n2) 
    { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
}