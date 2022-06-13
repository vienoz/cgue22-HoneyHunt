/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#include "Material.h"

/* --------------------------------------------- */
// Base material
/* --------------------------------------------- */

BaseMaterial::BaseMaterial(std::shared_ptr<ShaderNew> shader)
	: _shader(shader)
{
}

BaseMaterial::~BaseMaterial()
{
}

std::shared_ptr<ShaderNew> BaseMaterial::getShader()
{
	return _shader;
}

void BaseMaterial::setUniforms(glm::mat4 modelMatrix, Camera& camera, DirectionalLight& dirL)
{
	_shader->setUniform(0, modelMatrix);
	_shader->setUniform(1, camera.getViewMatrix());
	_shader->setUniform(2, camera.getProjMatrix());
	_shader->setUniform(3, camera.getPosition());

	_shader->setUniform(4, dirL.color);
	_shader->setUniform(5, dirL.direction);
}

/* --------------------------------------------- */
// Texture material
/* --------------------------------------------- */

TextureMaterial::TextureMaterial(std::shared_ptr<ShaderNew> shader, std::shared_ptr<Texture> diffuseTexture)
	: BaseMaterial(shader), _diffuseTexture(diffuseTexture)
{
}

TextureMaterial::~TextureMaterial()
{
}

void TextureMaterial::setUniforms(glm::mat4 modelMatrix, Camera& camera, DirectionalLight& dirL)
{
	BaseMaterial::setUniforms(modelMatrix, camera, dirL);

	_diffuseTexture->bind(0);
}

std::shared_ptr<Texture> TextureMaterial::getTexture() {
	return _diffuseTexture;
}

/* --------------------------------------------- */
// Cel-shaded material
/* --------------------------------------------- */

CelShadedMaterial::CelShadedMaterial(std::shared_ptr<ShaderNew> shader, std::shared_ptr<Texture> diffuseTexture, glm::vec3 materialCoefficients, float alpha)
	: TextureMaterial(shader, diffuseTexture), _materialCoefficients(materialCoefficients), _alpha(alpha)
{
}

CelShadedMaterial::~CelShadedMaterial()
{
}

void CelShadedMaterial::setUniforms(glm::mat4 modelMatrix, Camera& camera, DirectionalLight& dirL)
{
	TextureMaterial::setUniforms(modelMatrix, camera, dirL);

	_shader->setUniform(6, _materialCoefficients);
	_shader->setUniform(7, _alpha);
}