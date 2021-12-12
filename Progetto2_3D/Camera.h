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

void walkCameraForward() {
	//ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	vec4 dir = ViewSetup.direction;
	dir.y = 0;
	ViewSetup.position += dir * cameraSpeed;
}

void walkCameraBack() {
	//ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	vec4 dir = ViewSetup.direction;
	dir.y = 0;
	ViewSetup.position -= dir * cameraSpeed;
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

void SpecialInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		walkCameraForward();
		break;
	case GLUT_KEY_DOWN:
		walkCameraBack();
		break;
	case GLUT_KEY_LEFT:
		//do something here
		break;
	case GLUT_KEY_RIGHT:
		//do something here
		break;
	}
	glutPostRedisplay();
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
	ViewSetup.position = glm::vec4(0.0, 0.5, 25.0, 0.0);
	ViewSetup.target = glm::vec4(0.0, 0.0, 0.0, 0.0);
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.upVector = glm::vec4(0.0, 1.0, 0.0, 0.0);

	//Imposto la proiezione prospettica
	PerspectiveSetup = {};
	PerspectiveSetup.aspect = (GLfloat)width / (GLfloat)height;
	PerspectiveSetup.fovY = 45.0f;
	PerspectiveSetup.far_plane = 2000.0f;
	PerspectiveSetup.near_plane = 0.1f;
}



#endif

