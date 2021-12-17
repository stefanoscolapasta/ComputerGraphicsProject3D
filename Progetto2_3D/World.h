#pragma once
#include <list>

#ifndef WORLD_H
#define WORLD_H

static unsigned int MatrixProj, MatrixProj_txt, MatModel, MatView, loc_time, lsh, leye, lscelta, loc_view_pos;

class World {

public:
	float width;
	float height;
	float w_up;
	float h_up;
	list<Mesh> scena;

	World();
};


World::World() {
	this->width = 1024.0;
	this->height = 800.0;
	this->w_up = this->width;
	this->h_up = this->height;

	this->scena = {};
}



#endif
