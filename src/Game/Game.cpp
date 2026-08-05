#include "Game.h"
#include "Core/Application.h"
#include "Core/Input.h"
#include "SharedConstants.h"

#include <SDL3/SDL_render.h>

#define BACKGROUND_COLOR 255, 255, 255, 255

CGame::CGame()
{
	m_controller.m_renderer = &m_renderer;
	m_renderer.m_controller = &m_controller;
}

void CGame::Update( CApplication* app )
{
	const CInput* input = app->GetInput();

	if ( input->IsKeyJustPressed( SDL_SCANCODE_ESCAPE ) )
	{
		app->Quit();
	}

	if ( input->IsKeyJustPressed( SDL_SCANCODE_F11 ) )
	{
		app->ToggleFullscreen();
	}
	
	m_controller.Update( app );
}

void CGame::Render( CApplication* app )
{
	SDL_Renderer* renderer = app->GetRenderer();
	
	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
	SDL_RenderClear( renderer );
	
	static constexpr SDL_FRect kBackgroundRect = { 0.0f, 0.0f, kGameWidth, kGameHeight };
	SDL_SetRenderDrawColor( renderer, BACKGROUND_COLOR );
	SDL_RenderFillRect( renderer, &kBackgroundRect );
	
	m_renderer.Render( app );
	
	SDL_RenderPresent( renderer );
}
