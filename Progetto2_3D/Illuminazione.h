#pragma once
#include <iostream>

#include "Materiali.h"
#include "Lib.h"
#include "Strutture.h"

#ifndef ILLUMINAZIONE_H
#define ILLUMINAZIONE_H

extern Road road;

vector<vec3> lampioniAndLightsPosition = { 
	vec3(road.position.x + 5.0f * road.width, 0.0f, road.width + road.position.z),
	vec3(road.position.x + 9.0f * road.width, 0.0f, 2.0f*road.width + road.position.z),
	vec3(road.position.x + 4.0f * road.width, 0.0f, 4.0f*road.width + road.position.z),
	vec3(road.position.x + 8.0f * road.width, 0.0f, 6.0f*road.width + road.position.z)
};

void INIT_Illuminazione(point_light lights[], vector<Material>& materials, vector<Shader>& shaders) {
	/* luce sole */
	lights[0].position = { 0.0,5.0,0.0 };
	lights[0].color = { 1.0, 1.0, 0 };
	lights[0].power = 1.5f;

	for (int i = 1; i < lampioniAndLightsPosition.size(); i++) {
		/* luce lampione */
		lights[i].position = lampioniAndLightsPosition[i] + vec3(0,7.5,0);
		lights[i].color = { 1.0,1.0,1.0 };
		lights[i].power = .7f;
	}



	materials.resize(7);

	materials[MaterialType::RED_PLASTIC].name = "Red Plastic";
	materials[MaterialType::RED_PLASTIC].ambient = red_plastic_ambient;
	materials[MaterialType::RED_PLASTIC].diffuse = red_plastic_diffuse;
	materials[MaterialType::RED_PLASTIC].specular = red_plastic_specular;
	materials[MaterialType::RED_PLASTIC].shininess = red_plastic_shininess;

	materials[MaterialType::EMERALD].name = "Emerald";
	materials[MaterialType::EMERALD].ambient = emerald_ambient;
	materials[MaterialType::EMERALD].diffuse = emerald_diffuse;
	materials[MaterialType::EMERALD].specular = emerald_specular;
	materials[MaterialType::EMERALD].shininess = emerald_shininess;

	materials[MaterialType::BRASS].name = "Brass";
	materials[MaterialType::BRASS].ambient = brass_ambient;
	materials[MaterialType::BRASS].diffuse = brass_diffuse;
	materials[MaterialType::BRASS].specular = brass_specular;
	materials[MaterialType::BRASS].shininess = brass_shininess;

	materials[MaterialType::SLATE].name = "Slate";
	materials[MaterialType::SLATE].ambient = slate_ambient;
	materials[MaterialType::SLATE].diffuse = slate_diffuse;
	materials[MaterialType::SLATE].specular = slate_specular;
	materials[MaterialType::SLATE].shininess = slate_shininess;

	materials[MaterialType::YELLOW].name = "Yellow";
	materials[MaterialType::YELLOW].ambient = yellow_ambient;
	materials[MaterialType::YELLOW].diffuse = yellow_diffuse;
	materials[MaterialType::YELLOW].specular = yellow_specular;
	materials[MaterialType::YELLOW].shininess = yellow_shininess;

	materials[MaterialType::NO_MATERIAL].name = "NO_MATERIAL";
	materials[MaterialType::NO_MATERIAL].ambient = glm::vec3(1, 1, 1);
	materials[MaterialType::NO_MATERIAL].diffuse = glm::vec3(0, 0, 0);
	materials[MaterialType::NO_MATERIAL].specular = glm::vec3(0, 0, 0);
	materials[MaterialType::NO_MATERIAL].shininess = 1.f;

	// 0.02	0.02	0.02	0.01	0.01	0.01	0.4	0.4	0.4	.078125
	materials[MaterialType::BLACK_RUBBER].name = "BLACK_RUBBER";
	materials[MaterialType::BLACK_RUBBER].ambient = glm::vec3(0.02, 0.02, 0.02);
	materials[MaterialType::BLACK_RUBBER].diffuse = glm::vec3(0.02, 0.01, 0.01);
	materials[MaterialType::BLACK_RUBBER].specular = glm::vec3(0.4, 0.4, 0.4);
	materials[MaterialType::BLACK_RUBBER].shininess = .078125f;

	//Setup degli shader
	shaders.resize(5);
	shaders[ShaderOption::NONE].value = 0;
	shaders[ShaderOption::NONE].name = "NONE";
	shaders[ShaderOption::GOURAD_SHADING].value = 1;
	shaders[ShaderOption::GOURAD_SHADING].name = "GOURAD SHADING";
	shaders[ShaderOption::PHONG_SHADING].value = 2;
	shaders[ShaderOption::PHONG_SHADING].name = "PHONG SHADING";
	shaders[ShaderOption::ONDE_SHADING].value = 3;
	shaders[ShaderOption::ONDE_SHADING].name = "ONDE SHADING";
	shaders[ShaderOption::BANDIERA_SHADING].value = 4;
	shaders[ShaderOption::BANDIERA_SHADING].name = "BANDIERA SHADING";
}


#endif