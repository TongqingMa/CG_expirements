#version 330 core
out vec4 FragColor;

struct Material {
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//in vec3 FragPos;  
//in vec3 Normal;  
//in vec2 TexCoords;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;  

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform bool blinn;

float ShadowCalculation(vec4 fragPosLightSpace)
{
   // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // 取得当前片元在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片元是否在阴影中
    vec3 norm = fs_in.Normal;
    vec3 lightDir = normalize(light.position - fs_in.FragPos);	
    float bias = max(0.05 * (1.0 - dot(norm, lightDir)), 0.005); 		
   //进行光滑	
    float shadow = 0.0;	
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);	
   for(int x = -1; x <= 1; ++x)
 {		
         for(int y = -1; y <= 1; ++y)		
       {        
        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;         
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        		
        }    	
}	
     shadow /= 9.0;

    return shadow;
}


void main()
{
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);	
    // Ambient
    vec3 ambient = light.ambient * color;
  	
    // Diffuse 
    //vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * lightColor;  
    
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = 0.0;
    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    vec3 specular = light.specular * (spec * lightColor);  
    
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;        

    FragColor = vec4(lighting, 1.0f);
} 