#include "fbo.h"

FBO::FBO(GLuint width, GLuint height) {
	this->width = width;
	glGetError();
	this->height = height;
	glGenFramebuffers(1, &rbHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, rbHandle);

	glGenRenderbuffers(1, &depthTextureHandle);
	glBindRenderbuffer(GL_RENDERBUFFER, depthTextureHandle);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTextureHandle);

	glGenTextures(1, &colorTextureHandle);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorTextureHandle);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, width, height, true);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorTextureHandle, 0);//This entire thing is a horrible mess and I don't know what it does

	glGenFramebuffers(1, &handle);
	glBindFramebuffer(GL_FRAMEBUFFER, handle);

	glGenTextures(1, &resolveHandle);
	glBindTexture(GL_TEXTURE_2D, resolveHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resolveHandle, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "framebuffer go whoops" << endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FBO::~FBO() {
	glDeleteTextures(1, &colorTextureHandle);
	glDeleteTextures(1, &depthTextureHandle);
	glDeleteFramebuffers(1, &handle);
}