#version 330 core
// Vertex color (interpolated/fragment)
in vec3 vColor;
// Vertex texture position (interpolated/fragment)
in vec2 vTexPos;
// texture
uniform sampler2D image;

// Fragment Color
out vec4 color;

void main(){
	vec3 texColor  = texture(image,vTexPos).xyz;
	ivec2 texSize  = textureSize(image, 0);
	ivec2 texIndex = ivec2(vTexPos.xy * vec2(texSize.xy));
	vec3 sobel    = vec3(0.0f);
	mat3 sobelMat = mat3(-1,-2,-1,0,0,0,1,2,1);
	for (int i=-1 ;i<=1;i++){
		for (int j=-1 ; j<=1;j++){
			sobel += texelFetch(image, texIndex + ivec2(i, j), 0).xyz * sobelMat[i+1][j+1];
		}
	}
	color = vec4(sobel,1.0f);
}