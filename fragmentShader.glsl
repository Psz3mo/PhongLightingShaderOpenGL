#version 460 core

#define num_of_lights 2

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

vec3 objectColor = vec3(1.0, 1.0, 1.0);
int materialShininess = 32;

struct LightPoint{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

uniform LightPoint light[num_of_lights];
uniform vec3 viewPos;

vec3 CalcPointLight(LightPoint light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main(){
	
	vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = vec3(0.0);

	for(int i = 0; i < num_of_lights; i++){
		result += CalcPointLight(light[i], norm, FragPos, viewDir);
	}

	result *= objectColor;

	FragColor = vec4(result, 1.0);
}

vec3 CalcPointLight(LightPoint light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	//ambient
	vec3 ambient = light.ambient;

	//diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff;

	//specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
	vec3 specular = light.specular * spec;

	//attenuation
	float distance    = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));   

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	return result;
}