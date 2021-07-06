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

//#include "Config.h"
//#include "Emulator.h"

#include "peanut_gb.h"

struct priv_t
{
	uint8_t* rom;
	uint8_t* cart_ram;
	uint16_t selected_palette[3][4];
	uint16_t fb[LCD_HEIGHT][LCD_WIDTH];
	uint16_t nameTable[LCD_HEIGHT][LCD_WIDTH];
};

uint8_t gb_rom_read(struct gb_s* gb, const uint_fast32_t addr)
{
	const struct priv_t* const p = (priv_t*)gb->direct.priv;
	return p->rom[addr];
}

uint8_t gb_cart_ram_read(struct gb_s* gb, const uint_fast32_t addr)
{
	const struct priv_t* const p = (priv_t*)gb->direct.priv;
	return p->cart_ram[addr];
}

void gb_cart_ram_write(struct gb_s* gb, const uint_fast32_t addr,
	const uint8_t val)
{
	const struct priv_t* const p = (priv_t*)gb->direct.priv;
	p->cart_ram[addr] = val;
}

uint8_t* read_rom_to_ram(const char* file_name)
{
	FILE* rom_file = fopen(file_name, "rb");
	size_t rom_size;
	uint8_t* rom = NULL;

	if (rom_file == NULL)
		return NULL;

	fseek(rom_file, 0, SEEK_END);
	rom_size = ftell(rom_file);
	rewind(rom_file);
	rom = (uint8_t*)malloc(rom_size);

	if (fread(rom, sizeof(uint8_t), rom_size, rom_file) != rom_size)
	{
		free(rom);
		fclose(rom_file);
		return NULL;
	}

	fclose(rom_file);
	return rom;
}

void gb_error(struct gb_s* gb, const enum gb_error_e gb_err, const uint16_t val)
{
	struct priv_t* priv = (priv_t*)gb->direct.priv;

	switch (gb_err)
	{
	case GB_INVALID_OPCODE:
		fprintf(stdout, "Invalid opcode %#04x at PC: %#06x, SP: %#06x\n",
			val,
			gb->cpu_reg.pc - 1,
			gb->cpu_reg.sp);
		break;

	case GB_INVALID_WRITE:
	case GB_INVALID_READ:
		return;

	default:
		printf("Unknown error");
		break;
	}

	fprintf(stderr, "Error. Press q to exit, or any other key to continue.");

	if (getchar() == 'q')
	{
//		write_cart_ram_file("recovery.sav", &priv->cart_ram,
//			gb_get_save_size(gb));

		free(priv->rom);
		free(priv->cart_ram);
		exit(EXIT_FAILURE);
	}

	return;
}

void auto_assign_palette(struct priv_t* priv, uint8_t game_checksum)
{
	size_t palette_bytes = 3 * 4 * sizeof(uint16_t);

	switch (game_checksum)
	{
	case 0x71:
	case 0xFF:
	{
		const uint16_t palette[3][4] =
		{
			{ 0x7FFF, 0x7E60, 0x7C00, 0x0000 }, /* OBJ0 */
			{ 0x7FFF, 0x7E60, 0x7C00, 0x0000 }, /* OBJ1 */
			{ 0x7FFF, 0x7E60, 0x7C00, 0x0000 }  /* BG */
		};
		memcpy(priv->selected_palette, palette, palette_bytes);
		break;
	}

	/* Pokemon Yellow and Tetris */
	case 0x15:
	case 0xDB:
	case 0x95: /* Not officially */
	{
		const uint16_t palette[3][4] =
		{
			{ 0x7FFF, 0x7FE0, 0x7C00, 0x0000 }, /* OBJ0 */
			{ 0x7FFF, 0x7FE0, 0x7C00, 0x0000 }, /* OBJ1 */
			{ 0x7FFF, 0x7FE0, 0x7C00, 0x0000 }  /* BG */
		};
		memcpy(priv->selected_palette, palette, palette_bytes);
		break;
	}

	/* Donkey Kong */
	case 0x19:
	{
		const uint16_t palette[3][4] =
		{
			{ 0x7FFF, 0x7E10, 0x48E7, 0x0000 }, /* OBJ0 */
			{ 0x7FFF, 0x7E10, 0x48E7, 0x0000 }, /* OBJ1 */
			{ 0x7FFF, 0x7E60, 0x7C00, 0x0000 }  /* BG */
		};
		memcpy(priv->selected_palette, palette, palette_bytes);
		break;
	}

	/* Pokemon Blue */
	case 0x61:
	case 0x45:

		/* Pokemon Blue Star */
	case 0xD8:
	{
		const uint16_t palette[3][4] =
		{
			{ 0x7FFF, 0x7E10, 0x48E7, 0x0000 }, /* OBJ0 */
			{ 0x7FFF, 0x329F, 0x001F, 0x0000 }, /* OBJ1 */
			{ 0x7FFF, 0x329F, 0x001F, 0x0000 }  /* BG */
		};
		memcpy(priv->selected_palette, palette, palette_bytes);
		break;
	}

	/* Pokemon Red */
	case 0x14:
	{
		const uint16_t palette[3][4] =
		{
			{ 0x7FFF, 0x3FE6, 0x0200, 0x0000 }, /* OBJ0 */
			{ 0x7FFF, 0x7E10, 0x48E7, 0x0000 }, /* OBJ1 */
			{ 0x7FFF, 0x7E10, 0x48E7, 0x0000 }  /* BG */
		};
		memcpy(priv->selected_palette, palette, palette_bytes);
		break;
	}

	/* Pokemon Red Star */
	case 0x8B:
	{
		const uint16_t palette[3][4] =
		{
			{ 0x7FFF, 0x7E10, 0x48E7, 0x0000 }, /* OBJ0 */
			{ 0x7FFF, 0x329F, 0x001F, 0x0000 }, /* OBJ1 */
			{ 0x7FFF, 0x3FE6, 0x0200, 0x0000 }  /* BG */
		};
		memcpy(priv->selected_palette, palette, palette_bytes);
		break;
	}

	/* Kirby */
	case 0x27:
	case 0x49:
	case 0x5C:
	case 0xB3:
	{
		const uint16_t palette[3][4] =
		{
			{ 0x7D8A, 0x6800, 0x3000, 0x0000 }, /* OBJ0 */
			{ 0x001F, 0x7FFF, 0x7FEF, 0x021F }, /* OBJ1 */
			{ 0x527F, 0x7FE0, 0x0180, 0x0000 }  /* BG */
		};
		memcpy(priv->selected_palette, palette, palette_bytes);
		break;
	}

	/* Donkey Kong Land [1/2/III] */
	case 0x18:
	case 0x6A:
	case 0x4B:
	case 0x6B:
	{
		const uint16_t palette[3][4] =
		{
			{ 0x7F08, 0x7F40, 0x48E0, 0x2400 }, /* OBJ0 */
			{ 0x7FFF, 0x2EFF, 0x7C00, 0x001F }, /* OBJ1 */
			{ 0x7FFF, 0x463B, 0x2951, 0x0000 }  /* BG */
		};
		memcpy(priv->selected_palette, palette, palette_bytes);
		break;
	}

	/* Link's Awakening */
	case 0x70:
	{
		const uint16_t palette[3][4] =
		{
			{ 0x7FFF, 0x03E0, 0x1A00, 0x0120 }, /* OBJ0 */
			{ 0x7FFF, 0x329F, 0x001F, 0x001F }, /* OBJ1 */
			{ 0x7FFF, 0x7E10, 0x48E7, 0x0000 }  /* BG */
		};
		memcpy(priv->selected_palette, palette, palette_bytes);
		break;
	}

	/* Mega Man [1/2/3] & others I don't care about. */
	case 0x01:
	case 0x10:
	case 0x29:
	case 0x52:
	case 0x5D:
	case 0x68:
	case 0x6D:
	case 0xF6:
	{
		const uint16_t palette[3][4] =
		{
			{ 0x7FFF, 0x329F, 0x001F, 0x0000 }, /* OBJ0 */
			{ 0x7FFF, 0x3FE6, 0x0200, 0x0000 }, /* OBJ1 */
			{ 0x7FFF, 0x7EAC, 0x40C0, 0x0000 }  /* BG */
		};
		memcpy(priv->selected_palette, palette, palette_bytes);
		break;
	}

	default:
	{
		const uint16_t palette[3][4] =
		{
			{ 0x7FFF, 0x5294, 0x294A, 0x0000 },
			{ 0x7FFF, 0x5294, 0x294A, 0x0000 },
			{ 0x7FFF, 0x5294, 0x294A, 0x0000 }
		};
		printf("No palette found for 0x%02X.\n", game_checksum);
		memcpy(priv->selected_palette, palette, palette_bytes);
	}
	}
}

void lcd_draw_line(struct gb_s* gb, const uint8_t pixels[160],
	const uint_least8_t line)
{
	struct priv_t* priv = (priv_t*)gb->direct.priv;

	for (unsigned int x = 0; x < LCD_WIDTH; x++)
	{
		priv->fb[line][x] = priv->selected_palette
			[(pixels[x] & LCD_PALETTE_ALL) >> 4]
		[pixels[x] & 3];
	}
}

void lcd_name_table(struct gb_s* gb, const uint8_t index[160],
	const uint_least8_t line)
{
	struct priv_t* priv = (priv_t*)gb->direct.priv;

	for (unsigned int x = 0; x < LCD_WIDTH; x++)
	{
		priv->nameTable[line][x] = index[x];
	}
}

class GameBoyEmulator : public olc::PixelGameEngine
{
public:
	GameBoyEmulator() { sAppName = "WebGuiApp"; }
//	Emulator* m_Emulator;
//	bool keys[8];
//	olc::vf2d player;
//	bool bRender;

	struct priv_t priv;
	struct gb_s gb;
	enum gb_init_error_e gb_ret;

	SDL_Texture* texture;

	SDL_Event event;

public:
	bool OnUserCreate(SDL_Renderer* renderer) override
	{
//		m_Emulator = new Emulator();
//		m_Emulator->LoadRom("t.gat");
//		m_Emulator->ResetCPU();
//		bRender = false;
//		m_Emulator->bRender = &bRender;

		priv.rom = NULL;
		priv.cart_ram = NULL;

		if ((priv.rom = read_rom_to_ram("tetris.gb")) == NULL)
		{
			std::cout << "error";
		}

		gb_ret = gb_init(&gb, &gb_rom_read, &gb_cart_ram_read, &gb_cart_ram_write,
			&gb_error, &priv);

		gb_init_lcd(&gb, &lcd_draw_line, &lcd_name_table);

		auto_assign_palette(&priv, gb_colour_hash(&gb));

		texture = SDL_CreateTexture(renderer,
			SDL_PIXELFORMAT_RGB555,
			SDL_TEXTUREACCESS_STREAMING,
			LCD_WIDTH, LCD_HEIGHT);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime, SDL_Renderer* renderer) override
	{
/*		bRender = false;

		m_Emulator->Update();

		for (int k = 0; k < 8; k++)
		{
			short nKeyState = GetAsyncKeyState((unsigned char)("\x27\x25\x26\x28ZXAS"[k]));
			if (nKeyState & 0x8000)
			{
				if (!keys[k])
				{
					SetKeyPressed(k);
					keys[k] = true;
				}
			}
			else
			{
				if (keys[k])
				{
					SetKeyReleased(k);
					keys[k] = false;
				}
			}
		}

		if(bRender)
			Render();


		Draw(player.x, player.y, olc::YELLOW);*/

		if (GetKey(olc::Key::ENTER).bPressed) gb.direct.joypad_bits.start = 0;
		if (GetKey(olc::Key::BACK).bPressed) gb.direct.joypad_bits.select = 0;
		if (GetKey(olc::Key::Z).bPressed) gb.direct.joypad_bits.a = 0;
		if (GetKey(olc::Key::X).bPressed) gb.direct.joypad_bits.b = 0;
		if (GetKey(olc::Key::UP).bPressed) gb.direct.joypad_bits.up = 0;
		if (GetKey(olc::Key::RIGHT).bPressed) gb.direct.joypad_bits.right = 0;
		if (GetKey(olc::Key::DOWN).bPressed) gb.direct.joypad_bits.down = 0;
		if (GetKey(olc::Key::LEFT).bPressed) gb.direct.joypad_bits.left = 0;

		if (GetKey(olc::Key::ENTER).bReleased) gb.direct.joypad_bits.start = 1;
		if (GetKey(olc::Key::BACK).bReleased) gb.direct.joypad_bits.select = 1;
		if (GetKey(olc::Key::Z).bReleased) gb.direct.joypad_bits.a = 1;
		if (GetKey(olc::Key::X).bReleased) gb.direct.joypad_bits.b = 1;
		if (GetKey(olc::Key::UP).bReleased) gb.direct.joypad_bits.up = 1;
		if (GetKey(olc::Key::RIGHT).bReleased) gb.direct.joypad_bits.right = 1;
		if (GetKey(olc::Key::DOWN).bReleased) gb.direct.joypad_bits.down = 1;
		if (GetKey(olc::Key::LEFT).bReleased) gb.direct.joypad_bits.left = 1;

		gb_run_frame(&gb);



		SDL_UpdateTexture(texture, NULL, &priv.nameTable, LCD_WIDTH * sizeof(uint16_t));
//		SDL_UpdateTexture(texture, NULL, &priv.fb, LCD_WIDTH * sizeof(uint16_t));
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		SDL_Delay(10);

		return true;
	}

/*	void Render()
	{
		for (int y = 0; y < 144; y++)
			for (int x = 0; x < 160; x++)
				switch (m_Emulator->m_ScreenData[y][x][0])
				{
				case 0xFF: Draw(x, y, olc::WHITE); break;
				case 0xCC: Draw(x, y, olc::CYAN); break;
				case 0x77: Draw(x, y, olc::DARK_BLUE); break;
				case 0x00: Draw(x, y, olc::BLACK); break;

				case 0x11: 
					//Draw(x, y, olc::RED); 
					player.x = x;
					player.y = y;
					break;

				default: Draw(x, y, olc::DARK_CYAN);
				}
	}

	void SetKeyPressed(int key)
	{
		m_Emulator->KeyPressed(key);
	}

	void SetKeyReleased(int key)
	{
		m_Emulator->KeyReleased(key);
	}*/
};

int main()
{
//	StarField demo;
//	if (demo.Construct(256, 240, 4, 4))
//		demo.Start();

	GameBoyEmulator demo;
	if (demo.Construct(160, 144, 5, 5))
		demo.Start();

	return 0;
}

/*
int main(int argc, char** argv)
{
	struct gb_s gb;
	struct priv_t priv;

	priv.rom = NULL;
	priv.cart_ram = NULL;

	const double target_speed_ms = 1000.0 / VERTICAL_SYNC;
	double speed_compensation = 0.0;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Event event;
	SDL_GameController* controller = NULL;
	uint_fast32_t new_ticks, old_ticks;
	enum gb_init_error_e gb_ret;
	unsigned int fast_mode = 1;
	unsigned int fast_mode_timer = 1;
	int save_timer = 60;
	char* rom_file_name = NULL;
	char* save_file_name = NULL;
	int ret = EXIT_SUCCESS;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) < 0)
	{
		char buf[128];
		snprintf(buf, sizeof(buf),
			"Unable to initialise SDL2: %s\n", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", buf, NULL);
		ret = EXIT_FAILURE;
		goto out;
	}

	window = SDL_CreateWindow("Peanut-SDL: Opening File",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		LCD_WIDTH * 2, LCD_HEIGHT * 2,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS);

	if ((priv.rom = read_rom_to_ram("tetris.gb")) == NULL)
	{
		printf("%d: %s\n", __LINE__, strerror(errno));
		ret = EXIT_FAILURE;
		goto out;
	}

	gb_ret = gb_init(&gb, &gb_rom_read, &gb_cart_ram_read, &gb_cart_ram_write,
		&gb_error, &priv);


	gb_init_lcd(&gb, &lcd_draw_line);

	renderer = SDL_CreateRenderer(window, -1,
		SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) < 0)
	{
		printf("Renderer could not draw color: %s\n", SDL_GetError());
		ret = EXIT_FAILURE;
		goto out;
	}

	if (SDL_RenderClear(renderer) < 0)
	{
		printf("Renderer could not clear: %s\n", SDL_GetError());
		ret = EXIT_FAILURE;
		goto out;
	}

	SDL_RenderPresent(renderer);

	SDL_RenderSetLogicalSize(renderer, LCD_WIDTH, LCD_HEIGHT);
	SDL_RenderSetIntegerScale(renderer, (SDL_bool)1);

	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGB555,
		SDL_TEXTUREACCESS_STREAMING,
		LCD_WIDTH, LCD_HEIGHT);

	if (texture == NULL)
	{
		printf("Texture could not be created: %s\n", SDL_GetError());
		ret = EXIT_FAILURE;
		goto out;
	}

	auto_assign_palette(&priv, gb_colour_hash(&gb));

	while (SDL_QuitRequested() == SDL_FALSE)
	{
		int delay;
		static unsigned int rtc_timer = 0;
		static unsigned int selected_palette = 3;
		static unsigned int dump_bmp = 0;

		old_ticks = SDL_GetTicks();

		while (SDL_PollEvent(&event))
		{
			static int fullscreen = 0;

			switch (event.type)
			{
			case SDL_QUIT:
				goto quit;

			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
				switch (event.cbutton.button)
				{
				case SDL_CONTROLLER_BUTTON_A:
					gb.direct.joypad_bits.a = !event.cbutton.state;
					break;

				case SDL_CONTROLLER_BUTTON_B:
					gb.direct.joypad_bits.b = !event.cbutton.state;
					break;

				case SDL_CONTROLLER_BUTTON_BACK:
					gb.direct.joypad_bits.select = !event.cbutton.state;
					break;

				case SDL_CONTROLLER_BUTTON_START:
					gb.direct.joypad_bits.start = !event.cbutton.state;
					break;

				case SDL_CONTROLLER_BUTTON_DPAD_UP:
					gb.direct.joypad_bits.up = !event.cbutton.state;
					break;

				case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
					gb.direct.joypad_bits.right = !event.cbutton.state;
					break;

				case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
					gb.direct.joypad_bits.down = !event.cbutton.state;
					break;

				case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
					gb.direct.joypad_bits.left = !event.cbutton.state;
					break;
				}

				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_RETURN:
					gb.direct.joypad_bits.start = 0;
					break;

				case SDLK_BACKSPACE:
					gb.direct.joypad_bits.select = 0;
					break;

				case SDLK_z:
					gb.direct.joypad_bits.a = 0;
					break;

				case SDLK_x:
					gb.direct.joypad_bits.b = 0;
					break;

				case SDLK_UP:
					gb.direct.joypad_bits.up = 0;
					break;

				case SDLK_RIGHT:
					gb.direct.joypad_bits.right = 0;
					break;

				case SDLK_DOWN:
					gb.direct.joypad_bits.down = 0;
					break;

				case SDLK_LEFT:
					gb.direct.joypad_bits.left = 0;
					break;

				case SDLK_SPACE:
					fast_mode = 2;
					break;

				case SDLK_1:
					fast_mode = 1;
					break;

				case SDLK_2:
					fast_mode = 2;
					break;

				case SDLK_3:
					fast_mode = 3;
					break;

				case SDLK_4:
					fast_mode = 4;
					break;

				case SDLK_r:
					gb_reset(&gb);
					break;
#if ENABLE_LCD

				case SDLK_i:
					gb.direct.interlace = ~gb.direct.interlace;
					break;

				case SDLK_o:
					gb.direct.frame_skip = ~gb.direct.frame_skip;
					break;

				case SDLK_b:
					dump_bmp = ~dump_bmp;

					if (dump_bmp)
						puts("Dumping frames");
					else
						printf("\nStopped dumping frames\n");

					break;
#endif

				case SDLK_p:
					if (event.key.keysym.mod == KMOD_LSHIFT)
					{
						auto_assign_palette(&priv, gb_colour_hash(&gb));
						break;
					}

//					if (++selected_palette == NUMBER_OF_PALETTES)
//						selected_palette = 0;
//					manual_assign_palette(&priv, selected_palette);
					break;
				}

				break;

			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
				case SDLK_RETURN:
					gb.direct.joypad_bits.start = 1;
					break;

				case SDLK_BACKSPACE:
					gb.direct.joypad_bits.select = 1;
					break;

				case SDLK_z:
					gb.direct.joypad_bits.a = 1;
					break;

				case SDLK_x:
					gb.direct.joypad_bits.b = 1;
					break;

				case SDLK_UP:
					gb.direct.joypad_bits.up = 1;
					break;

				case SDLK_RIGHT:
					gb.direct.joypad_bits.right = 1;
					break;

				case SDLK_DOWN:
					gb.direct.joypad_bits.down = 1;
					break;

				case SDLK_LEFT:
					gb.direct.joypad_bits.left = 1;
					break;

				case SDLK_SPACE:
					fast_mode = 1;
					break;

				case SDLK_f:
					if (fullscreen)
					{
						SDL_SetWindowFullscreen(window, 0);
						fullscreen = 0;
						SDL_ShowCursor(SDL_ENABLE);
					}
					else
					{
						SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
						fullscreen = SDL_WINDOW_FULLSCREEN_DESKTOP;
						SDL_ShowCursor(SDL_DISABLE);
					}
					break;

				case SDLK_F11:
				{
					if (fullscreen)
					{
						SDL_SetWindowFullscreen(window, 0);
						fullscreen = 0;
						SDL_ShowCursor(SDL_ENABLE);
					}
					else
					{
						SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
						fullscreen = SDL_WINDOW_FULLSCREEN;
						SDL_ShowCursor(SDL_DISABLE);
					}
				}
				break;
				}

				break;
			}
		}

		gb_run_frame(&gb);

		rtc_timer += target_speed_ms / fast_mode;

		if (rtc_timer >= 1000)
		{
			rtc_timer -= 1000;
			gb_tick_rtc(&gb);
		}

		if (fast_mode_timer > 1)
		{
			fast_mode_timer--;
			continue;
		}

		fast_mode_timer = fast_mode;

		SDL_UpdateTexture(texture, NULL, &priv.fb, LCD_WIDTH * sizeof(uint16_t));
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		new_ticks = SDL_GetTicks();

		speed_compensation += target_speed_ms - (new_ticks - old_ticks);

		delay = (int)(speed_compensation);

		speed_compensation -= delay;

		if (delay > 0)
		{
			uint_fast32_t delay_ticks = SDL_GetTicks();
			uint_fast32_t after_delay_ticks;

			rtc_timer += delay;

			if (rtc_timer >= 1000)
			{
				rtc_timer -= 1000;
				gb_tick_rtc(&gb);

				--save_timer;

				if (!save_timer)
				{
#if ENABLE_SOUND_BLARGG
					SDL_LockAudioDevice(dev);
#endif
//					write_cart_ram_file(save_file_name,
//						&priv.cart_ram,
//						gb_get_save_size(&gb));
#if ENABLE_SOUND_BLARGG
					SDL_UnlockAudioDevice(dev);
#endif
					save_timer = 60;
				}
			}

			SDL_Delay(delay);

			after_delay_ticks = SDL_GetTicks();
			speed_compensation += (double)delay -
				(int)(after_delay_ticks - delay_ticks);
		}
	}

quit:
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(texture);
	SDL_GameControllerClose(controller);
	SDL_Quit();
#ifdef ENABLE_SOUND_BLARGG
	audio_cleanup();
#endif

//	write_cart_ram_file(save_file_name, &priv.cart_ram, gb_get_save_size(&gb));

out:
	free(priv.rom);
	free(priv.cart_ram);

	if (argc == 2)
		free(save_file_name);

	if (argc == 1)
		free(rom_file_name);

	return ret;
}*/