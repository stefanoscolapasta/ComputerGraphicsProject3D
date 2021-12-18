#pragma once

#include "Lib.h"

#ifndef MATERIALI_H
#define MATERIALI_H



glm::vec3 red_plastic_ambient = { 0.1, 0.0, 0.0 }, red_plastic_diffuse = { 0.6, 0.1, 0.1 }, red_plastic_specular = { 0.7, 0.6, 0.6 }; 
GLfloat red_plastic_shininess = 32.0f;
glm::vec3 brass_ambient = { 0.1, 0.06, 0.015 }, brass_diffuse = { 0.78, 0.57, 0.11 }, brass_specular = { 0.99, 0.91, 0.81 }; 
GLfloat brass_shininess = 27.8f;
glm::vec3 emerald_ambient = { 0.0215, 0.04745, 0.0215 }, emerald_diffuse = { 0.07568, 0.61424, 0.07568 }, emerald_specular = { 0.633, 0.727811, 0.633 };
GLfloat emerald_shininess = 78.8f;
glm::vec3 slate_ambient = { 0.02, 0.02, 0.02 }, slate_diffuse = { 0.1, 0.1, 0.1 }, slate_specular{ 0.4, 0.4, 0.4 };
GLfloat slate_shininess = 1.78125f;
glm::vec3 yellow_ambient = { 0.8,	0.8,	0.0 }, yellow_diffuse = { 0.5,0.5,0.4 }, yellow_specular{ 0.9,	0.9	,0.04 }; 
GLfloat yellow_shininess = 1.78125f;

glm::vec3 nuvola_ambient = { 0.012, 0.541, 1.0 }, nuvola_diffuse = { 0.07568, 0.61424, 0.07568 }, nuvola_specular = { 0.633, 0.727811, 0.633 };
GLfloat nuvola_shininess = 20.8f;



#endif // MATERIALI



