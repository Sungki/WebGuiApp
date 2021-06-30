#pragma once
#ifndef _GAMEBOY_H
#define _GAMEBOY_H

#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>

#include "Emulator.h"

class Emulator;


class GameBoy
{
public:
	static				GameBoy*				CreateInstance				( ) ;
	static				GameBoy*				GetSingleton				( ) ;
					~GameBoy(void);

						void					RenderGame					( ) ;
						bool					Initialize					( ) ;
						void					SetKeyPressed				( int key ) ;
						void					SetKeyReleased				( int key ) ;
						void					StartEmulation				( ) ;
						void					HandleInput					() ;
private:
					GameBoy(void);

						bool					CreateSDLWindow				( ) ;

						void					InitGL						( ) ;

	static				GameBoy*				m_Instance ;


						Emulator*				m_Emulator ;
};

#endif
