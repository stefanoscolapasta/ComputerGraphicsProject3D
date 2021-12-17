#pragma once

#include "Lib.h"
#include <string>

#ifndef STRUTTURE_H
#define STRUTTURE_H

//Strutture luci e materiali

typedef enum {
	RED_PLASTIC,
	EMERALD,
	BRASS,
	SLATE,
	YELLOW,
	NO_MATERIAL
} MaterialType;

typedef struct {
	string name;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat shininess;
} Material;

typedef enum {
	NONE,
	GOURAD_SHADING,
	PHONG_SHADING,
	ONDE_SHADING,
	BANDIERA_SHADING,
} ShaderOption;

typedef struct {
	string name;
	int value;
} Shader;


typedef struct {
	glm::vec3 position;
	glm::vec3 color;
	GLfloat power;
} point_light;

typedef struct {
	GLuint light_position_pointer;
	GLuint light_color_pointer;
	GLuint light_power_pointer;
	GLuint material_diffuse;
	GLuint material_ambient;
	GLuint material_specular;
	GLuint material_shininess;
} LightShaderUniform;

typedef enum {
	GOURAUD,
	PHONG,
	BLINN,
	TOON,
	PASS_THROUGH,
	WAVE
} ShadingType;


typedef struct {
	vector<vec3> vertici;
	vector<vec4> colori;
	vector <GLuint>  indici;
	vector<vec3> normali;
	vector<vec2> texCoords;
	mat4 Model;
	int sceltaVS;
	int sceltaFS;
	float initialScaleMultiplier;
	ShadingType shading;
	MaterialType material;
	GLuint VAO;
	GLuint VBO_G;
	GLuint VBO_C;
	GLuint VBO_normali;
	GLuint EBO_indici;
	GLuint VBO_coord_texture;
	string nome;

	float dx, dy, dz;
	float scalex, scaley, scalez;
	float angle;
	float rx, ry, rz;
} Mesh;

static vector<Mesh*> Scena;

static vector<vec3> centri;
static vector<float> raggi;

struct {
	vec4 position;
	vec4 target;
	vec4 upVector;
	vec4 direction;
} ViewSetup;

struct {
	float fovY, aspect, near_plane, far_plane;
} PerspectiveSetup;

struct Character {
	unsigned int TextureID; // ID handle of the glyph texture
	glm::ivec2   Size;      // Size of glyph
	glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};

typedef struct {
	vec3 position;
	int direction;
	float velocity;
	float rotation;
} Car;

typedef struct {
	float width;
	vec3 position;
} Road;

#endif // !STRUTTURE_H

