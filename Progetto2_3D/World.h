#pragma once

#ifndef WORLD_H
#define WORLD_H


static unsigned int MatrixProj, MatrixProj_txt, MatModel, MatView, loc_time, lsh, leye, lscelta, loc_view_pos;


class World {

public:
	float width;
	float height;

	World();

	

};


World::World() {
	this->width = 1024.0;
	this->height = 800.0;
}



#endif
