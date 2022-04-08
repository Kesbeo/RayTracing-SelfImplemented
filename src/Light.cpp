#include "Light.h"

glm::vec3 Light::ambient = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3 Light::attenuation = glm::vec3(1.0f, 0.0f, 0.0f);

////////////////////////////////////////////////////////////////////////////////
DirectionalLight::DirectionalLight(const glm::vec3& direction, const glm::vec3& color)
{
	this->direction = direction;
	this->color = color;
}
////////////////////////////////////////////////////////////////////////////////
DirectionalLight::~DirectionalLight()
{

}
////////////////////////////////////////////////////////////////////////////////
PointLight::PointLight(const glm::vec3& position, const glm::vec3& color)
{
	this->position = position;
	this->color = color;
}
////////////////////////////////////////////////////////////////////////////////
PointLight::~PointLight()
{

}