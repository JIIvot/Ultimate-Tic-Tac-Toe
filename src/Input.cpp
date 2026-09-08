#include "Input.h"

#include <SDL3/SDL_keyboard.h>

CInput::CInput()
	: m_currKeys( SDL_GetKeyboardState( &m_numKeys ) )
	, m_lastKeys( m_numKeys )
{
}

void CInput::Update()
{
	memcpy( m_lastKeys.data(), m_currKeys, m_numKeys );
	
	m_lastButtonFlags = m_currButtonFlags;
	m_currButtonFlags = SDL_GetMouseState( &m_mousePosition.x, &m_mousePosition.y );
}
