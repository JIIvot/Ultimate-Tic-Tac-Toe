#include "GameRenderer.h"
#include "Controller/GameController.h"
#include "Game.h"

#include <SDL3/SDL_render.h>

static constexpr SDL_Color kBackgroundColor = { 255, 255, 255, 255 };

CGameRenderer::CGameRenderer( CGameController* controller )
	: m_controller( controller )
{
	controller->SetOnDrawEventFunction( [this] { OnDraw(); } );
	controller->SetOnWinEventFunction( [this] { OnWin(); } );
	controller->SetOnResetEventFunction( [this] { Reset(); } );

	glm::ivec2 outer;
	for ( outer.x = 0; outer.x < kBoardSize; ++outer.x )
	{
		for ( outer.y = 0; outer.y < kBoardSize; ++outer.y )
		{
			CCellRenderer& outerRenderer = GetOuterRenderer( outer );
			outerRenderer.SetOuterScale();

			glm::vec2 position;
			position.x = kOuterCellRegionSize.x * ( outer.x + 0.5f );
			position.y = kOuterCellRegionSize.y * ( outer.y + 0.5f );

			outerRenderer.SetPosition( position );

			InitInnerCellRenderers( outer );
		}
	}
}

CGameRenderer::~CGameRenderer()
{
	m_controller->SetOnDrawEventFunction( nullptr );
	m_controller->SetOnWinEventFunction( nullptr );
	m_controller->SetOnResetEventFunction( nullptr );
}

void CGameRenderer::Reset()
{
	glm::ivec2 outer;
	for ( outer.x = 0; outer.x < kBoardSize; ++outer.x )
	{
		for ( outer.y = 0; outer.y < kBoardSize; ++outer.y )
		{
			CCellRenderer& outerRenderer = GetOuterRenderer( outer );

			outerRenderer.ResetAnimation();
			outerRenderer.SetNormalSize();

			ResetInnerCellRenderers( outer );
		}
	}
}

void CGameRenderer::Render( const CGame* game )
{
	SDL_Renderer* renderer = game->GetRenderer();

	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
	SDL_RenderClear( renderer );

	RenderBackground( renderer );

	ProcessHoveredCell();

	glm::ivec2 outer;
	for ( outer.x = 0; outer.x < kBoardSize; ++outer.x )
	{
		for ( outer.y = 0; outer.y < kBoardSize; ++outer.y )
		{
			if ( m_controller->IsBoardEnabled( outer ) )
			{
				RenderInnerBoard( game, outer, m_controller->IsBoardActive( outer ) );
				continue;
			}

			CCellRenderer& renderer = GetOuterRenderer( outer );
			const ECellState state = m_controller->GetOuterCellState( outer );

			renderer.ProcessAnimation( game );
			renderer.Render( game, state, false );
		}
	}

	SDL_RenderPresent( renderer );
}

void CGameRenderer::OnWin()
{
	const std::vector<glm::ivec2>& winningCells = m_controller->GetWinningCells();
	for ( glm::ivec2 coords : winningCells )
	{
		GetOuterRenderer( coords ).SetClosedSize();
	}
}

void CGameRenderer::OnDraw()
{
	for ( int32_t x = 0; x < kBoardSize; ++x )
	{
		for ( int32_t y = 0; y < kBoardSize; ++y )
		{
			m_outerRenderers[x][y].SetClosedSize();
		}
	}
}

void CGameRenderer::InitInnerCellRenderers( glm::ivec2 outer )
{
	SCellCoords coords;
	coords.outer = outer;

	for ( coords.inner.x = 0; coords.inner.x < kBoardSize; ++coords.inner.x )
	{
		for ( coords.inner.y = 0; coords.inner.y < kBoardSize; ++coords.inner.y )
		{
			CCellRenderer& renderer = GetInnerRenderer( coords );

			glm::vec2 position;
			position.x = kOuterCellRegionSize.x * outer.x + kInnerCellRegionSize.x * ( coords.inner.x + 0.5f );
			position.y = kOuterCellRegionSize.y * outer.y + kInnerCellRegionSize.y * ( coords.inner.y + 0.5f );

			renderer.SetPosition( position );
		}
	}
}

void CGameRenderer::ResetInnerCellRenderers( glm::ivec2 outer )
{
	SCellCoords coords;
	coords.outer = outer;

	for ( coords.inner.x = 0; coords.inner.x < kBoardSize; ++coords.inner.x )
	{
		for ( coords.inner.y = 0; coords.inner.y < kBoardSize; ++coords.inner.y )
		{
			CCellRenderer& renderer = GetInnerRenderer( coords );
			renderer.ResetAnimation();
		}
	}
}

void CGameRenderer::RenderBackground( SDL_Renderer* renderer )
{
	static constexpr SDL_FRect kBackgroundRect = { 0.0f, 0.0f, kGameWidth, kGameHeight };

	SDL_SetRenderDrawColor( renderer, kBackgroundColor.r, kBackgroundColor.g, kBackgroundColor.b, kBackgroundColor.a );
	SDL_RenderFillRect( renderer, &kBackgroundRect );
}

void CGameRenderer::RenderInnerBoard( const CGame* game, glm::ivec2 outer, bool isActive )
{
	SCellCoords coords;
	coords.outer = outer;

	for ( coords.inner.x = 0; coords.inner.x < kBoardSize; ++coords.inner.x )
	{
		for ( coords.inner.y = 0; coords.inner.y < kBoardSize; ++coords.inner.y )
		{
			CCellRenderer& renderer = GetInnerRenderer( coords );
			const ECellState state = m_controller->GetInnerCellState( coords );

			renderer.ProcessAnimation( game );
			renderer.Render( game, state, isActive );
		}
	}
}

void CGameRenderer::ProcessHoveredCell()
{
	if ( m_hadValidHoveredCell )
	{
		GetInnerRenderer( m_lastHoveredCellCoords ).SetNormalSize();
	}

	if ( !m_controller->HasValidHoveredCell() )
	{
		m_hadValidHoveredCell = false;
		return;
	}

	const SCellCoords& coords = m_controller->GetHoveredCell();
	GetInnerRenderer( coords ).SetHoveredSize();

	m_lastHoveredCellCoords = coords;
	m_hadValidHoveredCell = true;
}
