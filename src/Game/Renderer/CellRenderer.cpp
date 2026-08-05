#include "Core/Application.h"
#include "CellRenderer.h"

#include <SDL3/SDL_render.h>
#include <cmath>

void CCellRenderer::Render( CApplication* app, ECellState state, bool isActive ) const
{
	SDL_FRect destRect;
	destRect.x = m_position.x - m_size * m_scale * 0.5f;
	destRect.y = m_position.y - m_size * m_scale * 0.5f;
	destRect.w = m_size * m_scale;
	destRect.h = m_size * m_scale;

	SDL_RenderTexture( app->GetRenderer(), app->GetAtlasTexture(), GetSpriteRect( state, isActive ), &destRect );
}

void CCellRenderer::ProcessAnimation( CApplication* app )
{
	if ( m_size == m_targetSize )
	{
		return;
	}

	const float t = 1.0f - std::expf( -kSizeLerpSpeed * app->GetDeltaTime() );
	m_size = std::lerp( m_size, m_targetSize, t );

	if ( std::fabsf( m_size - m_targetSize ) <= kSizeLerpError )
	{
		m_size = m_targetSize;
	}
}

const SDL_FRect* CCellRenderer::GetSpriteRect( ECellState state, bool isActive )
{
	return isActive ? &kSpriteRects[state] : &kSpriteRects[state + eCellState_Count];
}
