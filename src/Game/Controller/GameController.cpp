#include "GameController.h"
#include "Game/CellState.h"
#include "Core/Application.h"
#include "Core/Input.h"
#include "SharedConstants.h"
#include "Game/Renderer/GameRenderer.h"

#include <SDL3/SDL_render.h>

CGameController::CGameController()
{
	Reset();
}

void CGameController::Update( CApplication* app )
{
	const CInput* input = app->GetInput();

	if ( input->IsKeyJustPressed( SDL_SCANCODE_R ) )
	{
		Reset();
		m_renderer->Reset();
	}

	UpdateHoveredCellCoords( app );

	if ( input->IsButtonJustPressed( SDL_BUTTON_LEFT ) )
	{
		ProcessClick();
	}
}

void CGameController::UpdateHoveredCellCoords( CApplication* app )
{
	const glm::vec2 mousePosition = app->GetInput()->GetMousePosition();

	float renderX;
	float renderY;
	SDL_RenderCoordinatesFromWindow( app->GetRenderer(), mousePosition.x, mousePosition.y, &renderX, &renderY );

	const bool isValidX = renderX >= 0 && renderX < kGameWidth;
	const bool isValidY = renderY >= 0 && renderY < kGameHeight;

	if ( !isValidX || !isValidY )
	{
		m_hasValidHoveredCell = false;
		return;
	}

	SCellCoords coords;

	coords.outer.x = static_cast<int32_t>( renderX / kOuterCellRegionSize.x );
	coords.outer.y = static_cast<int32_t>( renderY / kOuterCellRegionSize.y );

	const float localX = renderX - coords.outer.x * kOuterCellRegionSize.x;
	const float localY = renderY - coords.outer.y * kOuterCellRegionSize.y;

	coords.inner.x = static_cast<int32_t>( localX / kInnerCellRegionSize.x );
	coords.inner.y = static_cast<int32_t>( localY / kInnerCellRegionSize.y );

	if ( !IsBoardEnabled( coords.outer ) || !IsBoardActive( coords.outer ) || GetInnerCellState( coords ) != eCellState_None )
	{
		m_hasValidHoveredCell = false;
		return;
	}

	m_hoveredCellCoords   = coords;
	m_hasValidHoveredCell = true;
}

void CGameController::Reset()
{
	m_closedCellsCoords.clear();

	m_isCrossTurn       = true;
	m_isAllBoardsActive = true;
	m_outerBoardTurnNum = 0;

	glm::ivec2 outer;
	for ( outer.x = 0; outer.x < kBoardSize; ++outer.x )
	{
		for ( outer.y = 0; outer.y < kBoardSize; ++outer.y )
		{
			SetBoardEnabled( outer, true );
			SetOuterCellState( outer, eCellState_None );

			m_innerBoardTurnNums[outer.x][outer.y] = 0;

			ResetInnerBoard( outer );
		}
	}
}

void CGameController::ResetInnerBoard( glm::ivec2 outer )
{
	SCellCoords coords;
	coords.outer = outer;

	for ( coords.inner.x = 0; coords.inner.x < kBoardSize; ++coords.inner.x )
	{
		for ( coords.inner.y = 0; coords.inner.y < kBoardSize; ++coords.inner.y )
		{
			SetInnerCellState( coords, eCellState_None );
		}
	}
}

void CGameController::ProcessClick()
{
	if ( !m_hasValidHoveredCell )
	{
		return;
	}

	SetInnerCellState( m_hoveredCellCoords, m_isCrossTurn ? eCellState_Cross : eCellState_Circle );

	IncreaseInnerBoardTurnNum( m_hoveredCellCoords.outer );

	m_lastTurnCoords    = m_hoveredCellCoords;
	m_isCrossTurn       = !m_isCrossTurn;
	m_isAllBoardsActive = false;
	m_activeBoardCoords = m_hoveredCellCoords.inner;

	if ( TryCloseInnerBoard() )
	{
		UpdateOuterBoardState();
	}

	if ( !IsBoardEnabled( m_activeBoardCoords ) )
	{
		m_isAllBoardsActive = true;
	}
}

bool CGameController::TryCloseInnerBoard()
{
	for ( const glm::ivec2& line : kVictoryLines )
	{
		const ECellState( &board )[kBoardSize][kBoardSize] = m_innerCells[m_lastTurnCoords.outer.x][m_lastTurnCoords.outer.y];

		if ( !CheckForVictoryInLine( board, m_lastTurnCoords.inner, line ) )
		{
			continue;
		}

		++m_outerBoardTurnNum;
		const ECellState state = GetInnerCellState( m_lastTurnCoords );

		SetOuterCellState( m_lastTurnCoords.outer, state );
		SetBoardEnabled( m_lastTurnCoords.outer, false );

		return true;
	}

	if ( GetInnerBoardTurnNum( m_lastTurnCoords.outer ) < kTotalBoardSize )
	{
		return false;
	}

	++m_outerBoardTurnNum;
	SetBoardEnabled( m_lastTurnCoords.outer, false );

	return true;
}

void CGameController::UpdateOuterBoardState()
{
	for ( const glm::ivec2& line : kVictoryLines )
	{
		if ( !CheckForVictoryInLine( m_outerCells, m_lastTurnCoords.outer, line ) )
		{
			continue;
		}

		DisableAllBoards();
		CollectClosedCellsCoords( m_outerCells, m_lastTurnCoords.outer, line );

		m_renderer->OnVictory();

		return;
	}

	if ( m_outerBoardTurnNum == kTotalBoardSize )
	{
		m_renderer->OnDraw();
	}
}

void CGameController::DisableAllBoards()
{
	for ( int32_t x = 0; x < kBoardSize; ++x )
	{
		for ( int32_t y = 0; y < kBoardSize; ++y )
		{
			m_boardEnabled[x][y] = false;
		}
	}
}

int32_t CGameController::GetSequenceLength( const ECellState( &board )[kBoardSize][kBoardSize], glm::ivec2 start, glm::ivec2 direction, ECellState state )
{
	glm::ivec2 coords =  start + direction;

	int32_t length = 0;
	while ( IsValidCoords( coords ) && board[coords.x][coords.y] == state )
	{
		++length;
		coords += direction;
	}

	return length;
}

void CGameController::CollectSequence( const ECellState( &board )[kBoardSize][kBoardSize], glm::ivec2 start, glm::ivec2 direction, ECellState state )
{
	glm::ivec2 coords = start + direction;

	while ( IsValidCoords( coords ) && board[coords.x][coords.y] == state )
	{
		m_closedCellsCoords.push_back( coords );
		coords += direction;
	}
}

bool CGameController::CheckForVictoryInLine( const ECellState( &board )[kBoardSize][kBoardSize], glm::ivec2 start, glm::ivec2 line )
{
	const ECellState state = board[start.x][start.y];

	const int32_t positiveLength = GetSequenceLength( board, start, line, state );
	const int32_t negativeLength = GetSequenceLength( board, start, -line, state );

	return positiveLength + negativeLength + 1 >= kWinSequenceLength;
}

void CGameController::CollectClosedCellsCoords( const ECellState( &board )[kBoardSize][kBoardSize], glm::ivec2 start, glm::ivec2 line )
{
	const ECellState state = board[start.x][start.y];

	CollectSequence( board, start, line, state );
	CollectSequence( board, start, -line, state );

	m_closedCellsCoords.push_back( start );
}
