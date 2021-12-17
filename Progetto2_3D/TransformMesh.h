#pragma once

#include "Strutture.h"
#include "Lib.h"

#ifndef TRANSFORM_MESH_H
#define TRANSFORM_MESH_H

void increment_move(Mesh* mesh, float dx, float dy, float dz) {
	mesh->dx += dx;
	mesh->dy += dy;
	mesh->dz += dz;
}

void set_move_value(Mesh* mesh, float dx, float dy, float dz) {
	mesh->dx = dx;
	mesh->dy = dy;
	mesh->dz = dz;
}


void set_scale_value(Mesh* mesh, float scalex, float scaley, float scalez) {
	mesh->scalex = scalex;
	mesh->scaley = scaley;
	mesh->scalez = scalez;
}

void set_rotate_value(Mesh* mesh, float rx, float ry, float rz, float angle) {
	mesh->angle = angle;
	mesh->rx = rx;
	mesh->ry = ry;
	mesh->rz = rz;
}

void set_transform_values(Mesh* mesh, float dx, float dy, float dz, 
	float scalex, float scaley, float scalez, 
	float rx, float ry, float rz, float angle) {
	set_move_value(mesh, dx, dy, dz);
	set_scale_value(mesh, scalex, scaley, scalez);
	set_rotate_value(mesh, rx, ry, rz, angle);
}

void modify(Mesh* mesh, GLuint matModel) {
	mesh->Model = mat4(1.0);
	mesh->Model = translate(mesh->Model, vec3(mesh->dx, mesh->dy, mesh->dz));
	mesh->Model = scale(mesh->Model, vec3(mesh->scalex, mesh->scaley, mesh->scalez));
	mesh->Model = rotate(mesh->Model, radians(mesh->angle), vec3(mesh->rx, mesh->ry, mesh->rz));
}



#endif // !TRANSFORM_H

