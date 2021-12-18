#include <iostream>

#include "ShaderMaker.h"
#include "geometria.h"
#include <ft2build.h>

#include "Illuminazione.h"
#include "Materiali.h"
#include "VAO_And_Shader_Init.h"
#include "World.h"
#include "Camera.h"
#include "Menu.h"
#include "Agglomerato.h"
#include "TransformMesh.h"
#include "DrawMesh.h"
#include "Strutture.h"
#include "KeyCar.h"
#include FT_FREETYPE_H

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

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
void modifyModelMatrix(Mesh* obj, glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor);

World world = World();

string Operazione;
int idfg;
  
vec3 asse = vec3(0.0, 1.0, 0.0);
 
int selected_obj = -1;

Mesh Cubo, Tetto, Cofano, Ruota, Piano, Piramide, Centri, Sfera, Sole, Sfondo;
Mesh Palo, Lampione;



Mesh cerchio, quadrato;

vector<Material> materials;
vector<Shader> shaders;

//Puntatori alle variabili uniformi per l'impostazione dell'illuminazione
const float sphere_radius = 3.0f;
static bool moving_trackball = 0;
static int last_mouse_pos_Y;
static int last_mouse_pos_X;


mat4 Projection, Model, View;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float angolo = 0.0;

float period = 400; // 4 secondi

int row = 0;
int column = 0;

Road road= { 12, vec3(-6 * 12, 0, -6 * 12) };
Car car = { vec3(road.width / 2, 0, road.width) + road.position, D_UP, ((2 * (float)PI) * (road.width / 2) / period) };




vector<vec3> createCircularPathAtPos(float posZ) {
	int radious = 10.0f;
	int nTriangles = 1000;
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

vec3 calculateDirectionVecFromInt(int dir) {

	switch (dir) {
	case D_UP:
		return vec3(0.0f, 0.0f, -1.0f);
	case D_RIGHT:
		return vec3(1.0f, 0.0f, 0.0f);
	case D_LEFT:
		return vec3(-1.0f, 0.0f, 0.0f);
	case D_DOWN:
		return vec3(0.0f, 0.0f, 1.0f);
	default:
		return vec3(0.0f, 0.0f, 0.0f);
	}
}

float x = 0.05, y = 0.05f, z = 0.01;
int i = 0;

void update(int a) {
	world.translate_nuvole(x, y, z);
	i++;

	if (i % 100) {
		y = -y;
	}

	if (i == 500) {
		i = 0;
		x = -x;
		z = -z;
	}

	world.lights[0].position = calculateObjectVerticesBarycenter(&Sole);
	/* Positione del sole */


	/* Movement */
	int slot = roadMatrix[row][column];
	if ((slot == UP_L && car.direction == D_UP) || (slot == UP_R && car.direction == D_RIGHT)
		|| (slot == DW_R && car.direction == D_DOWN) || (slot == DW_L && car.direction == D_LEFT))
	{
		vec3 velocity = -car.velocity * vec3(0, 0, 1);
		car.rotation += 2 * PI / (period);
		car.position += vec3(vec4(velocity, 0) * rotate(mat4(1), car.rotation, vec3(0, 1, 0)));
	}
	else if ((slot == UP_L && car.direction == D_LEFT) || (slot == UP_R && car.direction == D_UP)
		|| (slot == DW_R && car.direction == D_RIGHT) || (slot == DW_L && car.direction == D_DOWN))
	{
		vec3 velocity = -car.velocity * vec3(0, 0, 1);
		car.rotation -= 2 * PI / (period);
		car.position += vec3(vec4(velocity, 0) * rotate(mat4(1), car.rotation, vec3(0, 1, 0)));
	}
	else if (roadMatrix[row][column] == STR) 
	{
		if (car.direction == D_RIGHT) {
			car.position += car.velocity * vec3(1, 0, 0);
		}
		else if (car.direction == D_DOWN) {
			car.position += car.velocity * vec3(0, 0, 1);
		}
		else if (car.direction == D_UP) {
			car.position -= car.velocity * vec3(0, 0, 1);
		}
		else if (car.direction == D_LEFT) {
			car.position -= car.velocity * vec3(1, 0, 0);
		}
	}

	/**/
	if (car.position.x >= road.position.x + (column + 1) * road.width) {
		if (slot == UP_L && car.direction == D_UP || slot == DW_L && car.direction == D_DOWN) {
			car.direction = D_RIGHT;
			column++;
		}
		else if (slot == STR && car.direction == D_RIGHT) {
			column++;
		}
	}
	else if (car.position.x <= road.position.x + column * road.width) {
		if (slot == UP_R && car.direction == D_UP || slot == DW_R && car.direction == D_DOWN) {
			car.direction = D_LEFT;
			column--;
		}
		else if (slot == STR && car.direction == D_LEFT) {
			column--;
		}
	}
	else if (car.position.z >= road.position.z + (row + 1) * road.width) {
		if (slot == UP_R && car.direction == D_RIGHT || slot == UP_L && car.direction == D_LEFT) {
			car.direction = D_DOWN;
			row++;
		}
		else if (slot == STR && car.direction == D_DOWN) {
			row++;
		}
	}
	else if (car.position.z <= road.position.z + row * road.width) {
		if (slot == DW_R && car.direction == D_RIGHT || slot == DW_L && car.direction == D_LEFT) {
			car.direction = D_UP;
			row--;
		}
		else if (slot == STR && car.direction == D_UP) {
			row--;
		}
	}



	if (pathPositionalIndex == path.size()-1) {
		pathPositionalIndex = 0;
	}
	else {
		pathPositionalIndex++;
	}
	vec3 positionVector = path[pathPositionalIndex];


	vec3 directVect = car.position - cameraMovementHandler.oldPosition;
	


	modifyModelMatrixToPos(&Sole, positionVector, vec3(1.0f,0.0f,0.0f), 0.0f, 1.0f);
	if (!cameraMovementHandler.detach) {
		makeCameraTrackObject(car.position);
		//makeCameraPointDirection(calculateDirectionVecFromInt(car.direction));
		vec3 positionToFollow = car.position + (cameraMovementHandler.switchSideViewingSelector) * (directVect * cameraMovementHandler.distanceMultiplierToTarget);
		makeCameraFollowObject(positionToFollow, vec3(0.0f, cameraMovementHandler.deltaYOffset, 0.0f));
	}
	cameraMovementHandler.oldPosition = car.position;

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
	point.x = (2.0f * x - world.width) / world.width;
	point.y = (world.height - 2.0f * y) / world.height;

	//Cooordinata z del punto di coordinate (x,y,z) che si muove sulla sfera virtuale con centro (0,0,0) e raggio r=1
	zTemp = 1.0f - pow(point.x, 2.0) - pow(point.y, 2.0);
	point.z = (zTemp > 0.0f) ? sqrt(zTemp) : 0.0;
	return point;
}

void INIT_VAO(void)
{

	world.build(MatModel);
	world.upload_VA0_VB0();
	world.insert_in_scena();
	
	world.build_cespugli(MatModel);
	world.upload_cespugli();
	world.insert_cespugli_in_scena();
	world.set_init_position_cespugli();

	world.build_nuvole(MatModel);
	world.upload_nuvole();
	world.insert_nuvole_in_scena();
	world.set_init_position_nuvole();

	//COSTRUZIONE AMBIENTE: STRUTTURA Scena
	crea_curva(&cerchio, road.width);
	crea_VAO_Vector(&cerchio);
	crea_piano(&quadrato);
	crea_VAO_Vector(&quadrato);
	crea_cubo(&Cubo, vec4(0, 0, 1, 1), vec4(0, 1, 1, 1));
	crea_VAO_Vector(&Cubo);
	crea_tetto_macchina(&Tetto, vec4(1, 0, 0, 1), vec4(1, 0, 0, 1));
	crea_VAO_Vector(&Tetto);
	crea_cofano(&Cofano);
	crea_VAO_Vector(&Cofano);
	crea_cilindro(&Ruota, vec4(0, 0, 0, 1));
	crea_VAO_Vector(&Ruota);
	/*LAMPIONe*/

	crea_cilindro(&Palo, vec4(0, 0, 0, 1));
	crea_VAO_Vector(&Palo);
	crea_sfera(&Lampione, vec4(1, 1, 1, 1));
	crea_VAO_Vector(&Lampione);


	//SOLE
	crea_sfera(&Sole, vec4(1.0, 1.0, 0.0, 1.0));
	crea_VAO_Vector(&Sole);
	Sole.nome = "Sole";
	Sole.Model = mat4(1.0);
	Sole.Model = translate(Sole.Model, vec3(0, 2.5, 0));
	Sole.initialScaleMultiplier = 40.0f;
	Sole.Model = scale(Sole.Model, vec3(Sole.initialScaleMultiplier, Sole.initialScaleMultiplier, Sole.initialScaleMultiplier));
	//cx = Sole.Model * vec4(-1.5, 3.5, 0.0, 1.0);
	//centri.push_back(vec3(cx));
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
	case '+':
		cameraMovementHandler.deltaYOffset+=0.3f;
		break;
	case '-':
		cameraMovementHandler.deltaYOffset-=0.3f;
		break;
	case 'v':
		cameraMovementHandler.switchSideViewingSelector = -cameraMovementHandler.switchSideViewingSelector;
		break;
	case 't':
		cameraMovementHandler.detach = !cameraMovementHandler.detach;
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

	glutSetWindow(idfg);
	glutPostRedisplay();
}

vec3 get_ray_from_mouse(float mouse_x, float mouse_y) {
	// mappiamo le coordinate di viewport del mouse [0,world.width], [world.height,0] in coordinate normalizzate [-1,1]  
	float x = (2.0f * mouse_x) / world.width - 1.0;
	float y = 1.0f - (2.0f * mouse_y) / world.height;
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
		cameraMovementHandler.distanceMultiplierToTarget -= 10.0f;
	}
	else
	{
		cameraMovementHandler.distanceMultiplierToTarget += 10.0f;
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
	world.w_up = w;
	world.h_up = h;
}

void setMaterialUniform(MaterialType material) {
	glUniform3fv(world.material_unif.ambient, 1, glm::value_ptr(materials[material].ambient));
	glUniform3fv(world.material_unif.diffuse, 1, glm::value_ptr(materials[material].diffuse));
	glUniform3fv(world.material_unif.specular, 1, glm::value_ptr(materials[material].specular));
	glUniform1f(world.material_unif.shininess, materials[material].shininess);
}

void drawScene(void)
{
	float sunHeightFactor = glm::max(0.0f, calculateObjectVerticesBarycenter(&Sole).y) / 400;
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
	glClearColor(f_RGB(0), f_RGB(89) * sunHeightFactor, f_RGB(255) * sunHeightFactor, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Costruisco la matrice di Vista che applicata ai vertici in coordinate del mondo li trasforma nel sistema di riferimento della camera.
	View = lookAt(vec3(ViewSetup.position), vec3(ViewSetup.target), vec3(ViewSetup.upVector));
	//Passo al Vertex Shader il puntatore alla matrice View, che sar� associata alla variabile Uniform mat4 Projection
	//all'interno del Vertex shader. Uso l'identificatio MatView
	glUniformMatrix4fv(MatView, 1, GL_FALSE, value_ptr(View));

	//Definizione colore luce, posizione ed intensit�
	for (int i = 0; i < LIGHTS_NUM; i++) {
		define_light_position_and_intensity(&world.lights_unif[i], &world.lights[i], angolo);
	}

	//Passo la posizione della camera
	glUniform3f(loc_view_pos, ViewSetup.position.x, ViewSetup.position.y, ViewSetup.position.z);
	glUniform1f(loc_time, time);


	/* ! CUORE !*/

	//world.cespuglio.set_position(10.0f, 10.0f, 10.0f);

	for (int k =0; k < Scena.size(); k++) {
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, glm::value_ptr(Scena[k]->Model));
		setMaterialUniform(Scena[k]->material);
		draw_mesh(&(*Scena[k]), GL_TRIANGLES, MatModel);
	}
	glUniform1i(lscelta, 1);
	Cubo.Model = mat4(1);
	Cubo.Model = translate(Cubo.Model, car.position);
	Cubo.Model = rotate(Cubo.Model, -car.rotation, vec3(0, 1, 0));
	Cubo.Model = translate(Cubo.Model, vec3(0, 1.1, 0));
	Cubo.Model = scale(Cubo.Model, vec3(2, 0.1, 4));
	setMaterialUniform(MaterialType::RED_PLASTIC);
	glBindVertexArray(Cubo.VAO);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Cubo.Model));
	glDrawElements(GL_TRIANGLES, (Cubo.indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);

	Tetto.Model = mat4(1);
	Tetto.Model = translate(Tetto.Model, car.position);
	Tetto.Model = rotate(Tetto.Model, -car.rotation, vec3(0, 1, 0));
	Tetto.Model = translate(Tetto.Model, vec3(0, 1.9, 1));
	Tetto.Model = scale(Tetto.Model, vec3(2, 0.8, 3));
	glBindVertexArray(Tetto.VAO);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Tetto.Model));
	glDrawElements(GL_TRIANGLES, (Tetto.indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);


	Cofano.Model = mat4(1);
	Cofano.Model = translate(Cofano.Model, car.position);
	Cofano.Model = rotate(Cofano.Model, -car.rotation, vec3(0, 1, 0));
	Cofano.Model = translate(Cofano.Model, vec3(0, 1.2, -3));
	Cofano.Model = scale(Cofano.Model, vec3(2, 1, 1));
	glBindVertexArray(Cofano.VAO);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Cofano.Model));
	glDrawElements(GL_TRIANGLES, (Cofano.indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);

	setMaterialUniform(MaterialType::BLACK_RUBBER);
	Ruota.Model = mat4(1);
	Ruota.Model = translate(Ruota.Model, car.position);
	Ruota.Model = rotate(Ruota.Model, -car.rotation, vec3(0, 1, 0));
	Ruota.Model = translate(Ruota.Model, vec3(-1, 0.5, -3));
	Ruota.Model = rotate(Ruota.Model, radians(90.0f), vec3(0, 0, 1));
	Ruota.Model = scale(Ruota.Model, vec3(0.5, 1, 0.5));
	glBindVertexArray(Ruota.VAO);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Ruota.Model));
	glDrawElements(GL_TRIANGLES, (Ruota.indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);

	Ruota.Model = mat4(1);
	Ruota.Model = translate(Ruota.Model, car.position);
	Ruota.Model = rotate(Ruota.Model, -car.rotation, vec3(0, 1, 0));
	Ruota.Model = translate(Ruota.Model, vec3(2, 0.5, -3));
	Ruota.Model = rotate(Ruota.Model, radians(90.0f), vec3(0, 0, 1));
	Ruota.Model = scale(Ruota.Model, vec3(0.5, 1, 0.5));
	glBindVertexArray(Ruota.VAO);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Ruota.Model));
	glDrawElements(GL_TRIANGLES, (Ruota.indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);

	Ruota.Model = mat4(1);
	Ruota.Model = translate(Ruota.Model, car.position);
	Ruota.Model = rotate(Ruota.Model, -car.rotation, vec3(0, 1, 0));
	Ruota.Model = translate(Ruota.Model, vec3(-1, 0.5, 3));
	Ruota.Model = rotate(Ruota.Model, radians(90.0f), vec3(0, 0, 1));
	Ruota.Model = scale(Ruota.Model, vec3(0.5, 1, 0.5));
	glBindVertexArray(Ruota.VAO);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Ruota.Model));
	glDrawElements(GL_TRIANGLES, (Ruota.indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);

	Ruota.Model = mat4(1);
	Ruota.Model = translate(Ruota.Model, car.position);
	Ruota.Model = rotate(Ruota.Model, -car.rotation, vec3(0, 1, 0));
	Ruota.Model = translate(Ruota.Model, vec3(2, 0.5, 3));
	Ruota.Model = rotate(Ruota.Model, radians(90.0f), vec3(0, 0, 1));
	Ruota.Model = scale(Ruota.Model, vec3(0.5, 1, 0.5));
	glBindVertexArray(Ruota.VAO);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Ruota.Model));
	glDrawElements(GL_TRIANGLES, (Ruota.indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	
	setMaterialUniform(MaterialType::SLATE);
	for (int i = 0; i < roadMatrix.size(); i++) {
		for (int j = 0; j < roadMatrix[i].size(); j++) {
			mat4 modellation = translate(mat4(1), road.position + vec3(j * road.width, 0, (i + 1) * road.width));
			if (roadMatrix[i][j] >= UP_R && roadMatrix[i][j] <= DW_R) {
				cerchio.Model = modellation;
				switch (roadMatrix[i][j])
				{
				case UP_L:
					cerchio.Model = translate(cerchio.Model, vec3(road.width, 0, 0));
					cerchio.Model = scale(cerchio.Model, vec3(-1, 1, 1));
					break;
				case DW_L:
					cerchio.Model = translate(cerchio.Model, vec3(road.width, 0, -road.width));
					cerchio.Model = scale(cerchio.Model, vec3(-1, 1, -1));
					break;
				case DW_R:
					cerchio.Model = translate(cerchio.Model, vec3(0, 0, -road.width));
					cerchio.Model = scale(cerchio.Model, vec3(1, 1, -1));
					break;
				default:
					break;
				}
				glBindVertexArray(cerchio.VAO);
				glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(cerchio.Model));
				glDrawElements(GL_TRIANGLES, (cerchio.indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
			} else if (roadMatrix[i][j] == STR) {
				quadrato.Model = modellation;
				quadrato.Model = translate(quadrato.Model, vec3(road.width/2, 0, -road.width/2));
				quadrato.Model = scale(quadrato.Model, vec3(road.width/2, 1, road.width/2));
				glBindVertexArray(quadrato.VAO);
				glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(quadrato.Model));
				glDrawElements(GL_TRIANGLES, (quadrato.indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
			}
		}
	}
	for (int i = 0; i < lampioniAndLightsPosition.size(); i++) {
		/* Lampione */
		glUniform1i(lscelta, 0);
		Palo.Model = mat4(1);
		Palo.Model = translate(Palo.Model, lampioniAndLightsPosition[i]);
		Palo.Model = scale(Palo.Model, vec3(0.2, 7, 0.2));
		glBindVertexArray(Palo.VAO);
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Palo.Model));
		glDrawElements(GL_TRIANGLES, (Palo.indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);

		Lampione.Model = mat4(1);
		Lampione.Model = translate(Lampione.Model, lampioniAndLightsPosition[i]+vec3(0,7.5f,0));
		Lampione.Model = scale(Lampione.Model, vec3(1, 1, 1));
		glBindVertexArray(Lampione.VAO);
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Lampione.Model));
		glDrawElements(GL_TRIANGLES, (Lampione.indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	}


	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	//Inizializzo finestra per il rendering della scena 3d con tutti i suoi eventi le sue inizializzazioni e le sue impostazioni
	glutInitWindowSize(world.width, world.height);
	glutInitWindowPosition(100, 100);
	idfg = glutCreateWindow("Scena 3D");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboardPressedEvent);
	glutSpecialFunc(SpecialInput);
	glutTimerFunc(2000, update, 0);
	glutMotionFunc(mouseActiveMotion); // Evento tasto premuto
	glutMouseWheelFunc(mouseWheel);

	glewExperimental = GL_TRUE;
	glewInit();

	INIT_SHADER();
	INIT_VAO();
	INIT_Illuminazione(world.lights, materials, shaders);

	INIT_CAMERA_PROJECTION(world.width, world.height);
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

	/* location delle lights */
	for (int i = 0; i < LIGHTS_NUM; i++) {
		string positionStr = "lights[" + to_string(i) + "].position";
		world.lights_unif[i].position = glGetUniformLocation(programId, positionStr.c_str());
		string colorStr = "lights[" + to_string(i) + "].color";
		world.lights_unif[i].color = glGetUniformLocation(programId, colorStr.c_str());
		string lightStr = "lights[" + to_string(i) + "].power";
		world.lights_unif[i].power = glGetUniformLocation(programId, lightStr.c_str());
	}

	world.material_unif.ambient = glGetUniformLocation(programId, "material.ambient");
	world.material_unif.diffuse = glGetUniformLocation(programId, "material.diffuse");
	world.material_unif.specular = glGetUniformLocation(programId, "material.specular");
	world.material_unif.shininess = glGetUniformLocation(programId, "material.shininess");

	glutMainLoop();
}
