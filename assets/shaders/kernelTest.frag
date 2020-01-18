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
	vec3 mean = vec3(0.0f);
	for (int i=-10 ;i<=10;i++){
		for (int j=-10 ; j<=10;j++){
			mean += texelFetch(image, texIndex + ivec2(i, j), 0).xyz;
		}
	}
	mean /= 21*21;
	color = vec4(mean,1.0f);
}