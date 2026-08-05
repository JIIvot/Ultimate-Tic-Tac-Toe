#pragma once

#include <memory>
#include <SDL3/SDL_rect.h>

class CInput;
class CGame;
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

class CApplication
{
public:
	CApplication();

	~CApplication();
	
	void Run();

	void Quit()
	{
		m_shouldRun = false;
	}
	
	void ToggleFullscreen();

	[[nodiscard]]
	const CInput* GetInput() const
	{
		return m_input.get();
	}
	
	[[nodiscard]]
	float GetDeltaTime() const
	{
		return m_deltaTime;
	}
	
	[[nodiscard]]
	SDL_Renderer* GetRenderer() const
	{
		return m_renderer;
	}
	
	[[nodiscard]]
	SDL_Texture* GetAtlasTexture() const
	{
		return m_atlasTexture;
	}
	
private:
	void UpdateDeltaTime();

	void ProcessEvents();
	
private:
	static constexpr char kTitle[] = "Ultimate Tic Tac Toe";

	bool m_shouldRun = false;
	bool m_isFullscreen = false;
	
	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;
	SDL_Texture* m_atlasTexture = nullptr;
	
	float m_deltaTime;
	float m_perfFrequency;
	uint64_t m_lastPerfCounter;
	
	std::unique_ptr<CInput> m_input;
	std::unique_ptr<CGame> m_game;
};
