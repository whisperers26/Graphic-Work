#pragma once
#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include "ShaderProgram.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>
using namespace glm;

// This class define a particle system composed of a set of randomly-moving particles
// The particle data are maintained on the GPU using shader storage buffer objects
// It can be used by a compute shader

class ParticleSystem
{
public:
	unsigned int num;	// the number of particle
	vec3 size_min_point; 
	vec3 size_max_point;

	GLuint vao;			// render the particles with vertex attributes within the shaders
						// a VAO (vertex array objects) allows to set data to attributes (rather than uniform variables)

	// the ssbo ids of those particle's pos, dir, speed on GPU
	GLuint pos_ssbo;
	GLuint dir_ssbo;
	GLuint speed_ssbo;
	GLuint color_ssbo;
	
private:
	vec4* pos_array;	// positions (x, y, z, w) of particles, w = 1, which is mapped to pos_ssbo
	vec4* dir_array;	// moving directions (x, y, z, w) of particles, w = 0, which his mapped to dir_ssbo 
	float* speed_array;	// the moving speed of particles, which is mapped to speed_ssbo 
	vec4* color_array;	// colors (r, g, b, a) of particles, a = 1, which is mapped to color_ssbo
	
	// note that the vert and frag shaders cannot be combined with a compute shader in the same shader programs.
	// that' why we create two shader programs = vert and frag shaders are for rendering, and comp shader is for computing only
	ShaderProgram cShaderProg; // shader program for the computer shader 
	ShaderProgram vfShaderProg;  // shader program for the vertex and fragment shaders

	ShaderClass cShader; // computer shader for general-purpose computing
	ShaderClass vShader; // vertex shader
	ShaderClass fShader; // fragment shader

	float randomf(float min = -1.0f, float max = 1.0f);

public:
	ParticleSystem();
	~ParticleSystem();
	void create(unsigned int particle_width, unsigned int particle_height, vec3 min_point, vec3 max_point,
				const char* compute_shader_file, const char* vertex_shader_file, const char* fragment_shader_file);
	void update(vec3 camPos, vec3 spherePos, float radius);  // invoke the compute shader in the update ()
	void draw(float particle_size, mat4 view_mat, mat4 proj_mat);    // invoke the vert and frag shaders in the draw ()
	void sphereIntersect(vec3 camPos, vec3 spherePos, float radius);
};