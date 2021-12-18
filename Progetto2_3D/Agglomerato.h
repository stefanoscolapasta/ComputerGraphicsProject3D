#pragma once


#include <list>
#include "Strutture.h"
#include "geometria.h"
#include "Color.h"
#include "VAO_And_Shader_Init.h"
#include "TransformMesh.h"

#ifndef AGGLOMERATO_H
#define AGGLOMERATO_H

#define N_LEAF 11;

static float posXCespugli[11] = { -2.5f, -2.f, -1.5f, -1.0f, -.5f, 0.0f, .5f, 1.0f, 1.5f, 2.0f, 2.5f };
static float posYCespugli[11] = { 0.0f, 3.0f, 2.0f, 2.5f, 3.0f, 0.0f, 3.0f, 2.5f, 2.0f, 3.0f, 0.0f };

static float posXNuvole[11] = { -0.15f, -0.15f, -0.3f, -0.3f, 0.0f, 0.0f, 0.3f, 0.3f, 0.15f, 0.15f, 0.05f };
static float posYNuvole[11] = { 1.0f, -1.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 1.0f, -1.0f, 1.0f };
static float posZNuvole[11] = { -4.0f, -4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f, 4.0f, 4.0f };


class Agglomerato {

public:
	list<Mesh> body;
	int nLeaf;
	float x, y, z;

	Agglomerato();

	void build_cespuglio(GLuint matModel);
	void build_nuvola(GLuint matModel);
	void upload_VA0_VB0();
	void insert_in_scena();
	void set_sceltaVS(int scelta);

	void set_position(float x, float y, float z);
	void set_position_nuvola(float x, float y, float z);
	void translate(float x, float y, float z);

private:
	void set_init_position(GLuint matModel);
	void set_init_position_nuvola(GLuint matModel);

};

Agglomerato::Agglomerato() {
	this->body = {};
	this->nLeaf = N_LEAF;
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
}

void Agglomerato::set_init_position(GLuint matModel) {
	int index = 0;

	list <Mesh>::iterator it;
	for (it = this->body.begin(); it != this->body.end(); ++it) {
		set_transform_values(&(*it), posXCespugli[index], posYCespugli[index], 0.0f,
			2.0f, 2.0f, 2.0f,
			0.0f, 0.0f, 1.0f, 0.0f);
		modify(&(*it), matModel);
		index += 1;
	}
}

void Agglomerato::set_init_position_nuvola(GLuint matModel) {
	int index = 0;

	list <Mesh>::iterator it;
	for (it = this->body.begin(); it != this->body.end(); ++it) {
		set_transform_values(&(*it), posXNuvole[index], posYNuvole[index], posZNuvole[index],
			3.0f, 3.0f, 3.0f,
			0.0f, 0.0f, 1.0f, 0.0f);
		modify(&(*it), matModel);
		index += 1;
	}
}

void Agglomerato::build_cespuglio(GLuint matModel) {
	for (int i = 0; i < this->nLeaf; i++) {
		Mesh foglia = {};
		crea_sfera(&foglia, vec_RGBA(63, 195, 128));
		foglia.nome = "foglia";
		foglia.sceltaVS = 1;
		foglia.material = MaterialType::EMERALD;
		this->body.push_back(foglia);
	}

	this->set_init_position(matModel);
	this->set_sceltaVS(1);
}

void Agglomerato::build_nuvola(GLuint matModel) {
	for (int i = 0; i < this->nLeaf; i++) {
		Mesh foglia = {};
		crea_sfera(&foglia, vec_RGBA(3, 138, 255));
		foglia.nome = "nuvola";
		foglia.sceltaVS = 1;
		foglia.material = MaterialType::NUVOLA;
		this->body.push_back(foglia);
	}

	this->set_init_position_nuvola(matModel);
	this->set_sceltaVS(0);
}

void Agglomerato::insert_in_scena() {
	list <Mesh>::iterator it;
	for (it = this->body.begin(); it != this->body.end(); ++it) {
		Scena.push_back(&(*it));
	}
}

void Agglomerato::upload_VA0_VB0() {
	list <Mesh>::iterator it;
	for (it = this->body.begin(); it != this->body.end(); ++it) {
		crea_VAO_Vector(&(*it));
	}
}

void Agglomerato::set_sceltaVS(int scelta) {
	list <Mesh>::iterator it;
	for (it = this->body.begin(); it != this->body.end(); ++it) {
		it->sceltaVS = scelta;
	}
}

void Agglomerato::set_position(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;

	int index = 0;
	list <Mesh>::iterator it;
	for (it = this->body.begin(); it != this->body.end(); ++it) {
		set_move_value(&(*it), x + posXCespugli[index], y + posYCespugli[index], z);
		index += 1;
	}
}

void Agglomerato::set_position_nuvola(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;

	int index = 0;
	list <Mesh>::iterator it;
	for (it = this->body.begin(); it != this->body.end(); ++it) {
		set_move_value(&(*it), x + posXNuvole[index], y + posYNuvole[index], z + posZNuvole[index]);
		index += 1;
	}
}


void Agglomerato::translate(float x, float y, float z) {
	this->x += x;
	this->y += y;
	this->z += z;

	list <Mesh>::iterator it;
	for (it = this->body.begin(); it != this->body.end(); ++it) {
		increment_move(&(*it), x, y, z);
	}
}





#endif
