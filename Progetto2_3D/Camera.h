#pragma once

#include "Lib.h"
#include "Strutture.h"


#ifndef CAMERA_H
#define CAMERA_H

float cameraSpeed = 0.05;

void moveCameraForward()
{
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.position += ViewSetup.direction * cameraSpeed;

}

void moveCameraBack()
{
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.position -= ViewSetup.direction * cameraSpeed;

}

void moveCameraLeft()
{
	//Calcolo la direzione perpendicolare alla direzione della camera e l'alto della camera
		// e muovo la camera a sinistra lungo questa direzione
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	vec3 direzione_scorrimento = glm::cross(vec3(ViewSetup.direction), glm::vec3(ViewSetup.upVector));
	ViewSetup.position -= vec4((direzione_scorrimento), .0) * cameraSpeed;
	ViewSetup.target -= vec4((direzione_scorrimento), .0) * cameraSpeed;
}

void moveCameraRight()
{
	//Calcolo la direzione perpendicolare alla direzione della camera e l'alto della camera
	// e muovo la camera a destra lungo questa direzione
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	vec3 direzione_scorrimento = glm::cross(vec3(ViewSetup.direction), glm::vec3(ViewSetup.upVector)) * cameraSpeed;
	ViewSetup.position += vec4(direzione_scorrimento, 0);
	ViewSetup.target += vec4(direzione_scorrimento, 0);
}


void moveCamera(unsigned char key) {
	switch (key) {
		case 'a': moveCameraLeft(); break;
		case 'd': moveCameraRight(); break;
		case 'w': moveCameraForward(); break;
		case 's': moveCameraBack(); break;
	}
}


void INIT_CAMERA_PROJECTION(float width, float height) {
	//Imposto la telecamera
	ViewSetup = {};
	ViewSetup.position = glm::vec4(0.0, 0.5, 80.0, 0.0);
	ViewSetup.target = glm::vec4(0.0, 0.0, 0.0, 0.0);
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.upVector = glm::vec4(0.0, 1.0, 0.0, 0.0);

	//Imposto la proiezione prospettica
	PerspectiveSetup = {};
	PerspectiveSetup.aspect = (GLfloat)width / (GLfloat)height;
	PerspectiveSetup.fovY = 60.0f;
	PerspectiveSetup.far_plane = 2000.0f;
	PerspectiveSetup.near_plane = 0.1f;
}



#endif

