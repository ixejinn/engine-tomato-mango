#ifndef MANGO_WAVECOMPONENT_H
#define MANGO_WAVECOMPONENT_H

#include <glm/vec3.hpp>
#include <stdint.h>
namespace tomato
{
	struct WaveComponent
	{
		glm::vec3 origin{ 0 };
		float diameter;

		int64_t startTick{ 0 };
	};
}

#endif // MANGO_WAVECOMPONENT_H
