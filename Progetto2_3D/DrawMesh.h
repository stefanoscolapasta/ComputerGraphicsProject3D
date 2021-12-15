#pragma once

#include "Lib.h"
#include "TransformMesh.h"
#include "GestioneTesto.h"
#include FT_FREETYPE_H
#include <glm/glm.hpp>

#ifndef DRAW_MESH_H
#define DRAW_MESH_H

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void define_light_position_and_intensity(LightShaderUniform* light_unif, point_light* light, float angle) {
	glUniform3f(light_unif->light_position_pointer, light->position.x + 10 * cos(radians(angle)), light->position.y, light->position.z + 10 * sin(radians(angle)));
	glUniform3f(light_unif->light_color_pointer, light->color.r, light->color.g, light->color.b);
	glUniform1f(light_unif->light_power_pointer, light->power);
}


void define_light_mesh_material(Mesh* mesh, LightShaderUniform* light_unif) {
	glUniform3fv(light_unif->material_ambient, 1, glm::value_ptr(materials[mesh->material].ambient));
	glUniform3fv(light_unif->material_diffuse, 1, glm::value_ptr(materials[mesh->material].diffuse));
	glUniform3fv(light_unif->material_specular, 1, glm::value_ptr(materials[mesh->material].specular));
	glUniform1f(light_unif->material_shininess, materials[mesh->material].shininess);
}


void draw(Mesh* mesh, int typeDraw, GLuint matModel) {
	glBindVertexArray(mesh->VAO);
	modify(mesh, matModel);
	glUniformMatrix4fv(matModel, 1, GL_FALSE, value_ptr(mesh->Model));
	glDrawElements(typeDraw, ((*mesh).indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


#endif // !DRAW_MESH_H
