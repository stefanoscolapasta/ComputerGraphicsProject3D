#pragma once
#include <list>
#include "Agglomerato.h"
#include "Strutture.h"

#ifndef WORLD_H
#define WORLD_H

#define LIGHTS_NUM 4
static unsigned int MatrixProj, MatrixProj_txt, MatModel, MatView, loc_time, lsh, leye, lscelta, loc_view_pos;

class World {

public:
	LightUniform lights_unif[LIGHTS_NUM];
	point_light lights[LIGHTS_NUM];
	MaterialUniform material_unif;

	float width;
	float height;
	float w_up;
	float h_up;

	//Mesh muro;
	Mesh piano;

	list<Agglomerato> cespugli;
	list<Agglomerato> nuvole;


	World();

	void build(GLuint matModel);
	void upload_VA0_VB0();
	void insert_in_scena();

	void build_cespugli(GLuint matModel);
	void upload_cespugli();
	void insert_cespugli_in_scena();

	void set_Init_Position_Cespugli();



private:
	void set_init_position(GLuint matModel);

};


World::World() {

	for (int i = 0; i < LIGHTS_NUM; i++) {
		this->lights[i] = {};
		this->lights_unif[i] = {};
	}

	this->width = 1024.0;
	this->height = 800.0;
	this->w_up = this->width;
	this->h_up = this->height;

	//this->muro = {};
	this->piano = {};

	this->cespugli = {};
	this->nuvole = {};
}


void World::set_init_position(GLuint matModel) {
	/*set_transform_values(&this->muro,
		0.0f, 0.0f, -100.0f,
		1000.0f, 1000.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 90.0f);
	modify(&this->muro, matModel);*/

	set_transform_values(&this->piano,
		0.0f, -0.1f, 0.0f,
		1000.0f, 1.0f, 1000.0f,
		0.0f, 0.0f, 1.0f, 0.0f);
	modify(&this->piano, matModel);
}

void World::build(GLuint matModel) {
	/*crea_piano_suddiviso(&this->muro, vec4(0.2, 0.2, 0.9, 0.5));
	this->muro.nome = "Sfondo";
	this->muro.sceltaVS = 0;
	this->muro.material = MaterialType::EMERALD;*/
	
	crea_piano_suddiviso(&this->piano, vec4(0.21, 1.0, 0.3, 1));
	this->piano.nome = "Piano Terra";
	this->piano.sceltaVS = 1;
	this->piano.material = MaterialType::EMERALD;

	this->set_init_position(matModel);
}

void World::upload_VA0_VB0() {
	//crea_VAO_Vector(&this->muro);
	crea_VAO_Vector(&this->piano);
}

void World::insert_in_scena() {
	//Scena.push_back(&this->muro);
	Scena.push_back(&this->piano);
}

void World::set_Init_Position_Cespugli() {
	list <Agglomerato>::iterator it;
	vector<vec3> positions = {
		vec3(road.position.x + 3.0f * road.width, 0.0f, road.position.z + 2.0f * road.width),
		vec3(road.position.x + 5.0f * road.width, 0.0f, road.position.z + 7.0f * road.width), 
		vec3(road.position.x + 0.0f * road.width, 0.0f, road.position.z + 2.0f * road.width),
		vec3(road.position.x + 6.0f * road.width, 0.0f, road.position.z + 5.0f * road.width), 
		vec3(road.position.x + 0.0f * road.width, 0.0f, road.position.z + 5.0f * road.width), 
		vec3(road.position.x + 10.0f * road.width, 0.0f, road.position.z + 6.0f * road.width), 
		vec3(road.position.x + 8.0f * road.width, 0.0f, road.position.z + 1.0f * road.width), 
		vec3(road.position.x + 4.0f * road.width, 0.0f, road.position.z + 6.0f * road.width)
	};
	int i = 0;
	for (it = this->cespugli.begin(); it != this->cespugli.end(); ++it) {
		it->set_position(positions[i].x, positions[i].y, positions[i].z);
		i++;
	}
}

void World::build_cespugli(GLuint matModel) {
	for (int i = 0; i < 8; i++) {
		Agglomerato cespuglio = {};
		cespuglio.build_cespuglio(matModel);
		cespuglio.set_sceltaVS(1);
		this->cespugli.push_back(cespuglio);
	}
}

void World::upload_cespugli() {
	list <Agglomerato>::iterator it;
	for (it = this->cespugli.begin(); it != this->cespugli.end(); ++it) {
		it->upload_VA0_VB0();
	}
}

void World::insert_cespugli_in_scena() {
	list <Agglomerato>::iterator it;
	for (it = this->cespugli.begin(); it != this->cespugli.end(); ++it) {
		it->insert_in_scena();
	}
}



#endif
