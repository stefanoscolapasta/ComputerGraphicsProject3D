#pragma once

#include "Lib.h"
#include "TransformMesh.h"
#include "GestioneTesto.h"
using namespace glm;

#ifndef DRAW_MESH_H
#define DRAW_MESH_H

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void define_light_position_and_intensity(LightShaderUniform* light_unif, point_light* light, float angle) {
	glUniform3f(light_unif->light_position_pointer, light->position.x + 10 * cos(radians(angle)), light->position.y, light->position.z + 10 * sin(radians(angle)));
	glUniform3f(light_unif->light_color_pointer, light->color.r, light->color.g, light->color.b);
	glUniform1f(light_unif->light_power_pointer, light->power);
}




void draw_mesh(Mesh* mesh, int typeDraw, GLuint matModel) {
	glUniform1i(lscelta, mesh->sceltaVS);
	glBindVertexArray(mesh->VAO);
	modify(mesh, matModel);
	glDrawElements(GL_TRIANGLES, (mesh->indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void draw_mesh_kel(Mesh* mesh, int typeDraw, GLuint matModel) {
	glUniform1i(lscelta, 0);
	glBindVertexArray(mesh->VAO);
	//modify(mesh, matModel);
	glDrawElements(GL_TRIANGLES, (mesh->indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


#endif // !DRAW_MESH_H
