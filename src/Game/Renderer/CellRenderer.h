#pragma once

#include "Game/CellState.h"

#include <glm/vec2.hpp>
#include <SDL3/SDL_rect.h>

class CApplication;

class CCellRenderer
{
public:
	void Render( CApplication* app, ECellState state, bool isActive ) const;

	void ProcessAnimation( CApplication* app );

	void ResetAnimation() { m_size = 0.0f; }

	void SetNormalSize() { m_targetSize = kNormalSize; }
	void SetHoveredSize() { m_targetSize = kHoveredSize; }
	void SetClosedSize() { m_targetSize = kClosedSize; }

	void SetOuterScale() { m_scale = kOuterScale; }

private:
	[[nodiscard]]
	static const SDL_FRect* GetSpriteRect( ECellState state, bool isActive );

public:
	glm::vec2 m_position   = { 0.0f, 0.0f };

private:
	static constexpr float     kSizeLerpSpeed = 10.0f;
	static constexpr float     kSizeLerpError = 0.1f;

	static constexpr int32_t   kSpriteSize    = 24;
	static constexpr float     kNormalSize    = kSpriteSize;
	static constexpr float     kHoveredSize   = kNormalSize * 2.0f;
	static constexpr float     kClosedSize    = kNormalSize * 0.75f;

	static constexpr float     kOuterScale    = 4.0f;

	// Each index corresponds to ECellState enum.
	// Active cell sprites go first.
	static constexpr SDL_FRect kSpriteRects[] = {
		{  0.0f, 32.0f, kSpriteSize, kSpriteSize }, // None   Active
		{ 32.0f,  0.0f, kSpriteSize, kSpriteSize }, // Cross  Active
		{  0.0f,  0.0f, kSpriteSize, kSpriteSize }, // Circle Active
		{ 64.0f, 32.0f, kSpriteSize, kSpriteSize }, // None   Not Active
		{ 96.0f,  0.0f, kSpriteSize, kSpriteSize }, // Cross  Not Active
		{ 64.0f,  0.0f, kSpriteSize, kSpriteSize }  // Circle Not Active
	};

	float     m_size       = 0.0f;
	float     m_targetSize = kNormalSize;
	float     m_scale      = 1.0f;
};
