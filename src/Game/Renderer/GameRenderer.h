#pragma once

#include "CellRenderer.h"
#include "SharedConstants.h"
#include "Game/CellCoords.h"

#include <glm/vec2.hpp>

class CCellRenderer;
class CGameController;
class CApplication;

class CGameRenderer
{
public:
	CGameRenderer();

	void Reset();

	void Render( CApplication* app );

	void OnVictory();
	void OnDraw();

private:
	void InitInnerCellRenderers( glm::ivec2 outer );

	void ResetInnerCellRenderers( glm::ivec2 outer );

	void RenderInnerBoard( CApplication* app, glm::ivec2 outer, bool isActive );

	void ProcessHoveredCell();

	[[nodiscard]]
	CCellRenderer& GetOuterRenderer( glm::ivec2 outer ) { return m_outerRenderers[outer.x][outer.y]; }

	[[nodiscard]]
	CCellRenderer& GetInnerRenderer( const SCellCoords& coords ) { return m_innerRenderers[coords.outer.x][coords.outer.y][coords.inner.x][coords.inner.y]; }

public:
	CGameController* m_controller          = nullptr;

private:
	SCellCoords      m_lastHoveredCellCoords;
	bool             m_hadValidHoveredCell = false;

	CCellRenderer    m_outerRenderers[kBoardSize][kBoardSize];
	CCellRenderer    m_innerRenderers[kBoardSize][kBoardSize][kBoardSize][kBoardSize];
};
