#include "Game.h"
#include "Constants.h"
#include "Input.h"
#include "Controller/GameController.h"
#include "Renderer/GameRenderer.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

static constexpr char kTitle[] = "Ultimate Tic Tac Toe";

CGame::CGame()
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

	m_window = SDL_CreateWindow( kTitle, windowSize, windowSize, SDL_WINDOW_RESIZABLE );
	if ( !m_window )
	{
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, kTitle, "Failed to create a window!", nullptr );
		return;
	}

	m_renderer = SDL_CreateRenderer( m_window, nullptr );
	if ( !m_renderer )
	{
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, kTitle, "Failed to create a renderer!", nullptr );
		return;
	}

	m_textureAtlas = IMG_LoadTexture( m_renderer, "res/atlas.png" );
	if ( !m_textureAtlas )
	{
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, kTitle, "Failed to load the texture atlas!", nullptr );
		return;
	}

	SDL_SetTextureScaleMode( m_textureAtlas, SDL_SCALEMODE_NEAREST );

	SDL_SetRenderVSync( m_renderer, 1 );
	SDL_SetRenderLogicalPresentation( m_renderer, kGameWidth, kGameHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX );

	m_input = std::make_unique<CInput>();
	m_gameController = std::make_unique<CGameController>();
	m_gameRenderer = std::make_unique<CGameRenderer>( m_gameController.get() );

	m_shouldRun = true;
	m_perfFrequency = static_cast<float>( SDL_GetPerformanceFrequency() );
	m_lastPerfCounter = SDL_GetPerformanceCounter();
}

CGame::~CGame()
{
	m_gameRenderer.reset();
	m_gameController.reset();
	m_input.reset();

	SDL_DestroyTexture( m_textureAtlas );
	SDL_DestroyRenderer( m_renderer );
	SDL_DestroyWindow( m_window );

	SDL_Quit();
}

void CGame::Run()
{
	while ( m_shouldRun )
	{
		UpdateDeltaTime();
		m_input->Update();
		ProcessEvents();

		m_gameController->Update( this );
		m_gameRenderer->Render( this );
	}
}

void CGame::ToggleFullscreen()
{
	m_isFullscreen = !m_isFullscreen;
	SDL_SetWindowFullscreen( m_window, m_isFullscreen );
}

void CGame::UpdateDeltaTime()
{
	const uint64_t currCounter = SDL_GetPerformanceCounter();
	m_deltaTime = static_cast<float>( currCounter - m_lastPerfCounter ) / m_perfFrequency;
	m_lastPerfCounter = currCounter;
}

void CGame::ProcessEvents()
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
