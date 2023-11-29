#include <iostream>
#include "Renderer.h"
#include "Random.h"
#include "Canvas.h"
#include "Camera.h"
#include "Scene.h"


int main(int argc, char* argv[])
{
	std::cout << "Hello World!" << std::endl;
	Renderer renderer;
	renderer.Initialize();
	renderer.CreateWindow("RayTracer", 400, 300);

	Canvas canvas(400, 300, renderer);

	float aspectRatio = canvas.GetSize().x / (float)canvas.GetSize().y;
	std::shared_ptr<Camera> camera = std::make_shared<Camera>(glm::vec3{ 0, 0, 1 }, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 }, 70.0f, aspectRatio);

	Scene scene; // sky color could be set with the top and bottom color
	scene.SetCamera(camera);

	bool quit = false;
	while (!quit)
	{
		SDL_Event event;
		SDL_PollEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				quit = true;
				break;
			}
			break;
		}


		/*
		//canvas.Clear({ 0, 0, 0, 1 });
		for (int i = 0; i < 1000; i++)
		{
			canvas.DrawPoint({ random(0, 400), random(0, 300) }, { random01(), random01(), random01(), 1 });
		}
		canvas.Update();
		*/

		canvas.Clear({ 0, 0, 0, 1 });
		scene.Render(canvas);
		canvas.Update();

		renderer.PresentCanvas(canvas);
	}

	renderer.Shutdown();

	return 0;
}