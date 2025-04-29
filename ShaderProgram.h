#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>

using namespace std;

class ShaderProgram {
public:
	GLuint ID;
	ShaderProgram(const char* vertexpath, const char* grafmentPath);
	void use();
	~ShaderProgram();
	void setVec3(const string& name, float x, float y, float z) const;
	void setVec3(const string& name, glm::vec3 xyz) const;
	void setFloat(const string& name, float value) const;
private:
	bool loadShaderSource(const char* path, string& shaderCode);
	GLuint compileShader(const char* shaderSource, GLenum shaderType);
};

