#pragma once

#include "CellCoords.h"
#include "Constants.h"
#include "CellRenderer.h"

#include <glm/vec2.hpp>
#include <array>

struct SDL_Renderer;
class CGameController;
class CGame;

class CGameRenderer
{
private:
	using BoardRenderers = std::array<std::array<CCellRenderer, kBoardSize>, kBoardSize>;
	using InnerRenderers = std::array<std::array<BoardRenderers, kBoardSize>, kBoardSize>;

public:
	CGameRenderer( CGameController* controller );

	~CGameRenderer();

	void Render( const CGame* game );

private:
	static void RenderBackground( SDL_Renderer* renderer );

	void Reset();

	void OnWin();

	void OnDraw();

	void InitInnerCellRenderers( glm::ivec2 outer );

	void ResetInnerCellRenderers( glm::ivec2 outer );

	void RenderInnerBoard( const CGame* game, glm::ivec2 outer, bool isActive );

	void ProcessHoveredCell();

	[[nodiscard]]
	CCellRenderer& GetOuterRenderer( glm::ivec2 outer )
	{
		return m_outerRenderers[outer.x][outer.y];
	}

	[[nodiscard]]
	CCellRenderer& GetInnerRenderer( const SCellCoords& coords )
	{
		return m_innerRenderers[coords.outer.x][coords.outer.y][coords.inner.x][coords.inner.y];
	}

private:
	CGameController* m_controller;

	SCellCoords m_lastHoveredCellCoords;
	bool m_hadValidHoveredCell = false;

	BoardRenderers m_outerRenderers;
	InnerRenderers m_innerRenderers;
};
