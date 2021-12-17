#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec4 aColor; // the color variable has attribute position 1
layout (location = 2 ) in vec3 vertexNormal; // Attributo Normale 2

 uniform mat4 Model;
 uniform mat4 Projection;
 uniform mat4 View;
uniform int sceltaVs;
 
uniform int sceltaS;
uniform float time;
uniform vec3 viewPos;

//Struttura per la gestione di un punto luce

struct PointLight{
	vec3 position;
	vec3 color;
	float power;
 };

 //definizione di una varialie uniform che hala struttura PointLight
uniform PointLight light;

//Struttura per la gestione di un materiale

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

out vec4 ourColor; // output a color to the fragment shader

void main()
{
    if (sceltaVs == 0) {
        gl_Position = Projection * View * Model * vec4(aPos, 1.0);
        ourColor = aColor;
    }
           
    
     
     if (sceltaVs==4)
    {
        vec4 v=vec4(aPos,1);
        float offset =  0.3*sin(0.5*time+5.0*v.x);
        v.z=v.z+offset;
        gl_Position = Projection*View*Model*v;
         ourColor = aColor; // set ourColor to the input color we got from the vertex data
    }     
    
   
}  

