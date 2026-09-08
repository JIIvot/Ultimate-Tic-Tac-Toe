#pragma once

#include <memory>
#include <cstdint>

struct SDL_Renderer;
struct SDL_Window;
struct SDL_Texture;
class CInput;
class CGameController;
class CGameRenderer;

class CGame
{
public:
	CGame();

	~CGame();

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
	SDL_Renderer* GetRenderer() const
	{
		return m_renderer;
	}

	[[nodiscard]]
	SDL_Texture* GetTextureAtlas() const
	{
		return m_textureAtlas;
	}

	[[nodiscard]]
	float GetDeltaTime() const
	{
		return m_deltaTime;
	}

private:
	void UpdateDeltaTime();

	void ProcessEvents();

private:
	bool m_shouldRun = false;
	bool m_isFullscreen = false;

	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;
	SDL_Texture* m_textureAtlas = nullptr;

	float m_deltaTime;
	float m_perfFrequency;
	uint64_t m_lastPerfCounter;

	std::unique_ptr<CInput> m_input;

	std::unique_ptr<CGameController> m_gameController;
	std::unique_ptr<CGameRenderer> m_gameRenderer;
};
