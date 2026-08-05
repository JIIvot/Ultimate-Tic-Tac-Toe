#pragma once

#include "SharedConstants.h"
#include "Game/CellCoords.h"

#include <vector>
#include <array>
#include <glm/vec2.hpp>

enum ECellState : uint8_t;
class CApplication;
class CGameRenderer;

class CGameController
{
private:
	using BoardCells = std::array<std::array<ECellState, kBoardSize>, kBoardSize>;
	using InnerCells = std::array<std::array<BoardCells, kBoardSize>, kBoardSize>;
	using BoardEnabled = std::array<std::array<bool, kBoardSize>, kBoardSize>;
	using InnerBoardTurnNums = std::array<std::array<int32_t, kBoardSize>, kBoardSize>;

public:
	CGameController();

	void Update( CApplication* app );

	[[nodiscard]]
	bool IsBoardEnabled( glm::ivec2 coords ) const
	{
		return m_boardEnabled[coords.x][coords.y];
	}

	[[nodiscard]]
	bool HasValidHoveredCell() const
	{
		return m_hasValidHoveredCell;
	}

	[[nodiscard]]
	const SCellCoords& GetHoveredCell() const
	{
		return m_hoveredCell;
	}

	[[nodiscard]]
	ECellState GetOuterCellState( glm::ivec2 outer ) const
	{
		return m_outerCells[outer.x][outer.y];
	}

	[[nodiscard]]
	ECellState GetInnerCellState( const SCellCoords& coords ) const
	{
		return m_innerCells[coords.outer.x][coords.outer.y][coords.inner.x][coords.inner.y];
	}

	[[nodiscard]]
	bool IsBoardActive( glm::ivec2 coords ) const
	{
		return m_isAllBoardsActive || ( m_activeBoard.x == coords.x && m_activeBoard.y == coords.y );
	}

	[[nodiscard]]
	const std::vector<glm::ivec2>& GetWinningCells() const
	{
		return m_winningCells;
	}

private:
	[[nodiscard]]
	static bool IsValidCoords( glm::ivec2 coords )
	{
		return coords.x >= 0 && coords.x < kBoardSize && coords.y >= 0 && coords.y < kBoardSize;
	}

	[[nodiscard]]
	static int32_t GetSequenceLength( const BoardCells& board, glm::ivec2 start, glm::ivec2 direction, ECellState state );

	[[nodiscard]]
	static bool CheckForWinInLine( const BoardCells& board, glm::ivec2 start, glm::ivec2 line );

	void UpdateHoveredCell( CApplication* app );

	void Reset();

	void ResetInnerBoard( glm::ivec2 outer );

	void ProcessClick();

	void SetInnerCellState( const SCellCoords& coords, ECellState state )
	{
		m_innerCells[coords.outer.x][coords.outer.y][coords.inner.x][coords.inner.y] = state;
	}

	bool TryFinishInnerBoard();

	void TryFinishOuterBoard();

	void DisableAllBoards();

	void CollectOuterWinningCells( glm::ivec2 start, glm::ivec2 direction, ECellState state );

	void CollectOuterWinningCellsLine( glm::ivec2 start, glm::ivec2 line );

	void SetOuterCellState( glm::ivec2 coords, ECellState state )
	{
		m_outerCells[coords.x][coords.y] = state;
	}

	void SetBoardEnabled( glm::ivec2 coords, bool isEnabled )
	{
		m_boardEnabled[coords.x][coords.y] = isEnabled;
	}

	[[nodiscard]]
	int32_t GetInnerBoardTurnNum( glm::ivec2 coords ) const
	{
		return m_innerBoardTurnNums[coords.x][coords.y];
	}

	void IncreaseInnerBoardTurnNum( glm::ivec2 coords )
	{
		++m_innerBoardTurnNums[coords.x][coords.y];
	}

	void ResetInnerBoardTurnNum( glm::ivec2 coords )
	{
		m_innerBoardTurnNums[coords.x][coords.y] = 0;
	}

public:
	CGameRenderer* m_renderer = nullptr;

private:
	static constexpr glm::ivec2 kVictoryLines[] = {
		{ 1,  0 },
		{ 0,  1 },
		{ 1,  1 },
		{ 1, -1 }
	};

	BoardCells m_outerCells;
	InnerCells m_innerCells;
	BoardEnabled m_boardEnabled;

	InnerBoardTurnNums m_innerBoardTurnNums;
	int32_t m_outerBoardTurnNum;

	SCellCoords m_hoveredCell;
	bool m_hasValidHoveredCell = false;

	bool m_isAllBoardsActive = true;
	glm::ivec2 m_activeBoard;

	bool m_isCrossTurn;
	SCellCoords m_lastTurnCoords;

	std::vector<glm::ivec2> m_winningCells;
};
