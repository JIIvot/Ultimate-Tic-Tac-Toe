#pragma once

#include "CellState.h"

#include <glm/vec2.hpp>
#include <SDL3/SDL_rect.h>

class CGame;

class CCellRenderer
{
public:
	void Render( const CGame* game, ECellState state, bool isActive ) const;

	void ProcessAnimation( const CGame* game );

	void ResetAnimation()
	{
		m_size = 0.0f;
	}

	void SetNormalSize()
	{
		m_targetSize = kNormalSize;
	}

	void SetHoveredSize()
	{
		m_targetSize = kHoveredSize;
	}

	void SetClosedSize()
	{
		m_targetSize = kClosedSize;
	}

	void SetOuterScale()
	{
		m_scale = kOuterScale;
	}

	void SetPosition( glm::vec2 position )
	{
		m_position = position;
	}

private:
	[[nodiscard]]
	static const SDL_FRect* GetSpriteRect( ECellState state, bool isActive )
	{
		return &kSpriteRects[isActive][state];
	}

private:
	static constexpr float kSizeLerpSpeed = 10.0f;
	static constexpr float kSizeLerpError = 0.1f;

	static constexpr int32_t kSpriteSize = 24;
	static constexpr float kNormalSize = kSpriteSize;
	static constexpr float kHoveredSize = kNormalSize * 2.0f;
	static constexpr float kClosedSize = kNormalSize * 0.75f;

	static constexpr float kOuterScale = 4.0f;

	// First index is whether the cell is active.
	// Second index is the cell's type and corresponds to ECellState enum
	static constexpr SDL_FRect kSpriteRects[2][eCellState_Count] = {
		// Active cell sprites
		{
			{ 64.0f, 32.0f, kSpriteSize, kSpriteSize }, // None
			{ 96.0f,  0.0f, kSpriteSize, kSpriteSize }, // Cross
			{ 64.0f,  0.0f, kSpriteSize, kSpriteSize }  // Circle
		},
		// Not active cell sprites
		{
			{  0.0f, 32.0f, kSpriteSize, kSpriteSize }, // None
			{ 32.0f,  0.0f, kSpriteSize, kSpriteSize }, // Cross
			{  0.0f,  0.0f, kSpriteSize, kSpriteSize }  // Circle
		}
	};

	glm::vec2 m_position = { 0.0f, 0.0f };

	float m_size = 0.0f;
	float m_targetSize = kNormalSize;
	float m_scale = 1.0f;
};
