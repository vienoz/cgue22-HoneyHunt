/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "ShaderNew.h"
#include "Texture.h"
#include "Camera.h"
#include "Light.h"


/*!
 * Base material
 */
class BaseMaterial
{
protected:
	/*!
	 * The shader used for rendering this material
	 */
	std::shared_ptr<ShaderNew> _shader;

public:
	/*!
	 * Base material constructor
	 * @param shader: The shader used for rendering this material
	 */
	BaseMaterial(std::shared_ptr<ShaderNew> shader);

	virtual ~BaseMaterial();

	/*!
	 * @return The shader associated with this material
	 */
	std::shared_ptr<ShaderNew> getShader();

	/*!
	 * Sets this material's parameters as uniforms in the shader
	 */
	virtual void setUniforms(glm::mat4 modelMatrix, Camera& camera, DirectionalLight& dirL);
};


/*!
 * Texture material
 */
class TextureMaterial : public BaseMaterial
{
protected:
	/*!
	 * The diffuse texture of this material
	 */
	std::shared_ptr<Texture> _diffuseTexture;

public:
	/*!
	 * Texture material constructor
	 * @param shader: The shader used for rendering this material
	 * @param diffuseTexture: The diffuse texture of this material
	 */
	TextureMaterial(std::shared_ptr<ShaderNew> shader, std::shared_ptr<Texture> diffuseTexture);

	virtual ~TextureMaterial();

	/*!
	 * Set's this material's parameters as uniforms in the shader
	 */
	virtual void setUniforms(glm::mat4 modelMatrix, Camera& camera, DirectionalLight& dirL);
	virtual std::shared_ptr<Texture> getTexture();
};

/*
 * Cel shaded material
 */
class CelShadedMaterial : public TextureMaterial
{
protected:
	/*!
	 * The material's coefficients (x = ambient, y = diffuse, z = specular)
	 */
	glm::vec3 _materialCoefficients;

	/*!
	 * Alpha value, i.e. the shininess constant
	 */
	float _alpha;

public:
	CelShadedMaterial(std::shared_ptr<ShaderNew> shader, std::shared_ptr<Texture> diffuseTexture, glm::vec3 materialCoefficients, float alpha);

	virtual ~CelShadedMaterial();

	/*!
	 * Set's this material's parameters as uniforms in the shader
	 */
	virtual void setUniforms(glm::mat4 modelMatrix, Camera& camera, DirectionalLight& dirL);
};

class OutlineShadedMaterial : public BaseMaterial
{
protected:
	std::shared_ptr<Texture> _fragColor;

public:
	/*!
	 * Texture material constructor
	 * @param shader: The shader used for rendering this material
	 * @param diffuseTexture: The diffuse texture of this material
	 */
	OutlineShadedMaterial(std::shared_ptr<ShaderNew> shader);

	virtual ~OutlineShadedMaterial();

	/*!
	 * Set's this material's parameters as uniforms in the shader
	 */
	virtual void setUniforms(glm::vec4& texture);
};