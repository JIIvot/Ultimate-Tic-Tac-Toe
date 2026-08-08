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

	UpdateHoveredCell( app );

	if ( input->IsButtonJustPressed( SDL_BUTTON_LEFT ) )
	{
		ProcessClick();
	}
}

void CGameController::UpdateHoveredCell( CApplication* app )
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

	m_hoveredCell = coords;
	m_hasValidHoveredCell = true;
}

void CGameController::Reset()
{
	m_winningCells.clear();

	m_isCrossTurn = true;
	m_isAllBoardsActive = true;
	m_outerBoardTurnNum = 0;

	glm::ivec2 outer;
	for ( outer.x = 0; outer.x < kBoardSize; ++outer.x )
	{
		for ( outer.y = 0; outer.y < kBoardSize; ++outer.y )
		{
			SetBoardEnabled( outer, true );
			SetOuterCellState( outer, eCellState_None );
			ResetInnerBoardTurnNum( outer );
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

	SetInnerCellState( m_hoveredCell, m_isCrossTurn ? eCellState_Cross : eCellState_Circle );

	IncreaseInnerBoardTurnNum( m_hoveredCell.outer );

	m_lastTurnCoords = m_hoveredCell;
	m_isCrossTurn = !m_isCrossTurn;
	m_isAllBoardsActive = false;
	m_activeBoard = m_hoveredCell.inner;

	if ( TryFinishInnerBoard() )
	{
		TryFinishOuterBoard();
	}

	if ( !IsBoardEnabled( m_activeBoard ) )
	{
		m_isAllBoardsActive = true;
	}
}

bool CGameController::TryFinishInnerBoard()
{
	for ( glm::ivec2 line : kVictoryLines )
	{
		const glm::ivec2 boardCoords = m_lastTurnCoords.outer;
		const BoardCells& board = m_innerCells[boardCoords.x][boardCoords.y];

		if ( !CheckForWinInLine( board, m_lastTurnCoords.inner, line ) )
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

void CGameController::TryFinishOuterBoard()
{
	for ( glm::ivec2 line : kVictoryLines )
	{
		if ( !CheckForWinInLine( m_outerCells, m_lastTurnCoords.outer, line ) )
		{
			continue;
		}

		DisableAllBoards();
		CollectOuterWinningCellsLine( m_lastTurnCoords.outer, line );

		m_renderer->OnWin();

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

int32_t CGameController::GetSequenceLength( const BoardCells& board, glm::ivec2 start, glm::ivec2 direction, ECellState state )
{
	glm::ivec2 coords = start + direction;

	int32_t length = 0;
	while ( IsValidCoords( coords ) && board[coords.x][coords.y] == state )
	{
		++length;
		coords += direction;
	}

	return length;
}

void CGameController::CollectOuterWinningCells( glm::ivec2 start, glm::ivec2 direction, ECellState state )
{
	glm::ivec2 coords = start + direction;

	while ( IsValidCoords( coords ) && GetOuterCellState( coords ) == state )
	{
		m_winningCells.push_back( coords );
		coords += direction;
	}
}

bool CGameController::CheckForWinInLine( const BoardCells& board, glm::ivec2 start, glm::ivec2 line )
{
	const ECellState state = board[start.x][start.y];
	if ( state == eCellState_None )
	{
		return false;
	}

	const int32_t positiveLength = GetSequenceLength( board, start, line, state );
	const int32_t negativeLength = GetSequenceLength( board, start, -line, state );

	return positiveLength + negativeLength + 1 >= kWinSequenceLength;
}

void CGameController::CollectOuterWinningCellsLine( glm::ivec2 start, glm::ivec2 line )
{
	const ECellState state = GetOuterCellState( start );

	CollectOuterWinningCells( start, line, state );
	CollectOuterWinningCells( start, -line, state );

	m_winningCells.push_back( start );
}
