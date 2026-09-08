#pragma once

#include <cstdint>

enum ECellState : uint8_t
{
	eCellState_None,
	eCellState_Cross,
	eCellState_Circle,
	eCellState_Count
};
