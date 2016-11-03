#pragma once
#ifndef __Texture__
#define __Texture__

#define GLEW_STATIC
#include <GL/glew.h>

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

#include <string>
#include <memory>
#include "Shape.h"
#include "Program.h"
#include "MatrixStack.h"

class TextureCube
{
public:
	TextureCube();
	virtual ~TextureCube();
	void setFilenames(const std::string &negx,
					  const std::string &posx,
					  const std::string &negy,
					  const std::string &posy,
					  const std::string &negz,
					  const std::string &posz);
	void init();
	void setUnit(GLint u) { unit = u; }
	GLint getUnit() const { return unit; }
	GLuint getFBO() const { return fbid; }
	void bind(GLint handle);
	void unbind();
	//void bindFBO(GLint handle);
	//void drawToFace(std::shared_ptr<MatrixStack> &M, std::shared_ptr<MatrixStack> &P, int side);
	
private:
	std::string filenames[6];
	int width;
	int height;
	GLuint tid;
	GLuint fbid;
	GLuint rendid;
	GLint unit;
	
};

#endif
