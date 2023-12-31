#include "Scene.h"
#include "Canvas.h"
#include "MathUtils.h"
#include "Random.h"
#include <iomanip>
#include <iostream>
#include <chrono>


void Scene::Render(Canvas& canvas, int numSamples, int depth)
{
	std::chrono::steady_clock::time_point beginTime = std::chrono::steady_clock::now();

	std::chrono::seconds totalTimeElapsed{ 0 };
	std::chrono::seconds averageIterationTime{ 0 };
	int totalIterations = canvas.GetSize().x * canvas.GetSize().y;

	// cast ray for each point (pixel) on the canvas
	for (int y = 0; y < canvas.GetSize().y; y++)
	{

		for (int x = 0; x < canvas.GetSize().x; x++)
		{
			// create vec2 pixel from canvas x,y
			glm::vec2 pixel = glm::vec2{ x, y };

			// set initial color
			color3_t color{ 0 };
			// cast a ray for each sample, accumulate color value for each sample
			// each ray will have a random offset
			for (int sample = 0; sample < numSamples; sample++)
			{
				// get normalized (0 - 1) point coordinates from pixel
				// add random x and y offset (0-1) to each pixel
				glm::vec2 point = (pixel + glm::vec2{ random01(), random01() }) / glm::vec2{ canvas.GetSize().x, canvas.GetSize().y };
				// flip y
				point.y = 1.0f - point.y;

				// create ray from camera
				ray_t ray = m_camera->GetRay(point);

				// cast ray into scene
				// add color value from trace
				raycastHit_t raycastHit;
				color += Trace(ray, 0, 100, raycastHit, depth);
			}

			// draw color to canvas point (pixel)
			// get average color (average = (color + color + color) / number of samples)
			color /= numSamples;
			canvas.DrawPoint(pixel, color4_t(color, 1));

			std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
			totalTimeElapsed = std::chrono::duration_cast<std::chrono::seconds>(endTime - beginTime);

			int currentIteration = (y + 1) * canvas.GetSize().x;
			double progress = static_cast<double>(currentIteration) / totalIterations;

			std::chrono::seconds projectedTimeLeft = std::chrono::seconds(static_cast<long long>((1.0 - progress) * (totalTimeElapsed.count() / progress)));

			int hours = projectedTimeLeft.count() / 3600;
			int minutes = (projectedTimeLeft.count() % 3600) / 60;
			int seconds = projectedTimeLeft.count() % 60;

			// Output progress and projected time left in hh:mm:ss format after each y iteration
			std::cout << "Progress: " << std::setprecision(2) << (progress * 100) << "%"
				<< " - Projected time left: " << std::setfill('0') << std::setw(2) << hours << ":"
				<< std::setfill('0') << std::setw(2) << minutes << ":"
				<< std::setfill('0') << std::setw(2) << seconds 
				<< " - Total rendering time: " << totalTimeElapsed.count() << " seconds\n";
			
		}

	}
}

color3_t Scene::Trace(const ray_t& ray, float minDistance, float maxDistance, raycastHit_t& raycastHit, int depth)
{
	bool rayHit = false;
	float closestDistance = maxDistance;

	// check if scene objects are hit by the ray
	for (auto& object : m_objects)
	{
		// when checking objects don't include objects farther than closest hit (starts at max distance)
		if (object->Hit(ray, minDistance, closestDistance, raycastHit))
		{
			rayHit = true;
			// set closest distance to the raycast hit distance (only hit objects closer than closest distance)
			closestDistance = raycastHit.distance;
		}
	}

	// if ray hit object, scatter (bounce) ray and check for next hit
	if (rayHit)
	{
		ray_t scattered;
		color3_t color;

		// check if maximum depth (number of bounces) is reached, get color from material and scattered ray
		if (depth > 0 && raycastHit.material->Scatter(ray, raycastHit, color, scattered))
		{
			// recursive function, call self and modulate colors of depth bounces
			return color * Trace(scattered, minDistance, maxDistance, raycastHit, depth - 1);
		}
		else
		{
			// reached maximum depth of bounces (get emissive color, will be black except for Emissive materials)
			return raycastHit.material->GetEmissive();
		}
	}


	// if ray not hit, return scene sky color
	glm::vec3 direction = glm::normalize(ray.direction);
	float t = (direction.y + 1) * 0.5f; // direction.y (-1 <-> 1) => (0 <-> 1)
	color3_t color = lerp(m_bottomColor, m_topColor, t);

	return color;
}
