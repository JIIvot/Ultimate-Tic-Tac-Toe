#pragma once

#include "Controller/GameController.h"
#include "Renderer/GameRenderer.h"

class CApplication;

class CGame
{
public:
	CGame();

	void Update( CApplication* app );
	void Render( CApplication* app );
	
private:
	CGameController m_controller;
	CGameRenderer   m_renderer;
};
