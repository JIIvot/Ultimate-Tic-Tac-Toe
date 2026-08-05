#pragma once

#include "SharedConstants.h"
#include "Game/CellCoords.h"

#include <vector>
#include <glm/vec2.hpp>

enum ECellState : uint8_t;
class CApplication;
class CGameRenderer;

class CGameController
{
public:
	CGameController();

	void Update( CApplication* app );

	[[nodiscard]]
	bool IsBoardEnabled( glm::ivec2 coords ) const { return m_boardEnabled[coords.x][coords.y]; }

	[[nodiscard]]
	bool HasValidHoveredCell() const { return m_hasValidHoveredCell; }

	[[nodiscard]]
	const SCellCoords& GetHoveredCellCoords() const { return m_hoveredCellCoords; }

	[[nodiscard]]
	ECellState GetOuterCellState( glm::ivec2 outer ) const { return m_outerCells[outer.x][outer.y]; }

	[[nodiscard]]
	ECellState GetInnerCellState( const SCellCoords& coords ) const { return m_innerCells[coords.outer.x][coords.outer.y][coords.inner.x][coords.inner.y]; }

	[[nodiscard]]
	bool IsBoardActive( glm::ivec2 coords ) const { return m_isAllBoardsActive || ( m_activeBoardCoords.x == coords.x && m_activeBoardCoords.y == coords.y ); }

	[[nodiscard]]
	const std::vector<glm::ivec2>& GetClosedCellsCoords() const { return m_closedCellsCoords; }

private:
	[[nodiscard]]
	static bool IsValidCoords( glm::ivec2 coords ) { return coords.x >= 0 && coords.x < kBoardSize && coords.y >= 0 && coords.y < kBoardSize; }

	void UpdateHoveredCellCoords( CApplication* app );

	void Reset();
	void ResetInnerBoard( glm::ivec2 outer );

	void ProcessClick();

	void SetInnerCellState( const SCellCoords& coords, ECellState state ) { m_innerCells[coords.outer.x][coords.outer.y][coords.inner.x][coords.inner.y] = state; }

	bool TryCloseInnerBoard();

	void UpdateOuterBoardState();

	void DisableAllBoards();

	[[nodiscard]]
	static int32_t GetSequenceLength( const ECellState( &board )[kBoardSize][kBoardSize], glm::ivec2 start, glm::ivec2 direction, ECellState state );

	[[nodiscard]]
	static bool CheckForVictoryInLine( const ECellState( &board )[kBoardSize][kBoardSize], glm::ivec2 start, glm::ivec2 line );

	void CollectSequence( const ECellState( &board )[kBoardSize][kBoardSize], glm::ivec2 start, glm::ivec2 direction, ECellState state );

	void CollectClosedCellsCoords( const ECellState( &board )[kBoardSize][kBoardSize], glm::ivec2 start, glm::ivec2 line );

	void SetOuterCellState( glm::ivec2 coords, ECellState state ) { m_outerCells[coords.x][coords.y] = state; }

	void SetBoardEnabled( glm::ivec2 coords, bool isEnabled ) { m_boardEnabled[coords.x][coords.y] = isEnabled; }

	[[nodiscard]]
	int32_t GetInnerBoardTurnNum( glm::ivec2 coords ) const { return m_innerBoardTurnNums[coords.x][coords.y]; }

	void IncreaseInnerBoardTurnNum( glm::ivec2 coords ) { ++m_innerBoardTurnNums[coords.x][coords.y]; }

public:
	CGameRenderer*          m_renderer            = nullptr;

private:
	static constexpr glm::ivec2 kVictoryLines[] = {
		{ 1,  0 },
		{ 0,  1 },
		{ 1,  1 },
		{ 1, -1 }
	};

	ECellState              m_outerCells[kBoardSize][kBoardSize];
	ECellState              m_innerCells[kBoardSize][kBoardSize][kBoardSize][kBoardSize];
	bool                    m_boardEnabled[kBoardSize][kBoardSize];

	int32_t                 m_innerBoardTurnNums[kBoardSize][kBoardSize];
	int32_t                 m_outerBoardTurnNum;

	SCellCoords             m_hoveredCellCoords;
	bool                    m_hasValidHoveredCell = false;

	bool                    m_isAllBoardsActive   = true;
	glm::ivec2              m_activeBoardCoords;

	bool                    m_isCrossTurn;
	SCellCoords             m_lastTurnCoords;

	std::vector<glm::ivec2> m_closedCellsCoords;
};
