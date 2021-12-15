#pragma once


#ifndef CESPUGLIO_H
#define CESPUGLIO_H

#include <list>
#include "Strutture.h"
#include "geometria.h"
#include "Color.h"
#include "VAO_And_Shader_Init.h"


class Cespuglio {

public:
	list<Mesh> cespuglio;
	Mesh foglia;


	vec4 cx;

	Cespuglio();
	//~Cespuglio();

	void build(GLuint matModel);
	void upload_VA0_VB0();
	vec4 get_center();

};

Cespuglio::Cespuglio() {
	this->cespuglio = {};
	this->foglia = {};
	this->cx = {};
	
}

void Cespuglio::build(GLuint matModel) {
	crea_sfera(&this->foglia, vec4(1.0, 1.0, 0.6, 1.0));
	crea_VAO_Vector(&this->foglia);
	this->foglia.Model = mat4(1.0);
	this->foglia.Model = translate(this->foglia.Model, vec3(0.0, 2.75, 0.0));
	this->foglia.Model = scale(this->foglia.Model, vec3(0.75, 0.75, 0.75));
	//this->foglia.Model = rotate(this->foglia.Model, radians(0.0f), vec3(0.0f, 0.0f, 1.0f));
	this->foglia.nome = "foglia";
	this->cx = this->foglia.Model * vec4(0.0, 0.0, 0.0, 1.0);



	//centri.push_back(cx);
	this->foglia.sceltaVS = 0;
	this->foglia.material = MaterialType::RED_PLASTIC;




	//Mesh agglomerato = {};
	//vec4 color = vec_RGBA(60.0f, 179.0f, 113.0f);
	//agglomerato.nome = "agglomerato";

	//crea_sfera(&agglomerato, color);
	//crea_VAO_Vector(&agglomerato);

	//set_transform_value(&agglomerato, 0.0f, 2.75f, 0.0f, 0.75f, 0.75f, 0.75f, 0.0f);
	//transfom(&agglomerato, matModel);

	//agglomerato.sceltaVS = 0;
	//agglomerato.material = MaterialType::RED_PLASTIC;

	//this->cespuglio.push_back(agglomerato);

	//this->cx = agglomerato.Model * vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void Cespuglio::upload_VA0_VB0() {

	list <Mesh>::iterator it;
	for (it = this->cespuglio.begin(); it != this->cespuglio.end(); ++it) {
		crea_VAO_Vector(&(*it));
	}

}


vec4 Cespuglio::get_center() {
	return this->cx;
}






#endif
