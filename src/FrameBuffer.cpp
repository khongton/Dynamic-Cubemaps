#include "FrameBuffer.h"

using namespace std;
using namespace Eigen;

GLenum targets[] = {
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
};

FrameBuffer::FrameBuffer() :
fboID(0),
renderID(0),
textureID(0)
{
}

FrameBuffer::~FrameBuffer()
{
}

void FrameBuffer::checkFBO() {
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cerr << "FrameBuffer isn't ok" << endl;
}

void FrameBuffer::init() {
	//Create cubemap textures	//Generate FBO
	glGenFramebuffers(1, &fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);	
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (int i = 0; i < 6; i++)
		glTexImage2D(targets[i], 0, GL_RGB, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Create uniform depth buffer for z-test purposes
	glGenRenderbuffers(1, &renderID);
	glBindRenderbuffer(GL_RENDERBUFFER, renderID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 2048, 2048);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//attach FBO and -X texture
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fboID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, targets[0], textureID, 0);

	checkFBO();
}

void FrameBuffer::drawToSide(shared_ptr<MatrixStack> &V, shared_ptr<MatrixStack> &P, int side) {
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, fboID, 0);
	checkFBO();

	//Reset perspective and view matrices
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	P->loadIdentity();
	//90 degree POV gives us a square viewport for the cubeface
	P->perspective(90, 1, 1, 1000);
	V->loadIdentity();

	//up vectors are flipped to get correct image orientation
	switch (GL_TEXTURE_CUBE_MAP_POSITIVE_X + side) {
	case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		V->lookAt(Vector3f(0, 0, 0), Vector3f(10, 0, 0), Vector3f(0, -1, 0));
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
		V->lookAt(Vector3f(0, 0, 0), Vector3f(-10, 0, 0), Vector3f(0, -1, 0));
		break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		V->lookAt(Vector3f(0, 0, 0), Vector3f(0, 10, 0), Vector3f(0, 0, 1));
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		V->lookAt(Vector3f(0, 0, 0), Vector3f(0, -10, 0), Vector3f(0, 0, -1));
		break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
		V->lookAt(Vector3f(0, 0, 0), Vector3f(0, 0, 10), Vector3f(0, -1, 0));
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
		V->lookAt(Vector3f(0, 0, 0), Vector3f(0, 0, -10), Vector3f(0, -1, 0));
		break;
	default:
		break;
	}
}

void FrameBuffer::bindFBO(GLuint handle, float width, float height) {
	glBindFramebuffer(GL_FRAMEBUFFER, handle);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}