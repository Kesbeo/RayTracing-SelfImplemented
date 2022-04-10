#pragma once
#include "Raytracer.h"

int Raytracer::depth;

////////////////////////////////////////////////////////////////////////////////
void Raytracer::tracer(Camera* cam, const std::vector<Object *>& objList, const std::vector<Light*>& lightList)
{
	int width = Camera::w;
	int height = Camera::h;

	float halfwidth = width / 2;
	float halfheight = height / 2;

	//generating ray
	for (int j = 0; j < height; j++) {
		std::cerr << "\rRow Remaining: " << height - j << std::flush;
		for (int i = 0; i < width; i++) {
			//int progress = (int)((j * width + i) / (width * height) * 100);
			//std::cerr << "\rRayTracing Progress [" << std::string(progress, '#') << std::string(100-progress, ' ') << "]" << std::flush;

			//calculating ratios
			float xRatio = (i + 0.5 - halfwidth)/halfwidth;
			float yRatio = (j + 0.5 - halfheight)/halfheight;
			
			// ray direction
			glm::vec3 dir = cam->front * cam->viewZ +
				cam->down * cam->viewY * yRatio+
				cam->right * cam->viewX * xRatio;
			dir = glm::normalize(dir);

			//generating ray
			Ray currRay(cam->eye, dir);
			//get color 
			glm::vec3 pixelColor = getColor(&currRay, objList, lightList);
			//std::cerr << pixelColor.x << " , " << pixelColor.y << " , " << pixelColor.z << "\n";
			cam->pixels[3 * (j * width + i)] = unsigned char((int)(pixelColor.z * 255));
			cam->pixels[3 * (j * width + i) + 1] = unsigned char((int)(pixelColor.y * 255));
			cam->pixels[3 * (j * width + i) + 2] = unsigned char((int)(pixelColor.x * 255));
		}
	}

}
////////////////////////////////////////////////////////////////////////////////
glm::vec3 Raytracer::getColor(Ray* ray, const std::vector<Object*>& objList, const std::vector<Light*>& lightList)
{
	float currHitMin = INFINITY;
	Object* currHit;
	//loop through all objects
	for (int i = 0; i < objList.size(); i++) {
		Object* currObj = objList[i];
		//when intersect with object
		if (currObj->checkIntersect(ray)) {
			//hittime < hit min
			float currHitTime = ray->getHitTime();
			//std::cerr << currHitTime << "\n";
			if (currHitTime > 0 && currHitTime < currHitMin) {
				currHitMin = currHitTime;//update hit time
				currHit = currObj;
			}
		}
	}
	if (currHitMin < INFINITY)
	{
		//std::cerr << "Hit!\n";
		glm::vec3 color = currHit->ambient + currHit->emission;
		for (auto light : lightList)
		{
			// compute diffuse
			if (light->type == LightType::point)
			{
				PointLight* point = (PointLight*)light;
				glm::vec3 hitPoint = ray->getHitPoint() + ray->getHitNormal() * glm::vec3(INTERSECT_EPSILON);
				glm::vec3 L = glm::normalize(point->position - hitPoint);
				Ray shadowRay(ray->getHitPoint() + ray->getHitNormal() * glm::vec3(INTERSECT_EPSILON), L);
				for (auto obj : objList)
				{
					if (obj->checkIntersect(&shadowRay))
					{
						if(shadowRay.getHitTime() > 0 && shadowRay.getHitTime() < INFINITY)
							return glm::vec3(0.2f);
					}
				}

				float r = glm::length(L);
				glm::vec3 r_vec = glm::vec3(1.0f, r, r * r);
				float attenFactor = glm::dot(Light::attenuation, r_vec);

				glm::vec3 diffuse_reflectance = currHit->diffuse * std::max(glm::dot(ray->getHitNormal(), L), 0.0f);
				color += light->color * diffuse_reflectance / attenFactor;
			}
			else if (light->type == LightType::directional)
			{
				DirectionalLight* directional = (DirectionalLight*)light;
				Ray shadowRay(ray->getHitPoint() + glm::vec3(INTERSECT_EPSILON), normalize(directional->direction));
				for (auto obj : objList)
				{
					if (obj->checkIntersect(&shadowRay))
					{
						if (shadowRay.getHitTime() > 0 && shadowRay.getHitTime() < INFINITY)
							return glm::vec3(0.2f);
					}
				}
				glm::vec3 diffuse_reflectance = currHit->diffuse * std::max(glm::dot(ray->getHitNormal(), -normalize(directional->direction)), 0.0f);
				color += light->color * diffuse_reflectance;
			}
		}
		return color;
	}
	else
		return glm::vec3(0.0f);
}