#pragma once

#include "Lib.h"


#ifndef MENU_H
#define MENU_H

extern int selected_obj;
extern vector<Shader> shaders;

typedef enum {
	WIRE_FRAME,
	FACE_FILL,
	CULLING_ON,
	CULLING_OFF,
}MenuOption;


void main_menu_func(int option) {
	switch (option) {
	case MenuOption::WIRE_FRAME: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case MenuOption::FACE_FILL: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;

	default: break;
	}
}



void material_menu_function(int option)
{
	if (selected_obj > -1)
		Scena[selected_obj]->material = (MaterialType)option;
}

void shader_menu_function(int option) {
	if (selected_obj > -1)
		Scena[selected_obj]->sceltaVS = shaders[option].value;

}



#endif