#version 330

layout(location = 0) in vec3 in_position;		
layout(location = 1) in vec3 in_normal;	

uniform mat4 model_matrix, view_matrix, projection_matrix;
uniform vec3 light_position;
uniform vec3 eye_position;
uniform int material_shininess;
uniform float material_kd;
uniform float material_ks;

out float light[3];

uniform vec3 color;
uniform float amplitudine[3];
uniform float frecventa[3];
uniform float faza[3];
uniform int timp;
uniform int flag;

vec3 out_pos;
uniform vec2 centru;

float valSimplu(float a, vec2 d, float f, int t, float fz){
	float y;
	y = a * sin( f * dot ( d, vec2(in_position.x, in_position.z) ) + t * fz);
	return y;
}
float valCircular(float a, float f, int t, float fz){
	vec2 d =  centru - vec2(in_position.x , in_position.z);
	if(d != vec2(0))
		d = normalize(d);
	float y = a * sin ( f * dot ( d, vec2(in_position.x, in_position.z) ) + t * fz);
	return y;
}
vec3 getPos(vec3 pos){
	pos.y = valSimplu(amplitudine[0], vec2(1, 0), frecventa[0], timp, faza[0]);
	pos.y += valSimplu(amplitudine[1], vec2(0.5, 0.5), frecventa[1], timp, faza[1]);
	pos.y += valCircular(amplitudine[2], frecventa[2], timp, faza[2]);
	return pos;
}

void main(){

	vec3 world_pos, world_normal;
	vec3 L, V, N, R, H;
	float D, S, light_distance;
	float factor;					
	int primesteLumina = 0;

	out_pos = getPos(in_position);
	out_pos.x = in_position.x;
	out_pos.z = in_position.z;

	if(flag == 0)
		gl_Position = projection_matrix * view_matrix * model_matrix * vec4(in_position, 1);
	else
		gl_Position = projection_matrix * view_matrix * model_matrix * vec4(out_pos, 1);

	vec3 left_vertex_pos = getPos(vec3(in_position.x - 1, in_position.y, in_position.z));
	vec3 right_vertex_pos = getPos(vec3(in_position.x + 1, in_position.y, in_position.z));
	vec3 up_vertex_pos = getPos(vec3(in_position.x, in_position.y, in_position.z + 1));
	vec3 down_vertex_pos = getPos(vec3(in_position.x, in_position.y, in_position.z - 1));

	vec3 left_up_normal = cross(left_vertex_pos - in_position, up_vertex_pos - in_position);
	vec3 right_up_normal = cross( up_vertex_pos - in_position, right_vertex_pos - in_position);
	vec3 left_down_normal = cross(down_vertex_pos - in_position, left_vertex_pos - in_position);
	vec3 right_down_normal = cross(right_vertex_pos - in_position, down_vertex_pos - in_position);
	
	N = (left_up_normal + right_up_normal + left_down_normal + right_down_normal)/4;

	world_pos = (model_matrix * vec4(out_pos, 1)).xyz;
	world_normal = normalize(mat3(model_matrix) * in_normal);

	L = normalize(light_position - world_pos);
	V = normalize(eye_position - world_pos);
	H = normalize(L+V);
	
	R = 2 * dot(world_normal, L) * N - L;

	D = material_kd * 1 * max(dot(N, L), 0);

	if(dot(N, L) > 0)
		primesteLumina = 1;
	
	S = material_ks * 1 * primesteLumina * pow(max(dot(N,H), 0), material_shininess);
//	light_distance = distance(light_position, world_pos);
//	factor = 1 / pow(light_distance, 2);
	light[0] = color.x + D + S;
	light[1] = color.y + D + S;
	light[2] = color.z + D + S;

}
