
#define _USE_MATH_DEFINES
#include <math.h>

namespace math {
	const float PI = static_cast<float>(M_PI);
	const float DEGREES_TO_RADIANS = PI / 180;
	const float RADIANS_TO_DEGREES = 1.f / DEGREES_TO_RADIANS;
}
