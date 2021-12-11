#include <iostream>

#include "ShaderMaker.h"
#include "Lib.h"
#include "geometria.h"
#include "GestioneTesto.h"
#include <ft2build.h>
#include FT_FREETYPE_H


int width = 1024;
int height = 800;
int width_i = 400;
int height_i = 400;

int w_up = width;
int h_up = height;
mat4 Projection_text;
string stringa_asse;
string Operazione;
int idfg, idfi;
  
static unsigned int programId, programId_text, MatrixProj, MatrixProj_txt, MatModel, MatView, loc_time,lsh, leye,lscelta, loc_view_pos;
unsigned int VAO_Text, VBO_Text;
vec3 asse = vec3(0.0, 1.0, 0.0);
 
int selected_obj = -1;
Mesh Cubo, Piano, Piramide, Centri, Sfera;
static vector<Material> materials;
static vector<Shader> shaders;

point_light light;

//Puntatori alle variabili uniformi per l'impostazione dell'illuminazione

LightShaderUniform light_unif = {};


const float sphere_radius = 3.0f;
static bool moving_trackball = 0;
static int last_mouse_pos_Y;
static int last_mouse_pos_X;
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
float cameraSpeed=0.05;
mat4 Projection, Model, View;
enum {
	NAVIGATION,
	CAMERA_MOVING,
	TRASLATING,
	ROTATING,
	SCALING
} OperationMode;
enum {
	X,
	Y,
	Z
} WorkingAxis;

typedef enum {
	WIRE_FRAME,
	FACE_FILL,
	CULLING_ON,
	CULLING_OFF,
}MenuOption;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

//MATERIALI DI BASE

glm::vec3 red_plastic_ambient = { 0.1, 0.0, 0.0 }, red_plastic_diffuse = { 0.6, 0.1, 0.1 }, red_plastic_specular = { 0.7, 0.6, 0.6 }; GLfloat red_plastic_shininess = 32.0f;
glm::vec3 brass_ambient = { 0.1, 0.06, 0.015 }, brass_diffuse = { 0.78, 0.57, 0.11 }, brass_specular = { 0.99, 0.91, 0.81 }; GLfloat brass_shininess = 27.8f;
glm::vec3 emerald_ambient = { 0.0215, 0.04745, 0.0215 }, emerald_diffuse = { 0.07568, 0.61424, 0.07568 }, emerald_specular = { 0.633, 0.727811, 0.633 }; GLfloat emerald_shininess = 78.8f;
glm::vec3 slate_ambient = { 0.02, 0.02, 0.02 }, slate_diffuse = { 0.1, 0.1, 0.1 }, slate_specular{ 0.4, 0.4, 0.4 }; GLfloat slate_shininess = 1.78125f;
glm::vec3 yellow_ambient = { 0.8,	0.8,	0.0 }, yellow_diffuse = { 0.5,0.5,0.4 }, yellow_specular{ 0.9,	0.9	,0.04 }; GLfloat yellow_shininess = 1.78125f;

float angolo = 0.0;

void update(int a)
{
	angolo += 0.1;
	glutTimerFunc(10, update, 0);
	glutSetWindow(idfi);
	glutPostRedisplay();

	glutSetWindow(idfg);
	glutPostRedisplay();

}

void INIT_SHADER(void)

{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_C.glsl";
	char* fragmentShader = (char*)"fragmentShader_C.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);

	//Generazione del program shader per la gestione del testo
	vertexShader = (char*)"VertexShader_Text.glsl";
	fragmentShader = (char*)"FragmentShader_Text.glsl";

	programId_text = ShaderMaker::createProgram(vertexShader, fragmentShader);

}



void crea_VAO_Vector(Mesh* mesh)
{

	glGenVertexArrays(1, &mesh->VAO);
	glBindVertexArray(mesh->VAO);
	//Genero , rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &mesh->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertici.size() * sizeof(vec3), mesh->vertici.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &mesh->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, mesh->colori.size() * sizeof(vec4), mesh->colori.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &mesh->VBO_normali);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_normali);
	glBufferData(GL_ARRAY_BUFFER, mesh->normali.size() * sizeof(vec3), mesh->normali.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO delle NORMALI nel layer 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	//EBO di tipo indici
	glGenBuffers(1, &mesh->EBO_indici);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO_indici);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indici.size() * sizeof(GLuint), mesh->indici.data(), GL_STATIC_DRAW);


}

glm::vec3 getTrackBallPoint(float x, float y)
{
	//La trackball virtuale pu� fornire un'interfaccia utente intuitiva 
	//per la rotazione interattiva del mondo. 
	float zTemp;
	glm::vec3 point;
	//map to [-1;1]
	point.x = (2.0f * x - width) / width;
	point.y = (height - 2.0f * y) / height;

	//Cooordinata z del punto di coordinate (x,y,z) che si muove sulla sfera virtuale con centro (0,0,0) e raggio r=1
	zTemp = 1.0f - pow(point.x, 2.0) - pow(point.y, 2.0);
	point.z = (zTemp > 0.0f) ? sqrt(zTemp) : 0.0;
	return point;
}
void INIT_VAO(void)
{

	Mesh Sfondo, cappello, pon, testa, naso, corpo, braccio_s, braccio_d, occhio_s, occhio_d, Casa, Palo, Telo, bottone, collo, bocca;
	Mesh mano_d, mano_s, piede_s, gamba_s, gambda_d, piede_d;
	//COSTRUZIONE AMBIENTE: STRUTTURA Scena

	//SFONDO

	crea_piano_suddiviso(&Sfondo, vec4(0.2, 0.2, 0.9, 0.5));
	crea_VAO_Vector(&Sfondo);
	Sfondo.nome = "Sfondo";
	Sfondo.Model = mat4(1.0);
	Sfondo.Model = translate(Sfondo.Model, vec3(0.0, 0.0, -100.0));
	
	Sfondo.Model = scale(Sfondo.Model, vec3(1000.0f, 1000.00f, 1.0f));
	Sfondo.Model = rotate(Sfondo.Model, radians(90.0f), vec3(1.0, 0.0, 0.0));
	vec3 cx = Sfondo.Model * vec4(0.0, 0.0, 0.0, 1.0);
	centri.push_back(cx);
	raggi.push_back(0.5);
	Sfondo.sceltaVS = 0;
	Sfondo.material = MaterialType::EMERALD;
	Scena.push_back(Sfondo);
	 
	//TERRENO
	crea_piano_suddiviso(&Piano, vec4(0.9, 0.9, 0.9, 0.5));
	crea_VAO_Vector(&Piano);
	Piano.nome = "Piano Terra";
	Piano.Model = mat4(1.0);
	Piano.Model = translate(Piano.Model, vec3(0.0, -4.5, 0.0));
	
	Piano.Model = scale(Piano.Model, vec3(1000.0f, 1.0f, 1000.0f));
	cx = Piano.Model * vec4(0.0, 0.0, 0.0, 1.0);
	centri.push_back(cx);
	raggi.push_back(0.5);
	Piano.sceltaVS = 0;
	Piano.material = MaterialType::EMERALD;
	Scena.push_back(Piano);

	//casa
	crea_casa(&Casa, vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 1.0, 1.0, 0.9));
	crea_VAO_Vector(&Casa);
	Casa.nome = "Casa";
	Casa.Model = mat4(1.0);
	Casa.Model = translate(Casa.Model, vec3(20.0, 0.0, 0.0));
	Casa.Model = scale(Casa.Model, vec3(2.0f, 2.0f, 2.0f));
	cx = Casa.Model * vec4(-1.5, 3.5, 0.0, 1.0);
	centri.push_back(vec3(cx));
	raggi.push_back(0.5);
	Casa.sceltaVS = 0;
	Casa.material = MaterialType::EMERALD;
	Scena.push_back(Casa);

	//COSTRUZIONE DEL PERSONAGGIO
	 
	//PON PON
	crea_sfera(&pon, vec4(0.0, 1.0, 0.0, 1.0));
	crea_VAO_Vector(&pon);
	pon.Model = mat4(1.0);
	pon.Model = translate(pon.Model, vec3(0.0, 4.8, 0.0));
	pon.Model = scale(pon.Model, vec3(0.4, 0.4, 0.4));
	pon.nome = "Pon Pon";
	cx = pon.Model * vec4(0.0, 0.0, 0.0, 1.0);
	centri.push_back(cx);
	raggi.push_back(0.4);
	pon.sceltaVS = 0;
	pon.material = MaterialType::SLATE;
	Scena.push_back(pon);

	//CAPPELLO
	crea_cono(&cappello, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&cappello);
	cappello.Model = mat4(1.0);
	cappello.Model = translate(cappello.Model, vec3(0.0, 4.6, 0.0));
		cappello.Model = scale(cappello.Model, vec3(1.1,1.4, 1.1));
	cappello.Model = rotate(cappello.Model, radians(180.0f),vec3(1.0, 0.0,0.0));
	cappello.nome = "Cappello";
	cx = cappello.Model * vec4(0.0, 0.0, 0.0, 1.0);
	centri.push_back(cx);
	raggi.push_back(0.5);
	cappello.sceltaVS = 0;
	cappello.material = MaterialType::SLATE;
	Scena.push_back(cappello);



	//Testa Clown
	 crea_sfera(&testa,vec4(1.0,1.0,0.6,1.0));
	 crea_VAO_Vector(&testa);
	 testa.Model = mat4(1.0);
	 testa.Model = translate(testa.Model, vec3(0.0, 2.75, 0.0));
	 testa.Model = scale(testa.Model, vec3(0.75, 0.75, 0.75));
	 testa.nome = "Testa";
	 cx = testa.Model * vec4(0.0, 0.0, 0.0, 1.0);
	 centri.push_back(cx);
	 raggi.push_back(0.5);
	 testa.sceltaVS = 0;
	 testa.material = MaterialType::RED_PLASTIC;
	 Scena.push_back(testa);
	 
	 //OCCHIO SINISTRO
	 crea_sfera(&occhio_s, vec4(1.0, 1.0, 1.0, 1.0));
	 crea_VAO_Vector(&occhio_s);
	 occhio_s.Model = mat4(1.0);
	 occhio_s.Model = translate(occhio_s.Model, vec3(-0.4, 2.75, 0.8));
	 occhio_s.Model = scale(occhio_s.Model, vec3(0.25, 0.2, 0.15));
	 occhio_s.nome = "occhio sinistro";
	 cx = occhio_s.Model * vec4(0.0,0.0, 0.0, 1.0);
	 centri.push_back(cx);
	 occhio_s.sceltaVS = 0;
	 raggi.push_back(0.5);
	 occhio_s.material = MaterialType::SLATE;
	 Scena.push_back(occhio_s);
	  

	 //OCCHIO DESTRO
	 crea_sfera(&occhio_d, vec4(1.0, 1.0, 1.0, 1.0));
	 crea_VAO_Vector(&occhio_d);
	 occhio_d.Model = mat4(1.0);
	 occhio_d.Model = translate(occhio_d.Model, vec3(0.4, 2.75, 0.8));
	 occhio_d.Model = scale(occhio_d.Model, vec3(0.25, 0.2, 0.15));
	 occhio_d.nome = "Occhio destro";
	 cx = occhio_d.Model * vec4(0.0, 0.0, 0.0, 1.0);
	 centri.push_back(cx);
	 raggi.push_back(0.5);
	 occhio_d.sceltaVS = 0;
	 occhio_d.material = MaterialType::SLATE;
	 Scena.push_back(occhio_d);

	 
	 occhio_s.colori.clear();
	 occhio_s.vertici.clear();
	 occhio_s.indici.clear();

	 occhio_d.colori.clear();
	 occhio_d.vertici.clear();
	 occhio_d.indici.clear();

	 //Pupille
	  //OCCHIO SINISTRO
	 crea_sfera(&occhio_s, vec4(0.0, 0.0, 0.0, 1.0));
	 crea_VAO_Vector(&occhio_s);
	 occhio_s.Model = mat4(1.0);
	 occhio_s.Model = translate(occhio_s.Model, vec3(-0.4, 2.75, 0.9));
	 occhio_s.Model = scale(occhio_s.Model, vec3(0.1, 0.15, 0.05));
	 occhio_s.nome = "pupilla sinistra";
	 cx = occhio_s.Model * vec4(0.0, 0.0, 0.0, 1.0);
	 centri.push_back(cx);
	 raggi.push_back(0.5);
	 occhio_s.sceltaVS = 0;
	 occhio_s.material = MaterialType::SLATE;
	 Scena.push_back(occhio_s);

	 //Pupille
	 //OCCHIO DESTRO
	 crea_sfera(&occhio_d, vec4(0.0, 0.0, 0.0, 1.0));
	 crea_VAO_Vector(&occhio_d);
	 occhio_d.Model = mat4(1.0);
	 occhio_d.Model = translate(occhio_d.Model, vec3(0.4, 2.75, 0.9));
	 occhio_d.Model = scale(occhio_d.Model, vec3(0.1, 0.15, 0.05));
	 occhio_d.nome = "Pupilla destra";
	 cx = occhio_d.Model * vec4(0.0, 0.0, 0.0, 1.0);
	 centri.push_back(cx);
	 raggi.push_back(0.5);
	 occhio_d.sceltaVS = 0;
	 occhio_d.material = MaterialType::SLATE;
	 Scena.push_back(occhio_d);



	 //NASO
	 crea_sfera(&naso, vec4(1.0, 0.0, 1.0, 1.0));
	 crea_VAO_Vector(&naso);
	 naso.Model = mat4(1.0);
	 naso.Model = translate(naso.Model, vec3(0.0, 2.6, 0.4));
	 naso.Model = scale(naso.Model, vec3(0.2, 0.2, 0.6));
	 cx = naso.Model * vec4(0.0, 0.0, 0.0, 1.0);
	 naso.nome = "naso";
	 centri.push_back(cx);
	 raggi.push_back(0.5);
	 naso.sceltaVS = 0;
	 naso.material = MaterialType::SLATE;
	 Scena.push_back(naso);


	 //Bocca
	 crea_sfera(&bocca, vec4(1.0, 0.0, 0.0, 1.0));
	 crea_VAO_Vector(&bocca);
	 bocca.Model = mat4(1.0);
	 bocca.Model = translate(bocca.Model, vec3(0.0, 2.2, 0.4));
	 bocca.Model = scale(bocca.Model, vec3(0.4, 0.1, 0.2));
	 bocca.nome = "bocca";
	 cx = bocca.Model * vec4(0.0, 0.0, 0.0, 1.0);
	 centri.push_back(cx);
	 raggi.push_back(0.5);
	 bocca.sceltaVS = 0;
	 bocca.material = MaterialType::SLATE;
	 Scena.push_back(bocca);
	
	 //Collo
	 crea_cilindro(&collo, vec4(1.0, 0.0, 1.0, 1.0));
	 crea_VAO_Vector(&collo);
	 collo.Model = mat4(1.0);
	 collo.Model = translate(collo.Model, vec3(0.0, 1.8, 0.0));
	 collo.Model = scale(collo.Model, vec3(0.2, 0.2, 0.2));
	 collo.nome = "collo";
	 cx = collo.Model * vec4(0.0, 1.0, 0.0, 1.0);
	 centri.push_back(cx);
	 raggi.push_back(0.5);
	 collo.sceltaVS = 0;
	 collo.material = MaterialType::SLATE;
	 Scena.push_back(collo);
	
	 //Corpo
	 crea_sfera(&corpo, vec4(0.0, 1.0, 0.0, 1.0));
	 crea_VAO_Vector(&corpo);
	 corpo.Model = mat4(1.0);
	 corpo.Model = translate(corpo.Model, vec3(0.0, 0.3, 0.0));
	 corpo.Model = scale(corpo.Model, vec3(1.4, 1.6, 1.0));
	 raggi.push_back(0.5);
	 corpo.nome = "Corpo";
	 corpo.material = MaterialType::EMERALD;
	 cx = corpo.Model * vec4(0.0,0.0,0.0, 1.0);
	 centri.push_back(cx);
	 corpo.sceltaVS = 0;
	  
	 Scena.push_back(corpo);
	 raggi.push_back(0.5);
	  
	 //Bottone centrale
	 crea_sfera(&bottone, vec4(1.0, 1.0, 0.0, 1.0));
	 crea_VAO_Vector(&bottone);
	 bottone.Model = mat4(1.0);
	 bottone.Model = translate(bottone.Model, vec3(0.0, -0.2, 0.8));
	 bottone.Model = scale(bottone.Model, vec3(0.2, 0.2, 0.2));
	 raggi.push_back(0.5);
	 bottone.nome = "Bottone centrale";
	 cx = bottone.Model * vec4(0.0, 0.0, 0.0, 1.0);
	 bottone.sceltaVS = 0;
	 centri.push_back(cx);
	 bottone.material = MaterialType::SLATE;
	 Scena.push_back(bottone);
	 raggi.push_back(0.1);

	 bottone.vertici.clear();
	 bottone.colori.clear();
	 bottone.indici.clear();

	 //Bottone superiore
	 crea_sfera(&bottone, vec4(1.0, 1.0, 0.0, 1.0));
	 crea_VAO_Vector(&bottone);
	 bottone.Model = mat4(1.0);
	 bottone.Model = translate(bottone.Model, vec3(0.0, 0.2, 0.8));
	 bottone.Model = scale(bottone.Model, vec3(0.2, 0.2, 0.2));
	 raggi.push_back(0.5);
	 bottone.nome = "Bottone Superiore";
	 cx = bottone.Model * vec4(0.0, 0.0, 0.0, 1.0);
	 centri.push_back(cx);
	 bottone.sceltaVS = 0;
	 bottone.material = MaterialType::SLATE;
	 Scena.push_back(bottone);
	 raggi.push_back(0.1);

	 bottone.vertici.clear();
	 bottone.colori.clear();
	 bottone.indici.clear();

	 //Bottone inferiore
	 crea_sfera(&bottone, vec4(1.0, 1.0, 0.0, 1.0));
	 crea_VAO_Vector(&bottone);
	 bottone.Model = mat4(1.0);
	 bottone.Model = translate(bottone.Model, vec3(0.0, -0.6, 0.8));
	 bottone.Model = scale(bottone.Model, vec3(0.2, 0.2, 0.2));
	 raggi.push_back(0.5);
	 bottone.nome = "Bottone Inferiore";
	 cx = bottone.Model * vec4(0.0, 0.0, 0.0, 1.0);
	 centri.push_back(cx);
	 bottone.sceltaVS = 0;
	 bottone.material = MaterialType::SLATE;
	 Scena.push_back(bottone);
	 raggi.push_back(0.5);
 
	 //Braccio sinistro
	 crea_cubo(&braccio_s, vec4(0.0, 0.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0));
	 crea_VAO_Vector(&braccio_s);
	 braccio_s.Model = mat4(1.0);
	 braccio_s.Model = translate(braccio_s.Model, vec3(-1.6, 0.9, 0.0));
	 braccio_s.Model = rotate(braccio_s.Model, radians(65.0f), vec3(0.0, 0.0, 1.0));
	 braccio_s.Model = scale(braccio_s.Model, vec3(0.5, 0.5, 1.0));
	 braccio_s.nome = "braccio sinistro";
	 cx = braccio_s.Model * vec4(1.0, 1.0, 1.0, 1.0);
	 centri.push_back(cx);
	 raggi.push_back(0.5);
	 braccio_s.sceltaVS = 0;
	 braccio_s.material = MaterialType::SLATE;
	 Scena.push_back(braccio_s);
	  
	 //Mano sinistra
	crea_sfera(&mano_s, vec4(1.0, 0.0, 1.0, 1.0));
	crea_VAO_Vector(&mano_s);
	mano_s.Model = mat4(1.0);
	mano_s.Model = translate(mano_s.Model, vec3(-1.9, 0.15, 0.0));
	mano_s.Model = rotate(mano_s.Model, radians(65.0f), vec3(0.0, 0.0, 1.0));
	 mano_s.Model = scale(mano_s.Model, vec3(0.4, 0.4, 1.0));
	mano_s.nome = "mano sinistra";
	 cx = mano_s.Model * vec4(0.0, 0.0, 0.0, 1.0);
	 
	centri.push_back(cx);
	 
	raggi.push_back(0.5);
	mano_s.sceltaVS = 0;
	mano_s.material = MaterialType::SLATE;
	Scena.push_back(mano_s);
	 
	//Braccio Destro
	crea_cubo(&braccio_d, vec4(0.0, 0.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0));
	crea_VAO_Vector(&braccio_d);
	braccio_d.Model = mat4(1.0);
	braccio_d.Model = translate(braccio_d.Model, vec3(1.6, 0.9, 0.0));
	braccio_d.Model = rotate(braccio_d.Model, radians(-65.0f), vec3(0.0, 0.0, 1.0));
	braccio_d.Model = scale(braccio_d.Model, vec3(0.5, 0.5, 1.0));
	braccio_d.nome = "braccio destro";
	cx = braccio_d.Model * vec4(1.0, 1.0, 1.0, 1.0);

	centri.push_back(cx);
	raggi.push_back(0.5);
	braccio_d.sceltaVS = 0;
	braccio_d.material = MaterialType::SLATE;
	Scena.push_back(braccio_d);
	  
	//Mano destra
	crea_sfera(&mano_d, vec4(1.0, 0.0, 1.0, 1.0));
	crea_VAO_Vector(&mano_d);
	mano_d.Model = mat4(1.0);
	mano_d.Model = translate(mano_d.Model, vec3(1.9, 0.15, 0.0));
	mano_d.Model = rotate(mano_d.Model, radians(-65.0f), vec3(0.0, 0.0, 1.0));
	mano_d.Model = scale(mano_d.Model, vec3(0.4, 0.4, 1.0));
	mano_d.nome = "mano destra";
	cx = mano_d.Model * vec4(0.0,0.0, 0.0, 1.0);
	centri.push_back(cx);
	raggi.push_back(0.5);
	mano_d.sceltaVS = 0;
	mano_d.material = MaterialType::SLATE;
	Scena.push_back(mano_d);
	 

	//Gamba sinistra
	crea_cubo(&gamba_s, vec4(0.0, 0.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0));
	crea_VAO_Vector(&gamba_s);
	gamba_s.Model = mat4(1.0);
	gamba_s.Model = translate(gamba_s.Model, vec3(-0.6, -2.4, -0.1));
	//gamba_s.Model = rotate(gamba_s.Model, radians(15.0f), vec3(0.0, 0.0, 1.0));
	gamba_s.Model = scale(gamba_s.Model, vec3(0.5, 1.2, 1.0));
	gamba_s.nome = "gamba sinistra";
	cx = gamba_s.Model * vec4(1.0, 1.0, 1.0, 1.0);
	centri.push_back(cx);
	raggi.push_back(0.2);
	gamba_s.sceltaVS = 0;
	gamba_s.material = MaterialType::SLATE;
	Scena.push_back(gamba_s);
	 

	//Piede sinsitro
	crea_sfera(&piede_s, vec4(1.0, 0.0, 1.0, 1.0));
	crea_VAO_Vector(&piede_s);
	piede_s.Model = mat4(1.0);

	piede_s.Model = translate(piede_s.Model, vec3(-0.8, -4.0, 0.0));
	piede_s.Model = rotate(piede_s.Model, radians(65.0f), vec3(0.0, 0.0, 1.0));
	piede_s.Model = scale(piede_s.Model, vec3(1.0, 0.4, 0.4));
	piede_s.nome = "piede sinistro";
	cx = piede_s.Model * vec4(0.0, 0.0, 0.0, 1.0);
	centri.push_back(cx);
	raggi.push_back(0.5);
	piede_s.sceltaVS = 0;
	piede_s.material = MaterialType::SLATE;
	Scena.push_back(piede_s);
	 
	//Gamba destra
	crea_cubo(&gambda_d, vec4(0.0, 0.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0));
	crea_VAO_Vector(&gambda_d);
	gambda_d.Model = mat4(1.0);
	gambda_d.Model = translate(gambda_d.Model, vec3(0.6, -2.4, -0.1));
	gambda_d.Model = scale(gambda_d.Model, vec3(0.5, 1.2, 1.0));
	gambda_d.nome = "gamba destra";
	cx = gambda_d.Model * vec4(1.0, 1.0, 1.0, 1.0);
	centri.push_back(cx);
	raggi.push_back(0.5);
	gambda_d.sceltaVS = 0;
	gambda_d.material = MaterialType::SLATE;
	Scena.push_back(gambda_d);
	
	 
	
	//Piede destro
	crea_sfera(&piede_d, vec4(1.0, 0.0, 1.0, 1.0));
	crea_VAO_Vector(&piede_d);
	piede_d.Model = mat4(1.0);
	piede_d.Model = translate(piede_d.Model, vec3(0.8, -4.0, 0.0));
	piede_d.Model = rotate(piede_d.Model, radians(-65.0f), vec3(0.0, 0.0, 1.0));
	piede_d.Model = scale(piede_d.Model, vec3(1.0, 0.4, 0.4));
	piede_d.nome = "piede destro";
	cx = piede_d.Model * vec4(0.0, 0.0, 0.0, 1.0);
	centri.push_back(cx);
	raggi.push_back(0.5);
	piede_d.sceltaVS = 0;
	piede_d.material = MaterialType::SLATE;
	Scena.push_back(piede_d);
   
	//Costruzione Bandiera
	 
	//Palo
	crea_cilindro(&Palo, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Palo);
	Palo.Model = mat4(1.0);
	Palo.Model = translate(Palo.Model, vec3(28.0, -2.5, 0.0));
	Palo.Model = scale(Palo.Model, vec3(0.1f, 10.0f, 0.1f));
	Palo.nome = "Palo";
	
	cx = Palo.Model * vec4(0.0, 1.0, 0.0, 1.0);
	centri.push_back(cx);
	raggi.push_back(0.5);
	Palo.sceltaVS = 0;
	Palo.material = MaterialType::SLATE;
	Scena.push_back(Palo);
 
	//Telo
	crea_piano_suddiviso(&Telo, vec4(0.8, 0.2, 0.9, 1.0));
	crea_VAO_Vector(&Telo);
	Telo.nome = "Telo";
	Telo.Model = mat4(1.0);
	Telo.Model = translate(Telo.Model, vec3(30.0, 6.0, 0.0));
	Telo.Model = scale(Telo.Model, vec3(4.0f, 3.0f, 1.0f));
	Telo.Model = rotate(Telo.Model, radians(90.0f), vec3(1.0, 0.0, 0.0));
	cx = Telo.Model * vec4(0.0, 0.0, 0.0, 1.0);
	 
	centri.push_back(cx);
	raggi.push_back(1.5);
	Telo.sceltaVS = 0;
	Telo.material = MaterialType::SLATE;
	Scena.push_back(Telo);
	 
	 }

void INIT_Illuminazione()
	 {
	light.position = { 0.0,5.0,0.0 };
	light.color = { 1.0,1.0,1.0 };
	light.power = 1.f;

	//Setup dei materiali
	// Materials setup
	materials.resize(6);
	materials[MaterialType::RED_PLASTIC].name = "Red Plastic";
	materials[MaterialType::RED_PLASTIC].ambient = red_plastic_ambient;
	materials[MaterialType::RED_PLASTIC].diffuse = red_plastic_diffuse;
	materials[MaterialType::RED_PLASTIC].specular = red_plastic_specular;
	materials[MaterialType::RED_PLASTIC].shininess = red_plastic_shininess;

	materials[MaterialType::EMERALD].name = "Emerald";
	materials[MaterialType::EMERALD].ambient = emerald_ambient;
	materials[MaterialType::EMERALD].diffuse = emerald_diffuse;
	materials[MaterialType::EMERALD].specular = emerald_specular;
	materials[MaterialType::EMERALD].shininess = emerald_shininess;

	materials[MaterialType::BRASS].name = "Brass";
	materials[MaterialType::BRASS].ambient = brass_ambient;
	materials[MaterialType::BRASS].diffuse = brass_diffuse;
	materials[MaterialType::BRASS].specular = brass_specular;
	materials[MaterialType::BRASS].shininess = brass_shininess;

	materials[MaterialType::SLATE].name = "Slate";
	materials[MaterialType::SLATE].ambient = slate_ambient;
	materials[MaterialType::SLATE].diffuse = slate_diffuse;
	materials[MaterialType::SLATE].specular = slate_specular;
	materials[MaterialType::SLATE].shininess = slate_shininess;

	materials[MaterialType::YELLOW].name = "Yellow";
	materials[MaterialType::YELLOW].ambient = yellow_ambient;
	materials[MaterialType::YELLOW].diffuse = yellow_diffuse;
	materials[MaterialType::YELLOW].specular = yellow_specular;
	materials[MaterialType::YELLOW].shininess = yellow_shininess;


	materials[MaterialType::NO_MATERIAL].name = "NO_MATERIAL";
	materials[MaterialType::NO_MATERIAL].ambient = glm::vec3(1, 1, 1);
	materials[MaterialType::NO_MATERIAL].diffuse = glm::vec3(0, 0, 0);
	materials[MaterialType::NO_MATERIAL].specular = glm::vec3(0, 0, 0);
	materials[MaterialType::NO_MATERIAL].shininess = 1.f;

	//Setup degli shader
	shaders.resize(5);
	shaders[ShaderOption::NONE].value = 0;
	shaders[ShaderOption::NONE].name = "NONE";
	shaders[ShaderOption::GOURAD_SHADING].value = 1;
	shaders[ShaderOption::GOURAD_SHADING].name = "GOURAD SHADING";
	shaders[ShaderOption::PHONG_SHADING].value = 2;
	shaders[ShaderOption::PHONG_SHADING].name = "PHONG SHADING";
	shaders[ShaderOption::ONDE_SHADING].value = 3;
	shaders[ShaderOption::ONDE_SHADING].name = "ONDE SHADING";
	shaders[ShaderOption::BANDIERA_SHADING].value = 4;
	shaders[ShaderOption::BANDIERA_SHADING].name = "BANDIERA SHADING";

}

void INIT_VAO_Text(void)
{

	// configure VAO/VBO for texture quads
	// -----------------------------------
	glGenVertexArrays(1, &VAO_Text);
	glGenBuffers(1, &VBO_Text);
	glBindVertexArray(VAO_Text);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Text);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void moveCameraForward()
{
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.position += ViewSetup.direction * cameraSpeed;

}

void moveCameraBack()
{
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.position -= ViewSetup.direction * cameraSpeed;

}

void moveCameraLeft()
{
	//Calcolo la direzione perpendicolare alla direzione della camera e l'alto della camera
		// e muovo la camera a sinistra lungo questa direzione
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	vec3 direzione_scorrimento = glm::cross(vec3(ViewSetup.direction), glm::vec3(ViewSetup.upVector));
	ViewSetup.position -= vec4((direzione_scorrimento), .0) * cameraSpeed;
	ViewSetup.target -= vec4((direzione_scorrimento), .0) * cameraSpeed;
}

void moveCameraRight()
{
	//Calcolo la direzione perpendicolare alla direzione della camera e l'alto della camera
	// e muovo la camera a destra lungo questa direzione
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	vec3 direzione_scorrimento = glm::cross(vec3(ViewSetup.direction), glm::vec3(ViewSetup.upVector)) * cameraSpeed;
	ViewSetup.position += vec4(direzione_scorrimento, 0);
	ViewSetup.target += vec4(direzione_scorrimento, 0);
}

void modifyModelMatrix(glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor)
{
	//ricordare che mat4(1) costruisce una matrice identit� di ordine 4
	mat4 traslation = glm::translate(glm::mat4(1), translation_vector);
	mat4 scale = glm::scale(glm::mat4(1), glm::vec3(scale_factor, scale_factor, scale_factor));
	mat4 rotation = glm::rotate(glm::mat4(1), angle, rotation_vector);

	//Modifica la matrice di Modellazione dell'oggetto della scena selezionato postmolitplicando per le matrici scale*rotation*traslation

	if (selected_obj > -1)
	{
		Scena[selected_obj].Model = Scena[selected_obj].Model * scale * rotation * traslation;
		centri[selected_obj] = centri[selected_obj] + translation_vector;
		raggi[selected_obj] = raggi[selected_obj] * scale_factor;
	}

}
void keyboardPressedEvent(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		moveCameraLeft();
		break;
	case 'd':
		moveCameraRight();
		break;

	case 'w':
		moveCameraForward();
		break;

	case 's':
		moveCameraBack();
		break;

	case 'g':  //Si entra in modalit� di operazione traslazione
				OperationMode = TRASLATING;
				Operazione = "TRASLAZIONE";
				break;
	case 'r': //Si entra in modalit� di operazione rotazione
				OperationMode = ROTATING;
				Operazione = "ROTAZIONE";
				break;
	case 'S':
				OperationMode = SCALING; //Si entra in modalit� di operazione scalatura
				Operazione = "SCALATURA";
				break;
	case 27:
				glutLeaveMainLoop();
				break;
				// Selezione dell'asse
			case 'x':
				WorkingAxis = X;  //Seleziona l'asse X come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
				stringa_asse = " Asse X";
				break;
			case 'y':
				WorkingAxis = Y;  //Seleziona l'asse X come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
				stringa_asse = " Asse Y";
				break;
			case 'z':
				WorkingAxis = Z;
				stringa_asse = " Asse Z";  //Seleziona l'asse X come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
				break;

			default:
				break;
	}

			// Selezione dell'asse per le trasformazioni
	switch (WorkingAxis) {
			case X:	asse = glm::vec3(1.0, 0.0, 0.0);

				break;
			case Y: asse = glm::vec3(0.0, 1.0, 0.0);

				break;
			case Z: asse = glm::vec3(0.0, 0.0, 1.0);

				break;
			default:
				break;
			}

			glutSetWindow(idfi);
			glutPostRedisplay();

			// I tasti + e -  aggiornano lo spostamento a destra o a sinistra, la rotazione in segno antiorario o in senso orario, la scalatura come amplificazione o diminuizione delle dimensioni

			float amount = .01;
			if (key == '+')
				amount *= 1;

			if (key == '-')
				amount *= -1;


			switch (OperationMode) {

				//la funzione modifyModelMatrix(glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor) 
				// definisce la matrice di modellazione che si vuole postmoltiplicare alla matrice di modellazione dell'oggetto selezionato, per poterlo traslare, ruotare scalare.

			case TRASLATING:
				// si passa angle 0 e scale factor =1, 
				modifyModelMatrix(asse * amount, asse, 0.0f, 1.0f);
				break;
			case ROTATING:
				// SI mette a zero il vettore di traslazione (vec3(0) e ad 1 il fattore di scale
				modifyModelMatrix(glm::vec3(0), asse, amount * 2.0f, 1.0f);
				break;
			case SCALING:
				// SI mette a zero il vettore di traslazione (vec3(0), angolo di rotazione a 0 e ad 1 il fattore di scala 1+amount.
				modifyModelMatrix(glm::vec3(0), asse, 0.0f, 1.0f + amount);
				break;



			}
			glutSetWindow(idfi);
			glutPostRedisplay();

			glutSetWindow(idfg);
			glutPostRedisplay();
		 


 
	
}

	
void INIT_CAMERA_PROJECTION(void)
{
	//Imposto la telecamera
	ViewSetup = {};
	ViewSetup.position = glm::vec4(0.0, 0.5, 25.0, 0.0);
	ViewSetup.target = glm::vec4(0.0, 0.0, 0.0, 0.0);
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.upVector = glm::vec4(0.0, 1.0, 0.0, 0.0);


	//Imposto la proiezione prospettica
	PerspectiveSetup = {};
	PerspectiveSetup.aspect = (GLfloat)width / (GLfloat)height;
	PerspectiveSetup.fovY = 45.0f;
	PerspectiveSetup.far_plane = 2000.0f;
	PerspectiveSetup.near_plane = 0.1f;
}


vec3 get_ray_from_mouse(float mouse_x, float mouse_y) {

	
	// mappiamo le coordinate di viewport del mouse [0,width], [height,0] in coordinate normalizzate [-1,1]  
	float x = (2.0f * mouse_x) / width - 1.0;
	float y = 1.0f - (2.0f * mouse_y) / height;
	float z = 1.0f;
	vec3  ray_nds = vec3(x, y, z);
	// clip space
	vec4 ray_clip = vec4(x, y, -1.0, 1.0);

	// eye space
	
	vec4 ray_eye = inverse(Projection) * ray_clip;


	ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	// world space
	vec3 ray_wor = vec3(inverse(View) * ray_eye);

	ray_wor = normalize(ray_wor);

	return ray_wor;
}

/*controlla se un raggio interseca una sfera. In caso negativo, restituisce false. Rigetta
le intersezioni dietro l'origine del raggio, e pone  intersection_distance all'intersezione p i� vicina.
*/

bool ray_sphere(vec3 ray_origin_wor, vec3 ray_direction_wor, vec3 sphere_centre_wor, float sphere_radius, float* intersection_distance) {
	 
	//Calcoliamo O-C
	vec3 dist_sfera = ray_origin_wor - sphere_centre_wor;
	float b = dot(dist_sfera, ray_direction_wor);
	float f = dot(dist_sfera, dist_sfera) - sphere_radius * sphere_radius;

	float delta = b * b - f;

	if (delta < 0)
		return false;

	if (delta > 0.0f) {
		//calcola le due intersezioni
		float t_a = -b + sqrt(delta);
		float t_b = -b - sqrt(delta);
		*intersection_distance = t_b;

		if (t_a <0.0) {
			if (t_b < 0)
				return false;
		}

		return true;
	}

	if (delta==0) {
		float t = -b + sqrt(delta);
		if (t < 0)
			return false;
		*intersection_distance = t;
		return true;
	}

	return false;
}


void mouse(int button, int state, int x, int y)
{
	 
	switch (button) {
		//Con il tasto sinistro premuto si attiva la modalit� di trackball
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) { moving_trackball = true; }
		if (state == GLUT_UP) { moving_trackball = false; }
		//OperationMode = NAVIGATION;
		last_mouse_pos_X = x;
		last_mouse_pos_Y = y;
		break;

	case GLUT_RIGHT_BUTTON:

		//Con il tasto destro si selezionano gli oggetti nella scena
			if (state == GLUT_DOWN && glutGetModifiers() == GLUT_ACTIVE_CTRL)
			{ 
				float xmouse = x;
				float ymouse = y;
				vec3 ray_wor = get_ray_from_mouse(xmouse, ymouse);

				selected_obj = -1;
				float closest_intersection = 0.0f;
				for (int i = 0; i < Scena.size(); i++)
				{
					float t_dist = 0.0f;

					if (ray_sphere(ViewSetup.position, ray_wor, centri[i], raggi[i], &t_dist))
					{
						if (selected_obj == -1 || t_dist < closest_intersection)
						{
							selected_obj = i;
							closest_intersection = t_dist;
						}
					}
				}
			if (selected_obj>-1)
				printf("Oggetto selezionato %d -> %s \n", selected_obj, Scena[selected_obj].nome.c_str());
			}
		break;
	default:
		break;
	} 
	glutSetWindow(idfi);
	glutPostRedisplay();

	glutSetWindow(idfg);
	glutPostRedisplay();

}
void mouseActiveMotion(int x, int y)
{
	// Spostamento su trackball del vettore posizione Camera 
	if (!moving_trackball) {
		return;
	}
	glm::vec3 destination = getTrackBallPoint(x, y);
	glm::vec3 origin = getTrackBallPoint(last_mouse_pos_X, last_mouse_pos_Y);
	float dx, dy, dz;
	dx = destination.x - origin.x;
	dy = destination.y - origin.y;
	dz = destination.z - origin.z;
	if (dx || dy || dz) {
		// rotation angle = acos( (v dot w) / (len(v) * len(w)) ) o approssimato da ||dest-orig||;
		float pi = glm::pi<float>();
		float angle = sqrt(dx * dx + dy * dy + dz * dz) * (180.0 / pi);
		// rotation axis = (dest vec orig) / (len(dest vec orig))
		glm::vec3 rotation_vec = glm::cross(origin, destination);
		// calcolo del vettore direzione w = C - A
		ViewSetup.direction = -ViewSetup.target + ViewSetup.position;
		// rotazione del vettore direzione w 
		// determinazione della nuova posizione della camera 
		ViewSetup.position = ViewSetup.target + glm::rotate(glm::mat4(1.0f), glm::radians(-angle), rotation_vec) * ViewSetup.direction;

	}
	last_mouse_pos_X = x; last_mouse_pos_Y = y;
	glutSetWindow(idfi);
	glutPostRedisplay();

	glutSetWindow(idfg);
	glutPostRedisplay();

}
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	PerspectiveSetup.aspect = (GLfloat)w / (GLfloat)h;
	Projection = perspective(radians(PerspectiveSetup.fovY), PerspectiveSetup.aspect, PerspectiveSetup.near_plane, PerspectiveSetup.far_plane);
	w_up = w;
	h_up = h;
}

void resize1(int w, int h)
{
	glViewport(0, 0, w, h);
	Projection_text = ortho(0.0f, (float)w, 0.0f, (float)h);

	w_up = w;
	h_up = h;
}

void drawScene(void)
{
	/*
	float timeValue = glutGet(GLUT_ELAPSED_TIME) * 0.001;
	ViewSetup.position.x = sin(timeValue) * 10.0;
	ViewSetup.position.z = cos(timeValue) * 10.0;
	*/
	
	//Passo al Vertex Shader il puntatore alla matrice Projection, che sar� associata alla variabile Uniform mat4 Projection
	//all'interno del Vertex shader. Uso l'identificatio MatrixProj

	float time = glutGet(GLUT_ELAPSED_TIME)/1000.0;

	glUseProgram(programId);

	Projection = perspective(radians(PerspectiveSetup.fovY), PerspectiveSetup.aspect, PerspectiveSetup.near_plane, PerspectiveSetup.far_plane);

	glUniformMatrix4fv(MatrixProj, 1, GL_FALSE, value_ptr(Projection));


	glClearColor(0.0, 0.0, 0.0, 0.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Costruisco la matrice di Vista che applicata ai vertici in coordinate del mondo li trasforma nel sistema di riferimento della camera.
	View = lookAt(vec3(ViewSetup.position), vec3(ViewSetup.target), vec3(ViewSetup.upVector));

	   
	//Passo al Vertex Shader il puntatore alla matrice View, che sar� associata alla variabile Uniform mat4 Projection
   //all'interno del Vertex shader. Uso l'identificatio MatView

	glPointSize(10.0);
	glUniformMatrix4fv(MatView, 1, GL_FALSE, value_ptr(View));

	//Definizione colore luce, posizione ed intensit�
	glUniform3f(light_unif.light_position_pointer, light.position.x + 10 * cos(radians(angolo)), light.position.y, light.position.z + 10 * sin(radians(angolo)));
	glUniform3f(light_unif.light_color_pointer, light.color.r, light.color.g, light.color.b);
	glUniform1f(light_unif.light_power_pointer, light.power);

	//Passo la posizione della camera
	glUniform3f(loc_view_pos, ViewSetup.position.x, ViewSetup.position.y, ViewSetup.position.z);
	glUniform1f(loc_time, time);
	for (int k =0; k <Scena.size(); k++)
	{
		 
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[k].Model));
		glUniform1i(lscelta, Scena[k].sceltaVS);
		
		
		glUniform3fv(light_unif.material_ambient, 1, glm::value_ptr(materials[Scena[k].material].ambient));
		glUniform3fv(light_unif.material_diffuse, 1, glm::value_ptr(materials[Scena[k].material].diffuse));
		glUniform3fv(light_unif.material_specular, 1, glm::value_ptr(materials[Scena[k].material].specular));
		glUniform1f(light_unif.material_shininess, materials[Scena[k].material].shininess);

		glBindVertexArray(Scena[k].VAO);
		 
		 glDrawElements(GL_TRIANGLES, (Scena[k].indici.size()-1)*sizeof(GLuint), GL_UNSIGNED_INT, 0);
 		int ind = Scena[k].indici.size() - 1;
	 
		//Disegno il centro della mesh: un punto in quella posizione
	    glDrawElements(GL_POINTS, 1,GL_UNSIGNED_INT, BUFFER_OFFSET(ind*sizeof(GLuint)));

		glBindVertexArray(0);

	}
	



	glutSwapBuffers();

}

void drawScene1(void)
{

	glClearColor(0.0, 1.0, 0.0, 0.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Projection_text = ortho(0.0f, (float)w_up, 0.0f, (float)h_up);


	RenderText(programId_text, Projection_text, Operazione, VAO_Text, VBO_Text, 10.0f, 100.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));

	RenderText(programId_text, Projection_text, stringa_asse, VAO_Text, VBO_Text, 10.0f, 70.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));


	RenderText(programId_text, Projection_text, "Oggetto selezionato", VAO_Text, VBO_Text, 10.0f, 50.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));
	if(selected_obj>-1)
	RenderText(programId_text, Projection_text, Scena[selected_obj].nome.c_str(), VAO_Text, VBO_Text, 10.0f, 10.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));

	glutSwapBuffers();

}

void main_menu_func(int option)
{
	switch (option)
	{
	case MenuOption::WIRE_FRAME: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case MenuOption::FACE_FILL: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	

	default:
		break;
	}
}
void material_menu_function(int option)
{
	 if (selected_obj>-1)
	 	Scena[selected_obj].material = (MaterialType)option;
}

void shader_menu_function(int option) {
	 if (selected_obj > -1)
	 Scena[selected_obj].sceltaVS = shaders[option].value;
	 
}

void buildOpenGLMenu()
{ 
	int materialSubMenu = glutCreateMenu(material_menu_function);

    glutAddMenuEntry(materials[MaterialType::EMERALD].name.c_str(), MaterialType::EMERALD);
	glutAddMenuEntry(materials[MaterialType::BRASS].name.c_str(), MaterialType::BRASS);
	glutAddMenuEntry(materials[MaterialType::SLATE].name.c_str(), MaterialType::SLATE);
	glutAddMenuEntry(materials[MaterialType::YELLOW].name.c_str(), MaterialType::YELLOW);
	
	int shaderSubMenu = glutCreateMenu(shader_menu_function);
	glutAddMenuEntry(shaders[ShaderOption::NONE].name.c_str(), ShaderOption::NONE);
	glutAddMenuEntry(shaders[ShaderOption::GOURAD_SHADING].name.c_str(), ShaderOption::GOURAD_SHADING);
	glutAddMenuEntry(shaders[ShaderOption::PHONG_SHADING].name.c_str(), ShaderOption::PHONG_SHADING);
	glutAddMenuEntry(shaders[ShaderOption::ONDE_SHADING].name.c_str(), ShaderOption::ONDE_SHADING);
	glutAddMenuEntry(shaders[ShaderOption::BANDIERA_SHADING].name.c_str(), ShaderOption::BANDIERA_SHADING);
	
	 
	glutCreateMenu(main_menu_func); // richiama main_menu_func() alla selezione di una voce menu
	glutAddMenuEntry("Opzioni", -1); //-1 significa che non si vuole gestire questa riga
	glutAddMenuEntry("", -1);
	glutAddMenuEntry("Wireframe", MenuOption::WIRE_FRAME);
	glutAddMenuEntry("Face fill", MenuOption::FACE_FILL);
	 glutAddSubMenu("Material", materialSubMenu);
	  glutAddSubMenu("Shader", shaderSubMenu);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);


	//Inizializzo finestra per il rendering della scena 3d con tutti i suoi eventi le sue inizializzazioni e le sue impostazioni

	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	idfg = glutCreateWindow("Scena 3D");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboardPressedEvent);
	glutTimerFunc(10, update, 0);
	glutMotionFunc(mouseActiveMotion); // Evento tasto premuto
	
	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	INIT_Illuminazione();
	buildOpenGLMenu();
	INIT_CAMERA_PROJECTION();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//Inizializzo finestra per il rendering delle informazioni con tutti i suoi eventi le sue inizializzazioni e le sue impostazioni

	glutInitWindowSize(width_i, height_i);
	glutInitWindowPosition(500, 350);
	idfi = glutCreateWindow("Informazioni");
	glutDisplayFunc(drawScene1);
	glutReshapeFunc(resize1);


	INIT_SHADER();
	INIT_VAO_Text();
	Init_Freetype();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Projection (in vertex shader).
	//QUesto identificativo sar� poi utilizzato per il trasferimento della matrice Projection al Vertex Shader
	MatrixProj = glGetUniformLocation(programId, "Projection");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//QUesto identificativo sar� poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	MatModel = glGetUniformLocation(programId, "Model");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 View (in vertex shader)
	//QUesto identificativo sar� poi utilizzato per il trasferimento della matrice View al Vertex Shader
	MatView = glGetUniformLocation(programId, "View");
	loc_time = glGetUniformLocation(programId, "time");
	loc_view_pos = glGetUniformLocation(programId, "ViewPos");
	 
	lscelta = glGetUniformLocation(programId, "sceltaVs");
	 
	light_unif.light_position_pointer = glGetUniformLocation(programId, "light.position");
	light_unif.light_color_pointer = glGetUniformLocation(programId, "light.color");
	light_unif.light_power_pointer = glGetUniformLocation(programId, "light.power");
	light_unif.material_ambient = glGetUniformLocation(programId, "material.ambient");
	light_unif.material_diffuse = glGetUniformLocation(programId, "material.diffuse");
	light_unif.material_specular = glGetUniformLocation(programId, "material.specular");
	light_unif.material_shininess = glGetUniformLocation(programId, "material.shininess");
	glutMainLoop();
}

