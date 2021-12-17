#pragma once

#include "Lib.h"
#include "Strutture.h"


#ifndef CAMERA_H
#define CAMERA_H

float cameraSpeed = 0.05;

typedef struct {
	float deltaXOffset;
	float offsetXMassimo;
	vec3 startingCameraPos;
}CameraMovementHandler;

CameraMovementHandler cameraMovementHandler = { 0.2f, 20.0f, ViewSetup.position };

vec3 calculateObjectVerticesBarycenter(Mesh* obj) {
	float sumX = 0.0f;
	float sumY = 0.0f;
	float sumZ = 0.0f;

	for (int i = 0; i < obj->vertici.size(); i++) {
		sumX += obj->vertici[i].x;
		sumY += obj->vertici[i].y;
		sumZ += obj->vertici[i].z;
	}

	int numberOfVertices = obj->vertici.size();

	return vec3(sumX / ((float)numberOfVertices), sumY / ((float)numberOfVertices), sumZ / ((float)numberOfVertices));

}

bool checkIfCameraReachedFixedBoundary() {
	return ViewSetup.position.x > cameraMovementHandler.startingCameraPos.x + cameraMovementHandler.offsetXMassimo ||
		ViewSetup.position.x < cameraMovementHandler.startingCameraPos.x - cameraMovementHandler.offsetXMassimo;
}

void cameraMovementUpdateHandler(Mesh* target) {
	if (checkIfCameraReachedFixedBoundary()) {
		cameraMovementHandler.deltaXOffset = -cameraMovementHandler.deltaXOffset;
	}
	ViewSetup.position.x += cameraMovementHandler.deltaXOffset;
}


void makeCameraTrackObject(Mesh* objToFollow) {
	ViewSetup.target = vec4(calculateObjectVerticesBarycenter(objToFollow), 0.0f);
}

void makeCameraFollowObject(Mesh* objToFollow, vec3 offsetToPos) {
	ViewSetup.position = vec4(calculateObjectVerticesBarycenter(objToFollow) + offsetToPos, 0.0f);
}

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

