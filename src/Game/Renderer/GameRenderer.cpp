#include "GameRenderer.h"
#include "Game/Controller/GameController.h"

CGameRenderer::CGameRenderer()
{
	glm::ivec2 outer;
	for ( outer.x = 0; outer.x < kBoardSize; ++outer.x )
	{
		for ( outer.y = 0; outer.y < kBoardSize; ++outer.y )
		{
			CCellRenderer& outerRenderer = GetOuterRenderer( outer );
			outerRenderer.SetOuterScale();

			outerRenderer.m_position = {
				kOuterCellRegionSize.x * ( outer.x + 0.5f ),
				kOuterCellRegionSize.y * ( outer.y + 0.5f )
			};

			InitInnerCellRenderers( outer );
		}
	}
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

void CGameRenderer::Render( CApplication* app )
{
	ProcessHoveredCell();

	glm::ivec2 outer;
	for ( outer.x = 0; outer.x < kBoardSize; ++outer.x )
	{
		for ( outer.y = 0; outer.y < kBoardSize; ++outer.y )
		{
			if ( m_controller->IsBoardEnabled( outer ) )
			{
				RenderInnerBoard( app, outer, m_controller->IsBoardActive( outer ) );
				continue;
			}

			CCellRenderer&   renderer = GetOuterRenderer( outer );
			const ECellState state    = m_controller->GetOuterCellState( outer );

			renderer.ProcessAnimation( app );
			renderer.Render( app, state, false );
		}
	}
}

void CGameRenderer::OnVictory()
{
	const std::vector<glm::ivec2>& closedCellsCoords = m_controller->GetClosedCellsCoords();
	for ( const glm::ivec2& coords : closedCellsCoords )
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

			renderer.m_position = {
				kOuterCellRegionSize.x * outer.x + kInnerCellRegionSize.x * ( coords.inner.x + 0.5f ),
				kOuterCellRegionSize.y * outer.y + kInnerCellRegionSize.y * ( coords.inner.y + 0.5f )
			};
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

void CGameRenderer::RenderInnerBoard( CApplication* app, glm::ivec2 outer, bool isActive )
{
	SCellCoords coords;
	coords.outer = outer;

	for ( coords.inner.x = 0; coords.inner.x < kBoardSize; ++coords.inner.x )
	{
		for ( coords.inner.y = 0; coords.inner.y < kBoardSize; ++coords.inner.y )
		{
			CCellRenderer&   renderer = GetInnerRenderer( coords );
			const ECellState state    = m_controller->GetInnerCellState( coords );

			renderer.ProcessAnimation( app );
			renderer.Render( app, state, isActive );
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

	const SCellCoords& coords = m_controller->GetHoveredCellCoords();
	GetInnerRenderer( coords ).SetHoveredSize();

	m_lastHoveredCellCoords = coords;
	m_hadValidHoveredCell   = true;
}
