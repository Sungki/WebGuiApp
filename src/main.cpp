/*#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif
#include <iostream>
#include <vector>

#include "SDL.h"
#undef main

#include "imgui.h"
#include "imgui_sdl.h"

struct Vector2
{
	float x;
	float y;
};

struct sStar
{
	float fAngle = 0.0f;
	float fDistance = 0.0f;
	float fSpeed = 0.0f;
};

bool run;
SDL_Renderer* renderer;
SDL_Texture* texture;
const int nStars = 500;
std::vector<sStar> vStars;
Vector2 vOrigin;

Uint64 NOW = SDL_GetPerformanceCounter();
Uint64 LAST = 0;
double deltaTime = 0;

float Random(float a, float b)
{
	return (b - a) * (float(rand()) / float(RAND_MAX)) + a;
}

void mainloop()
{
	LAST = NOW;
	NOW = SDL_GetPerformanceCounter();

	deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

	ImGuiIO& io = ImGui::GetIO();

	int wheel = 0;

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT) run = false;
		else if (e.type == SDL_WINDOWEVENT)
		{
			if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
					io.DisplaySize.x = static_cast<float>(e.window.data1);
					io.DisplaySize.y = static_cast<float>(e.window.data2);
			}
		}
		else if (e.type == SDL_MOUSEWHEEL)
		{
			wheel = e.wheel.y;
		}
	}

	int mouseX, mouseY;
	const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

	io.DeltaTime = 1.0f / 60.0f;
	io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
	io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
	io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
	io.MouseWheel = static_cast<float>(wheel);

	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	ImGui::Begin("Image");
	ImGui::Image(texture, ImVec2(100, 100));
	ImGui::End();

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	for (auto& star : vStars)
	{
		star.fDistance += star.fSpeed * deltaTime * (star.fDistance / 100.0f);
		if (star.fDistance > 1000.0f)
		{
			star.fAngle = Random(0.0f, 2.0f * 3.1459f);
			star.fSpeed = Random(10.0f, 100.0f);
			star.fDistance = Random(1.0f, 100.0f);
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawPoint(renderer, cos(star.fAngle) * star.fDistance + vOrigin.x, sin(star.fAngle)* star.fDistance + vOrigin.y);
//		Draw(olc::vf2d(cos(star.fAngle), sin(star.fAngle)) * star.fDistance + vOrigin, star.col * (star.fDistance / 100.0f));
	}

	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());

	SDL_RenderPresent(renderer);
}

int main()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("WebGuiApp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, 800, 600);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 100, 100);
	{
		SDL_SetRenderTarget(renderer, texture);
		SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderTarget(renderer, nullptr);
	}

	vStars.resize(nStars);

	for (auto& star : vStars)
	{
		star.fAngle = Random(0.0f, 2.0f * 3.1459f);
		star.fSpeed = Random(10.0f, 100.0f);
		star.fDistance = Random(1.0f, 100.0f);
	}

	vOrigin = { 400, 300 };

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(mainloop, 0, 0);
#else
	run = true;
	while (run)
	{
		mainloop();
	}

	ImGuiSDL::Deinitialize();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	ImGui::DestroyContext();
#endif

	return 0;
}

#include <string>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngineSDL.h"

class StarField : public olc::PixelGameEngine
{
public:
	StarField() { sAppName = "WebGuiApp";}

private:
	int nMapWidth = 16;				// World Dimensions
	int nMapHeight = 16;

	float fPlayerX = 14.7f;			// Player Start Position
	float fPlayerY = 5.09f;
	float fPlayerA = 0.0f;			// Player Start Rotation
	float fFOV = 3.14159f / 4.0f;	// Field of View
	float fDepth = 16.0f;			// Maximum rendering distance
	float fSpeed = 5.0f;			// Walking Speed
	std::wstring map;

	const int nStars = 1000;

	struct sStar
	{
		float fAngle = 0.0f;
		float fDistance = 0.0f;
		float fSpeed = 0.0f;
		olc::Pixel col = olc::WHITE;
	};

	std::vector<sStar> vStars;
	olc::vf2d vOrigin;

public:
	float Random(float a, float b)
	{
		return (b - a) * (float(rand()) / float(RAND_MAX)) + a;
	}

	bool OnUserCreate() override
	{
		vStars.resize(nStars);

		for (auto& star : vStars)
		{
			star.fAngle = Random(0.0f, 2.0f * 3.1459f);
			star.fSpeed = Random(10.0f, 100.0f);
			star.fDistance = Random(1.0f, 100.0f);
			float lum = Random(0.3f, 1.0f);
			star.col = olc::PixelF(lum, lum, lum, 1.0f);
		}

		vOrigin = { float(ScreenWidth() / 2), float(ScreenHeight() / 2) };

		map += L"################";
		map += L"#..............#";
		map += L"#.......########";
		map += L"#..............#";
		map += L"#...#####......#";
		map += L"#......##......#";
		map += L"#..............#";
		map += L"###............#";
		map += L"##.............#";
		map += L"#......####..###";
		map += L"#......#.......#";
		map += L"#...####.......#";
		map += L"#..............#";
		map += L"#......#########";
		map += L"#..............#";
		map += L"################";

		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetKey(olc::Key::A).bHeld)
			fPlayerA -= (fSpeed * 0.75f) * fElapsedTime;

		if (GetKey(olc::Key::D).bHeld)
			fPlayerA += (fSpeed * 0.75f) * fElapsedTime;

		if (GetKey(olc::Key::W).bHeld)
		{
			fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;;
			fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;;
		}

		if (GetKey(olc::Key::S).bHeld)
		{
			fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;;
			fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
		}

		Clear(olc::BLACK);

		for (auto& star : vStars)
		{
			star.fDistance += star.fSpeed * fElapsedTime * (star.fDistance / 100.0f);
			if (star.fDistance > 200.0f)
			{
				star.fAngle = Random(0.0f, 2.0f * 3.1459f);
				star.fSpeed = Random(10.0f, 100.0f);
				star.fDistance = Random(1.0f, 100.0f);
				float lum = Random(0.3f, 1.0f);
				star.col = olc::PixelF(lum, lum, lum, 1.0f);
			}

			Draw(olc::vf2d(cos(star.fAngle), sin(star.fAngle)) * star.fDistance + vOrigin, star.col * (star.fDistance / 100.0f));
		}

		for (int x = 0; x < ScreenWidth(); x++)
		{
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)ScreenWidth()) * fFOV;

			float fStepSize = 0.1f;		  // Increment size for ray casting, decrease to increase										
			float fDistanceToWall = 0.0f; //                                      resolution

			bool bHitWall = false;		// Set when ray hits wall block

			float fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
			float fEyeY = cosf(fRayAngle);

			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += fStepSize;
				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true;			// Just set distance to maximum depth
					fDistanceToWall = fDepth;
				}
				else
				{
					if (map.c_str()[nTestX * nMapWidth + nTestY] == '#')
					{
						bHitWall = true;
					}
				}
			}

			int nCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceToWall);
			int nFloor = ScreenHeight() - nCeiling;

			for (int y = 0; y < ScreenHeight(); y++)
			{
//				if (y <= nCeiling)
//					Draw(x, y, olc::BLACK);
				if (y > nCeiling && y <= nFloor)
					Draw(x, y, olc::DARK_GREEN);
//				else
//				{
//					Draw(x, y, olc::DARK_GREY);
//				}
			}
		}

		return true;
	}
};*/

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngineSDL.h"

#include "Config.h"
//#include "Emulator.h"
#include "Gameboy.h"

class GameBoyEmulator : public olc::PixelGameEngine
{
public:
	GameBoyEmulator() { sAppName = "WebGuiApp"; }
	GameBoy* gb;

//	Emulator* m_Emulator;

public:
	bool OnUserCreate() override
	{
		gb = GameBoy::CreateInstance();
//		m_Emulator = new Emulator();
//		m_Emulator->LoadRom("tetris.gb");
//		m_Emulator->ResetCPU();

//		gb.Initialize();

		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		gb->StartEmulation();

//		m_Emulator->Update();


		for (int y = 0; y < 144; y++)
			for (int x = 0; x < 160; x++)
				switch (gb->screen[y * 144 + x])
				{
				case 0x2588: Draw(x, y, olc::WHITE);break;
				case 0x2593: Draw(x, y, olc::CYAN); break;
				case 0x2592: Draw(x, y, olc::DARK_BLUE); break;
				case ' ': Draw(x, y, olc::BLACK); break;
				default: Draw(x, y, olc::DARK_CYAN);
				}

/*		for (int x = 0; x < ScreenWidth(); x++)
		{
			for (int y = 0; y < ScreenHeight(); y++)
    		{
					Draw(x, y, olc::DARK_GREEN);
			}
		}*/

		return true;
	}

	bool OnUserDestroy() override
	{
//		delete m_Emulator;

//		delete gb;

		return true;
	}
};

int main()
{
//	StarField demo;
//	if (demo.Construct(256, 240, 4, 4))
//		demo.Start();

//	GameBoy* gb = GameBoy::CreateInstance();
//	gb->StartEmulation();
//	delete gb;

	GameBoyEmulator demo;
	if (demo.Construct(160, 144, 4, 4))
		demo.Start();

	return 0;
}