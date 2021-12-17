#include <iostream>

#include "ShaderMaker.h"
#include "geometria.h"
#include "GestioneTesto.h"
#include <ft2build.h>

#include "Illuminazione.h"
#include "Materiali.h"
#include "VAO_And_Shader_Init.h"
#include "World.h"
#include "Camera.h"
#include "Menu.h"

#include FT_FREETYPE_H

#define PI 3.14159265358979323846

void modifyModelMatrix(Mesh* obj, glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor);

int width = 1024;
int height = 800;

int w_up = width;
int h_up = height;

string Operazione;
int idfg;
  
vec3 asse = vec3(0.0, 1.0, 0.0);
 
int selected_obj = -1;
Mesh Cubo, Piramide, Centri, Sfera;

vector<Material> materials;
vector<Shader> shaders;

point_light light;

//Puntatori alle variabili uniformi per l'impostazione dell'illuminazione
LightShaderUniform light_unif = {};

const float sphere_radius = 3.0f;
static bool moving_trackball = 0;
static int last_mouse_pos_Y;
static int last_mouse_pos_X;
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


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

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

Mesh Sfondo, Piano, Casa, Sole;

float angolo = 0.0;

vector<vec3> createCircularPathAtPos(float posZ) {
	int radious = 10.0f;
	int nTriangles = 100;
	float step = (2 * PI) / ((float)nTriangles);

	vector<vec3> positions;

	for (int i = 0; i <= nTriangles; i++) {
		positions.push_back(vec3(cos((double)i * step) * radious, sin((double)i * step) * radious, 0.0f));
	}

	return positions;
}

vector<vec3> path = createCircularPathAtPos(5.0f);
int pathPositionalIndex = 0;



void modifyModelMatrix(Mesh* obj, glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor)
{
	//ricordare che mat4(1) costruisce una matrice identità di ordine 4
	mat4 traslation = glm::translate(glm::mat4(1), translation_vector);
	mat4 scale = glm::scale(glm::mat4(1), glm::vec3(scale_factor, scale_factor, scale_factor));
	mat4 rotation = glm::rotate(glm::mat4(1), angle, rotation_vector);

	for (int i = 0; i < obj->vertici.size(); i++) {
		obj->vertici[i] += translation_vector * obj->initialScaleMultiplier;
		//funziona solo se l'oggetto seguito ha una scale = 1;
		//Per farlo funzionare devo moltiplicare il vettore applicato alla matrice model per il moltiplicatore della scale iniziale
	}

	//Modifica la matrice di Modellazione dell'oggetto della scena selezionato postmolitplicando per le matrici scale*rotation*traslation
	
	obj->Model = obj->Model * scale * rotation * traslation;

}

void modifyModelMatrixToPos(Mesh* obj, glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor)
{
	//ricordare che mat4(1) costruisce una matrice identità di ordine 4
	obj->Model = mat4(1);
	
	mat4 traslation = glm::translate(glm::mat4(1), translation_vector);
	mat4 scale = glm::scale(glm::mat4(1), glm::vec3(obj->initialScaleMultiplier, obj->initialScaleMultiplier, obj->initialScaleMultiplier));
	mat4 rotation = glm::rotate(glm::mat4(1), angle, rotation_vector);

	for (int i = 0; i < obj->vertici.size(); i++) {
		obj->vertici[i] = translation_vector * obj->initialScaleMultiplier;
		//funziona solo se l'oggetto seguito ha una scale = 1;
		//Per farlo funzionare devo moltiplicare il vettore applicato alla matrice model per il moltiplicatore della scale iniziale
	}

	//Modifica la matrice di Modellazione dell'oggetto della scena selezionato postmolitplicando per le matrici scale*rotation*traslation

	obj->Model = obj->Model * scale * rotation * traslation;
}


void update(int a) {
	angolo += 0.1;
	if (pathPositionalIndex == path.size()-1) {
		pathPositionalIndex = 0;
	}
	else {
		pathPositionalIndex++;
	}
	vec3 positionVector = path[pathPositionalIndex];

	modifyModelMatrixToPos(&Sole, positionVector, vec3(1.0f,0.0f,0.0f), 0.0f, 1.0f);
	makeCameraTrackObject(&Sole);
	//makeCameraFollowObject(&Sole, vec3(0.0f,0.0f, 30.0f));

	//modifyModelMatrixToPos(&Casa, vec3(ViewSetup.target.x, ViewSetup.target.y, ViewSetup.target.z));
	glutTimerFunc(10, update, 0);

	glutSetWindow(idfg);
	glutPostRedisplay();
}


glm::vec3 getTrackBallPoint(float x, float y) {
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
	//COSTRUZIONE AMBIENTE: STRUTTURA Scena

	//TERRENO
	crea_piano_suddiviso(&Piano, vec4(0.9, 0.9, 0.9, 0.5));
	crea_VAO_Vector(&Piano);
	Piano.nome = "Piano Terra";
	Piano.Model = mat4(1.0);
	Piano.Model = translate(Piano.Model, vec3(0.0, -4.5, 0.0));
	
	Piano.Model = scale(Piano.Model, vec3(1000.0f, 1.0f, 1000.0f));
	vec3 cx = Piano.Model * vec4(0.0, 0.0, 0.0, 1.0);
	centri.push_back(cx);
	raggi.push_back(0.5);
	Piano.sceltaVS = 0;
	Piano.material = MaterialType::EMERALD;
	Scena.push_back(&Piano);
	
	//CASA
	crea_casa(&Casa, vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 1.0, 1.0, 0.9));
	crea_VAO_Vector(&Casa);
	Casa.nome = "Casa";
	Casa.Model = mat4(1.0);
	Casa.Model = translate(Casa.Model, vec3(0, -2.5, 0.0));
	Casa.Model = scale(Casa.Model, vec3(2.0f, 2.0, 2.0f));
	cx = Casa.Model * vec4(-1.5, 3.5, 0.0, 1.0);
	centri.push_back(vec3(cx));
	raggi.push_back(0.5);
	Casa.sceltaVS = 0;
	Casa.material = MaterialType::EMERALD;
	Scena.push_back(&Casa);

	//SOLE
	crea_sfera(&Sole, vec4(1.0, 1.0, 0.0, 1.0));
	crea_VAO_Vector(&Sole);
	Sole.nome = "Sole";
	Sole.Model = mat4(1.0);
	Sole.Model = translate(Sole.Model, vec3(0, 2.5, 0));
	Sole.initialScaleMultiplier = 4.0f;
	Sole.Model = scale(Sole.Model, vec3(Sole.initialScaleMultiplier, Sole.initialScaleMultiplier, Sole.initialScaleMultiplier));
	cx = Sole.Model * vec4(-1.5, 3.5, 0.0, 1.0);
	centri.push_back(vec3(cx));
	raggi.push_back(0.5);
	Sole.sceltaVS = 0;
	Sole.material = MaterialType::RED_PLASTIC;
	Scena.push_back(&Sole);

}


void keyboardPressedEvent(unsigned char key, int x, int y) {

	moveCamera(key);

	switch (key) {
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
		break;
	case 'y':
		WorkingAxis = Y;  //Seleziona l'asse X come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
		break;
	case 'z':
		WorkingAxis = Z;
		break;

	default:
		break;
	}

	// Selezione dell'asse per le trasformazioni
	switch (WorkingAxis) {
		case X:	asse = glm::vec3(1.0, 0.0, 0.0); break;
		case Y: asse = glm::vec3(0.0, 1.0, 0.0); break;
		case Z: asse = glm::vec3(0.0, 0.0, 1.0); break;
		default:
			break;
	}

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
		//modifyModelMatrix(asse * amount, asse, 0.0f, 1.0f);
		break;
	case ROTATING:
		// SI mette a zero il vettore di traslazione (vec3(0) e ad 1 il fattore di scale
		//modifyModelMatrix(glm::vec3(0), asse, amount * 2.0f, 1.0f);
		break;
	case SCALING:
		// SI mette a zero il vettore di traslazione (vec3(0), angolo di rotazione a 0 e ad 1 il fattore di scala 1+amount.
		//modifyModelMatrix(glm::vec3(0), asse, 0.0f, 1.0f + amount);
		break;
	}
	glutSetWindow(idfg);
	glutPostRedisplay();
}

vec3 get_ray_from_mouse(float mouse_x, float mouse_y) {
	// mappiamo le coordinate di viewport del mouse [0,width], [height,0] in coordinate normalizzate [-1,1]  
	float x = (2.0f * mouse_x) / width - 1.0;
	float y = 1.0f - (2.0f * mouse_y) / height;
	float z = 1.0f;
	vec3 ray_nds = vec3(x, y, z);

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
		return !(t_a < 0.0 && t_b < 0);
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

void mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0)
	{
		// Zoom in
	}
	else
	{
		// Zoom out
	}

	return;
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
				printf("Oggetto selezionato %d -> %s \n", selected_obj, Scena[selected_obj]->nome.c_str());
		}
		break;
	default:
		break;
	} 

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

void drawScene(void)
{
	/*
	float timeValue = glutGet(GLUT_ELAPSED_TIME) * 0.001;
	ViewSetup.position.x = sin(timeValue) * 10.0;
	ViewSetup.position.z = cos(timeValue) * 10.0;
	*/
	
	//Passo al Vertex Shader il puntatore alla matrice Projection, che sar� associata alla variabile Uniform mat4 Projection
	//all'interno del Vertex shader. Uso l'identificatio MatrixProj
	glPointSize(10.0);
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
	glUniformMatrix4fv(MatView, 1, GL_FALSE, value_ptr(View));

	//Definizione colore luce, posizione ed intensit�
	glUniform3f(light_unif.light_position_pointer, light.position.x + 10 * cos(radians(angolo)), light.position.y, light.position.z + 10 * sin(radians(angolo)));
	glUniform3f(light_unif.light_color_pointer, light.color.r, light.color.g, light.color.b);
	glUniform1f(light_unif.light_power_pointer, light.power);

	//Passo la posizione della camera
	glUniform3f(loc_view_pos, ViewSetup.position.x, ViewSetup.position.y, ViewSetup.position.z);
	glUniform1f(loc_time, time);

	/* ! CUORE !*/
	for (int k =0; k <Scena.size(); k++)
	{
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[k]->Model));
		glUniform1i(lscelta, Scena[k]->sceltaVS);
		
		glUniform3fv(light_unif.material_ambient, 1, glm::value_ptr(materials[Scena[k]->material].ambient));
		glUniform3fv(light_unif.material_diffuse, 1, glm::value_ptr(materials[Scena[k]->material].diffuse));
		glUniform3fv(light_unif.material_specular, 1, glm::value_ptr(materials[Scena[k]->material].specular));
		glUniform1f(light_unif.material_shininess, materials[Scena[k]->material].shininess);

		glBindVertexArray(Scena[k]->VAO);
		glDrawElements(GL_TRIANGLES, (Scena[k]->indici.size()-1)*sizeof(GLuint), GL_UNSIGNED_INT, 0);
 		int ind = Scena[k]->indici.size() - 1;
		//Disegno il centro della mesh: un punto in quella posizione
	    //glDrawElements(GL_POINTS, 1,GL_UNSIGNED_INT, BUFFER_OFFSET(ind*sizeof(GLuint)));
		glBindVertexArray(0);
	}
	glutSwapBuffers();
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
	glutMouseWheelFunc(mouseWheel);

	glewExperimental = GL_TRUE;
	glewInit();

	INIT_SHADER();
	INIT_VAO();
	INIT_Illuminazione(&light, materials, shaders);

	buildOpenGLMenu();
	INIT_CAMERA_PROJECTION(width, height);
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

