#include "Application.h"
#include "SharedConstants.h"
#include "Input.h"
#include "Game/Game.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

CApplication::CApplication()
{
	if ( !SDL_Init( SDL_INIT_VIDEO ) )
	{
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, kTitle, "Failed to initialize SDL3!", nullptr );
		return;
	}

	const SDL_DisplayID displayId = SDL_GetPrimaryDisplay();
	const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode( displayId );

	const int32_t width = static_cast<int32_t>( displayMode->w / 1.5f );
	const int32_t height = static_cast<int32_t>( displayMode->h / 1.5f );
	const int32_t windowSize = std::min( width, height );
	
	m_window = SDL_CreateWindow( "Ultimate Tic-Tac-Toe", windowSize, windowSize, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY );
	if ( !m_window )
	{
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, kTitle, "Failed to create window!", nullptr );
		return;
	}
	
	m_renderer = SDL_CreateRenderer( m_window, nullptr );
	if ( !m_renderer )
	{
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, kTitle, "Failed to create renderer!", nullptr );
		return;
	}
	
	m_atlasTexture = IMG_LoadTexture( m_renderer, "res/atlas.png" );
	if ( !m_atlasTexture )
	{
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, kTitle, "Failed to load texture!", nullptr );
		return;
	}
	
	SDL_SetTextureScaleMode( m_atlasTexture, SDL_SCALEMODE_NEAREST );
	
	SDL_SetRenderLogicalPresentation( m_renderer, kGameWidth, kGameHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX );
	SDL_SetRenderVSync( m_renderer, 1 );

	m_input = std::make_unique<CInput>();
	m_game = std::make_unique<CGame>();
	m_shouldRun = true;
	m_perfFrequency = static_cast<float>( SDL_GetPerformanceFrequency() );
	m_lastPerfCounter = SDL_GetPerformanceCounter();
}

CApplication::~CApplication()
{
	m_game.reset();
	m_input.reset();
	
	if ( m_atlasTexture )
	{
		SDL_DestroyTexture( m_atlasTexture );
	}
	
	if ( m_renderer )
	{
		SDL_DestroyRenderer( m_renderer );
	}
	
	if ( m_window )
	{
		SDL_DestroyWindow( m_window );
	}
	
	SDL_Quit();
}

void CApplication::Run()
{
	while ( m_shouldRun )
	{
		UpdateDeltaTime();
		m_input->Update();
		ProcessEvents();
		
		m_game->Update( this );
		m_game->Render( this );
	}
}

void CApplication::ToggleFullscreen()
{
	m_isFullscreen = !m_isFullscreen;
	SDL_SetWindowFullscreen( m_window, m_isFullscreen );
}

void CApplication::UpdateDeltaTime()
{
	const uint64_t currPerfCounter = SDL_GetPerformanceCounter();
	m_deltaTime = static_cast<float>( currPerfCounter - m_lastPerfCounter ) / m_perfFrequency;
	m_lastPerfCounter = currPerfCounter;
}

void CApplication::ProcessEvents()
{
	SDL_Event event;
	while ( SDL_PollEvent( &event ) )
	{
		if ( event.type == SDL_EVENT_QUIT )
		{
			Quit();
		}
	}
}
