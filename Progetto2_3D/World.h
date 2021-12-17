#pragma once
#include <list>
#include "Cespuglio.h"
#include "Strutture.h"

#ifndef WORLD_H
#define WORLD_H

static unsigned int MatrixProj, MatrixProj_txt, MatModel, MatView, loc_time, lsh, leye, lscelta, loc_view_pos;

class World {

public:
	LightShaderUniform light_unif;
	point_light light;

	float width;
	float height;
	float w_up;
	float h_up;

	Mesh muro;
	Mesh piano;

	Cespuglio cespuglio;

	World();

	void build(GLuint matModel);
	void upload_VA0_VB0();
	void insert_in_scena();

private:
	void set_init_position(GLuint matModel);

};


World::World() {
	this->light_unif = {};
	this->light = {};

	this->width = 1024.0;
	this->height = 800.0;
	this->w_up = this->width;
	this->h_up = this->height;

	this->muro = {};
	this->piano = {};

	this->cespuglio = {};
}


void World::set_init_position(GLuint matModel) {
	set_transform_values(&this->muro,
		0.0f, 0.0f, -100.0f,
		1000.0f, 1000.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 90.0f);
	modify(&this->muro, matModel);

	set_transform_values(&this->piano,
		0.0f, -4.5f, 0.0f,
		1000.0f, 1.0f, 1000.0f,
		0.0f, 0.0f, 1.0f, 0.0f);
	modify(&this->piano, matModel);
}

void World::build(GLuint matModel) {
	crea_piano_suddiviso(&this->muro, vec4(0.2, 0.2, 0.9, 0.5));
	this->muro.nome = "Sfondo";
	this->muro.sceltaVS = 0;
	this->muro.material = MaterialType::EMERALD;
	
	crea_piano_suddiviso(&this->piano, vec4(0.9, 0.9, 0.9, 0.5));
	this->piano.nome = "Piano Terra";
	this->piano.sceltaVS = 0;
	this->piano.material = MaterialType::EMERALD;

	this->set_init_position(matModel);
}

void World::upload_VA0_VB0() {
	crea_VAO_Vector(&this->muro);
	crea_VAO_Vector(&this->piano);
}

void World::insert_in_scena() {
	Scena.push_back(this->muro);
	Scena.push_back(this->piano);
}




#endif
