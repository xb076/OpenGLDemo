

#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"




Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}


ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (std::string::npos != line.find("#shader"))
		{
			if (std::string::npos != line.find("vertex"))
			{
				type = ShaderType::VERTEX;
			}
			else if (std::string::npos != line.find("fragment"))
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else if (ShaderType::NONE != type)
		{
			ss[(int)type] << line << "\n";
		}
	}

	return{ ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result = GL_FALSE;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (GL_FALSE == result)
	{
		int length = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to complile " << \
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader!" << "\n";
		std::cout << message << "\n";
		glDeleteShader(id);

		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::UnBind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int v0)
{
	GLCall(glUniform1i(GetUniformLocation(name), v0));
}

void Shader::SetUniform1f(const std::string& name, float v0)
{
	GLCall(glUniform1f(GetUniformLocation(name), v0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

unsigned int Shader::GetUniformLocation(const std::string& name)
{
	int location = -1;
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
		location = m_UniformLocationCache[name];

	} else {
		GLCall(location = glGetUniformLocation(m_RendererID, name.c_str()));
		if (-1 == location)
			std::cout << "Warning: uniform '" << name << "' doesn't exist!\n";

		m_UniformLocationCache[name] = location;
	}
	
	return location;
}
