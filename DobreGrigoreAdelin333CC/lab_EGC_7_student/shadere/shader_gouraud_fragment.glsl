#version 330
layout(location = 0) out vec4 out_color;

in float light[3];

void main(){

	out_color = vec4(light[0], light[1], light[2], 1);
}