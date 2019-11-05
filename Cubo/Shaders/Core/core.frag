#version 410 core

in vec3 ourPos;
in vec2 TexCoord;
in vec3 ourNormal;
in vec3 eye_position, eye_normal;
uniform bool selecionado;
out vec4 FragColor;
uniform mat4 ViewMatrix;
uniform sampler2D texture1;
uniform vec3 Ka, Kd, Ks;
uniform float Ns;
vec3 light_position_world  = vec3 (0.0f, 11.5, 20.0);
vec3 La = vec3 (0.2, 0.2, 0.2);
vec3 Ld = vec3 (0.8, 0.8, 0.8);
vec3 Ls = vec3 (1.0, 1.0, 1.0);
void main()	{
	vec3 Ia = La * Ka;
	vec3 n_eye = normalize(eye_normal);
	vec3 light_position_eye = vec3 (ViewMatrix * vec4 (light_position_world, 1.0));
	vec3 distance_to_light_eye = light_position_eye - eye_position;
	vec3 direction_to_light_eye = normalize (distance_to_light_eye);
	float dot_prod = dot (direction_to_light_eye, n_eye);
	dot_prod = max(dot_prod, 0.0);
	vec3 Id = Ld * Kd * dot_prod;
	vec3 surface_to_viewer_eye = normalize(-eye_position);
	vec3 half_way_eye = normalize (surface_to_viewer_eye + direction_to_light_eye);
	float dot_prod_specular = dot(half_way_eye, n_eye);
	dot_prod_specular = max (dot (half_way_eye, n_eye), 0.0);
	float specular_factor = pow (dot_prod_specular, Ns);
	vec3 Is = Ls * Ks * specular_factor;

	if (selecionado) {
	   FragColor = texture(texture1, TexCoord) * vec4(0.5,0.2,0.5,1.0) * vec4 (Id + Ia + Is, 1.0);
	}else{
	   FragColor = texture(texture1, TexCoord) * vec4(1.0,1.0,1.0,1.0) * vec4 (Id + Ia + Is, 1.0);
	}
}