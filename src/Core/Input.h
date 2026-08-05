#pragma once

#include <glm/vec2.hpp>
#include <vector>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_mouse.h>

class CInput
{
public:
	CInput();
	
	void Update();
	
	[[nodiscard]]
	bool IsKeyPressed( SDL_Scancode code ) const
	{
		return m_currKeys[code];
	}
	
	[[nodiscard]]
	bool IsKeyJustPressed( SDL_Scancode code ) const
	{
		return m_currKeys[code] && !m_lastKeys[code];
	}
	
	[[nodiscard]]
	bool IsButtonPressed( uint32_t button ) const
	{
		return m_currButtonFlags & SDL_BUTTON_MASK( button );
	}
	
	[[nodiscard]]
	bool IsButtonJustPressed( uint32_t button ) const
	{
		return ( m_currButtonFlags & SDL_BUTTON_MASK( button ) ) && !( m_lastButtonFlags & SDL_BUTTON_MASK( button ) );
	}
	
	[[nodiscard]]
	glm::vec2 GetMousePosition() const
	{
		return m_mousePosition;
	}
	
private:
	int32_t m_numKeys;
	const bool* m_currKeys;
	std::vector<uint8_t> m_lastKeys;
	
	glm::vec2 m_mousePosition;

	SDL_MouseButtonFlags m_currButtonFlags;
	SDL_MouseButtonFlags m_lastButtonFlags;
};
