#pragma once

#include <type_traits>
#include <cstdint>

enum class MovementType
{
	Stop = 0,
	LeftRight = 1,
	ForwardBackward = 2,
	Diagonal = LeftRight | ForwardBackward,

	Unknown = 4
};

inline MovementType operator|(MovementType a, MovementType b)
{
	std::underlying_type_t<MovementType> intVal = static_cast<std::underlying_type_t<MovementType>>(a) | static_cast<std::underlying_type_t<MovementType>>(b);

	return static_cast<MovementType>(intVal);
}
