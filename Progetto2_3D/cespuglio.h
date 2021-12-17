#pragma once


#include <list>
#include "Strutture.h"
#include "geometria.h"
#include "Color.h"
#include "VAO_And_Shader_Init.h"
#include "TransformMesh.h"

#ifndef CESPUGLIO_H
#define CESPUGLIO_H

#define N_LEAF 11;

class Cespuglio {

public:
	list<Mesh> body;
	int nLeaf;
	float x;
	float y;

	Cespuglio();
	//~Cespuglio();

	void build(GLuint matModel);
	void upload_VA0_VB0();
	void insert_in_scena();
	void set_sceltaVS(int scelta);
	void translate(GLuint matModel, float x, float y, float z);

private:
	void set_init_position(GLuint matModel);

};

Cespuglio::Cespuglio() {
	this->body = {};
	this->nLeaf = N_LEAF;
	this->x = 0.0f;
	this->y = 3.0f;
}

void Cespuglio::set_init_position(GLuint matModel) {
	float posX [11] = { -5.0f, -4.0f, -3.0f, -2.0f, -1.0f, this->x, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
	float posY [11] = { 0.0f, 4.0f, this->y, this->y + 0.5f, 4.0f, 0.0f, 4.0f, this->y + 0.5f, this->y, 4.0f, 0.0f };
	int index = 0;

	list <Mesh>::iterator it;
	for (it = this->body.begin(); it != this->body.end(); ++it) {
		set_transform_values(&(*it), posX[index], posY[index], 0.0f,
			4.0f, 4.0f, 4.0f,
			0.0f, 0.0f, 1.0f, 0.0f);
		modify(&(*it), matModel);
		index += 1;
	}
}

void Cespuglio::build(GLuint matModel) {
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

void Cespuglio::insert_in_scena() {
	list <Mesh>::iterator it;
	for (it = this->body.begin(); it != this->body.end(); ++it) {
		Scena.push_back(*it);
	}
}

void Cespuglio::upload_VA0_VB0() {
	list <Mesh>::iterator it;
	for (it = this->body.begin(); it != this->body.end(); ++it) {
		crea_VAO_Vector(&(*it));
	}
}

void Cespuglio::set_sceltaVS(int scelta) {
	list <Mesh>::iterator it;
	for (it = this->body.begin(); it != this->body.end(); ++it) {
		it->sceltaVS = scelta;
	}
}

void Cespuglio::translate(GLuint matModel, float x, float y, float z) {
	this->x += x;
	this->y += y;


	list <Mesh>::iterator it;
	for (it = this->body.begin(); it != this->body.end(); ++it) {
		printf("transfonrmo % %f \n", x, y);

		it->dx += x;
		it->dy += y;
		it->dz += z;
		printf("transfonrmo %f %f \n", it->dx, it->dy);

	}
}





#endif
