#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec4 aColor; // the color variable has attribute position 1
layout (location = 2) in vec3 vertexNormal; // Attributo Normale 2

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
uniform PointLight lights[4];

//Struttura per la gestione di un materiale
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

out vec4 ourColor; // output a color to the fragment shader
out vec3 N,V,L,R;
out vec2 frag_coord_st;

void main() {
    // ourColor = aColor;
    // frag_coord_st = coord_st;

    if (sceltaVs == 0) {
           gl_Position = Projection*View*Model*vec4(aPos, 1.0);
           ourColor = aColor;
    }
    else if (sceltaVs == 1) {
        gl_Position = Projection * View * Model * vec4(aPos, 1.0);
        vec3 ambient = vec3(0);
        vec3 diffuse = vec3(0);
        vec3 specular = vec3(0);
        //Trasformare le coordinate del vertice da elaborare (aPos) in coordinate di vista
        vec4 eyePosition= View*Model*vec4(aPos,1.0);
        for(int i = 0; i < 4; i++) {
            //Trasformia la posizione della luce nelle coordinate di vista
            vec4 eyeLightPos= View * vec4(lights[i].position, 1.0);
            //trasformare le normali nel vertice in esame nel sistema di coordinate di vista
            N = normalize(transpose(inverse(mat3(View*Model)))*vertexNormal);
        
            //Calcoliamo la direzione della luce L, la direzione riflessione R e di vista
            V = normalize(viewPos - eyePosition.xyz);
            L = normalize((eyeLightPos - eyePosition).xyz);
            R = reflect(-L, N);  //Costruisce la direzione riflessa di L rispesso alla normale

            //ambientale
            ambient += lights[i].power * material.ambient;

            //diffuse
            float coseno_angolo_theta = max(dot(L, N), 0);
            diffuse += lights[i].power * lights[i].color * coseno_angolo_theta * material.diffuse;

            //speculare
            float coseno_angolo_alfa =  pow(max(dot(V, R), 0), material.shininess);
            specular +=  lights[i].power * lights[i].color * coseno_angolo_alfa * material.specular;
        }
        ourColor = vec4(ambient + diffuse + specular, 1.0); 
        //ourColor = aColor;
    }
    else if (sceltaVs == 2) {
        gl_Position = Projection * View * Model * vec4(aPos, 1.0);

        //Trasformare le coordinate del vertice da elaborare (aPos) in coordinate di vista
        vec4 eyePosition= View*Model*vec4(aPos, 1.0);
        //Trasformia la posizione della luce nelle coordinate di vista
        vec4 eyeLightPos = View * vec4(light.position, 1.0);
        //trasformare le normali nel vertice in esame nel sistema di coordinate di vista
        N = normalize(transpose(inverse(mat3(View * Model))) * vertexNormal);
        //Calcoliamo la direzione della luce L, la direzione riflessione R e di vista

        V = normalize(viewPos - eyePosition.xyz);
        L = normalize((eyeLightPos - eyePosition).xyz);
        R = reflect(-L, N);  //Costruisce la direzione riflessa di L rispesso alla normale

    }
    else if (sceltaVs == 3) {
        vec4 v = vec4(aPos, 1);
        float offset =  0.3 * sin(0.5 * time + 5.0 * v.x);
        v.z = v.z + offset;

        gl_Position = Projection*View*Model*v;

        //gl_Position = Projection * View * Model * vec4(aPos, 1.0);

        //Trasformare le coordinate del vertice da elaborare (aPos) in coordinate di vista
        vec4 eyePosition= View*Model*vec4(aPos,1.0);
        //Trasformia la posizione della luce nelle coordinate di vista
        vec4 eyeLightPos= View * vec4(light.position, 1.0);
        //trasformare le normali nel vertice in esame nel sistema di coordinate di vista
        N = normalize(transpose(inverse(mat3(View*Model)))*vertexNormal);
        
        //Calcoliamo la direzione della luce L, la direzione riflessione R e di vista
        V = normalize(viewPos - eyePosition.xyz);
        L = normalize((eyeLightPos - eyePosition).xyz);
        R = reflect(-L, N);  //Costruisce la direzione riflessa di L rispesso alla normale

        //ambientale
        vec3 ambient = light.power * material.ambient;

        //diffuse
        float coseno_angolo_theta = max(dot(L, N), 0);
        vec3 diffuse = light.power * light.color * coseno_angolo_theta * material.diffuse;

        //speculare
        float coseno_angolo_alfa =  pow(max(dot(V, R), 0), material.shininess);
        vec3 specular =  light.power * light.color * coseno_angolo_alfa * material.specular;
        ourColor = vec4(ambient + diffuse + specular, 1.0);   

    }
    else if (sceltaVs == 4) {
        vec4 v=vec4(aPos,1);
        float offset =  0.3*sin(0.5*time+5.0*v.x);
        v.z=v.z+offset;
        gl_Position = Projection*View*Model*v;
        ourColor = aColor; // set ourColor to the input color we got from the vertex data
    }     
    
   
}  

