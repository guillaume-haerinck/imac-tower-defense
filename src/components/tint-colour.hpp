#pragma once

#include <glm/glm.hpp>

namespace cmpt {
		struct TintColour {
			TintColour(glm::vec4 col, bool bOneTimeOnly = false) : col(col), bOneTimeOnly(bOneTimeOnly) {}

			glm::vec4 col;
			bool bOneTimeOnly;
	};
}
