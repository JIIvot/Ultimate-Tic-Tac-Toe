#pragma once

#include <cstdint>
#include <glm/vec2.hpp>

constexpr int32_t   kGameWidth           = 360;
constexpr int32_t   kGameHeight          = 360;
constexpr int32_t   kBoardSize           = 3;
constexpr int32_t   kTotalBoardSize      = kBoardSize * kBoardSize;
constexpr int32_t   kWinSequenceLength   = 3;

constexpr glm::vec2 kOuterCellRegionSize = {
	static_cast<float>( kGameWidth ) / kBoardSize,
	static_cast<float>( kGameHeight ) / kBoardSize
};

constexpr glm::vec2 kInnerCellRegionSize = {
	static_cast<float>( kGameWidth ) / kTotalBoardSize,
	static_cast<float>( kGameHeight ) / kTotalBoardSize
};
