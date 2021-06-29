#include "Config.h"
#include "Emulator.h"
#include "GameBoy.h"

//#include <Windows.h>
//#include <iostream>

static const int windowWidth = 160;
static const int windowHeight = 144 ;

///////////////////////////////////////////////////////////////////////////////////////

static int total = 0 ;
static int timer = 0 ;
static int current = 0 ;
static int counter = 0 ;
static bool first = true ;


static wchar_t* screen;
static HANDLE hConsole;

static void CheckFPS( )
{
	if (first)
	{
		first = false ;
		timer = clock() ;
	}

	counter++ ;
	current = clock() ;
	if ((timer + 1000) < current)
	{
		timer = current ;
		total = counter ;
		counter = 0 ;
		//OutputDebugStr(outputMessage);
	}
}

///////////////////////////////////////////////////////////////////////////////////////

static void DoRender( )
{
	GameBoy* gb = GameBoy::GetSingleton() ;
	gb->RenderGame( ) ;
}


///////////////////////////////////////////////////////////////////////////////////////

GameBoy* GameBoy::m_Instance = 0 ;

GameBoy* GameBoy::CreateInstance( )
{
	if (0 == m_Instance)
	{
		m_Instance = new GameBoy( ) ;
		m_Instance->Initialize( ) ;
	}
	return m_Instance ;
}

//////////////////////////////////////////////////////////////////////////////////////////

GameBoy* GameBoy::GetSingleton( )
{
	return m_Instance ;
}

//////////////////////////////////////////////////////////////////////////////////////////

GameBoy::GameBoy(void) :
	m_Emulator(NULL)
{
	m_Emulator = new Emulator( ) ;
	m_Emulator->LoadRom("CastlevaniaAdventure.gb") ;
	m_Emulator->InitGame( DoRender ) ;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool GameBoy::Initialize( )
{
	return CreateSDLWindow( ) ;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameBoy::StartEmulation( )
{
//	bool quit = false ;
//	SDL_Event event;

//	float fps = 59.73 ;
//	float interval = 1000 ;
//	interval /= fps ;

//	unsigned int time2 = clock() ;
//	bool keys[8];

//	while (!quit)
//	{
/*		while( SDL_PollEvent( &event ) )
		{
			HandleInput( event ) ;

			if( event.type == SDL_QUIT )
			{
				quit = true;
			}
		}*/

//		unsigned int current = SDL_GetTicks( ) ;

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

		m_Emulator->Update();

/*		if (GetAsyncKeyState((unsigned short)'Q') & 0x8000)
			quit = true;

		unsigned int current = clock();
		if( (time2 + interval) < current )
		{
			CheckFPS( ) ;
			m_Emulator->Update();
			time2 = current ;
		}*/

//	}
//	SDL_Quit( ) ;
}

//////////////////////////////////////////////////////////////////////////////////////////

GameBoy::~GameBoy(void)
{
	delete m_Emulator ;
}

//////////////////////////////////////////////////////////////////////////////////////////

void GameBoy::RenderGame( )
{
//	EmulationLoop() ;
/* 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 	glLoadIdentity();
 	glRasterPos2i(-1, 1);
	glPixelZoom(1, -1);
 	glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, m_Emulator->m_ScreenData);
	SDL_GL_SwapBuffers( ) ;*/

	for(int y=0; y<144;y++)
		for(int x=0; x<160;x++)
			switch (m_Emulator->m_ScreenData[y][x][0])
			{
			case 0xFF: screen[y * windowWidth + x] = 0x2588; break;
			case 0xCC: screen[y * windowWidth + x] = 0x2593; break;
			case 0x77: screen[y * windowWidth + x] = 0x2592; break;
			case 0x00: screen[y * windowWidth + x] = ' '; break;
			default: screen[y * windowWidth + x] = 'x';
			}

	DWORD bw;
	WriteConsoleOutputCharacter(hConsole, screen, windowWidth * windowHeight, { 0,0 }, &bw);
}

//////////////////////////////////////////////////////////////////////////////////////////

void GameBoy::SetKeyPressed(int key)
{
	m_Emulator->KeyPressed(key) ;
}

//////////////////////////////////////////////////////////////////////////////////////////

void GameBoy::SetKeyReleased(int key)
{
	m_Emulator->KeyReleased(key) ;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool GameBoy::CreateSDLWindow( )
{
/*	if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
	{
		return false ;
	}
	if( SDL_SetVideoMode( windowWidth, windowHeight, 8, SDL_OPENGL ) == NULL )
	{
		return false ;
	}

	InitGL();

	SDL_WM_SetCaption( "OpenGL Test", NULL );*/

	screen = new wchar_t[windowWidth * windowHeight];
	hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	return true ;
}

//////////////////////////////////////////////////////////////////////////////////////////

void GameBoy::InitGL( )
{
/*	glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0, windowWidth, windowHeight, 0, -1.0, 1.0);
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_FLAT);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DITHER);
	glDisable(GL_BLEND);*/
}

//////////////////////////////////////////////////////////////////////////////////////////

void GameBoy::HandleInput()
{
/*	if( event.type == SDL_KEYDOWN )
	{
		int key = -1 ;
		switch( event.key.keysym.sym )
		{
			case SDLK_a : key = 4 ; break ;
			case SDLK_s : key = 5 ; break ;
			case SDLK_RETURN : key = 7 ; break ;
			case SDLK_SPACE : key = 6; break ;
			case SDLK_RIGHT : key = 0 ; break ;
			case SDLK_LEFT : key = 1 ; break ;
			case SDLK_UP : key = 2 ; break ;
			case SDLK_DOWN : key = 3 ; break ;
		}
		if (key != -1)
		{
			SetKeyPressed(key) ;
		}
	}
	//If a key was released
	else if( event.type == SDL_KEYUP )
	{
		int key = -1 ;
		switch( event.key.keysym.sym )
		{
			case SDLK_a : key = 4 ; break ;
			case SDLK_s : key = 5 ; break ;
			case SDLK_RETURN : key = 7 ; break ;
			case SDLK_SPACE : key = 6; break ;
			case SDLK_RIGHT : key = 0 ; break ;
			case SDLK_LEFT : key = 1 ; break ;
			case SDLK_UP : key = 2 ; break ;
			case SDLK_DOWN : key = 3 ; break ;
		}
		if (key != -1)
		{
			SetKeyReleased(key) ;
		}
	}*/
}

//////////////////////////////////////////////////////////////////////////////////////////
