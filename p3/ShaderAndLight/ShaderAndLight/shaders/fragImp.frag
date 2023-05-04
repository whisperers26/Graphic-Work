#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 viewPos; 
uniform vec3 ambientColor1;
uniform vec3 diffuseColor1;
uniform vec3 specularColor1;
uniform vec3 ambientColor2;
uniform vec3 diffuseColor2;
uniform vec3 specularColor2;
uniform vec3 objectColor;

void main()
{
    // ambient
    vec3 ambient = (ambientColor1 + ambientColor2)/2.0f;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir1 = normalize(lightPos1 - FragPos);
    float diff1 = max(dot(norm, lightDir1), 0.0);
    vec3 diffuse1 = diff1 * diffuseColor1;

    vec3 lightDir2 = normalize(lightPos2 - FragPos);
    float diff2 = max(dot(norm, lightDir2), 0.0);
    vec3 diffuse2 = diff2 * diffuseColor2;
    
    vec3 diffuse = (diffuse1 + diffuse2)/2.0f;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir1 = reflect(-lightDir1, norm);  
    float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0), 20);
    vec3 specular1 = spec1 * specularColor1;  

    vec3 reflectDir2 = reflect(-lightDir2, norm);  
    float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), 20);
    vec3 specular2 = spec2 * specularColor2;  

    vec3 specular = (specular1 + specular2)/2.0f;
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 