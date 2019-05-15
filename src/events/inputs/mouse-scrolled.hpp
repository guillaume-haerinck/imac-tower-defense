#pragma once

namespace evnt {
	struct MouseScrolled {
		MouseScrolled(int value) : value(value) {}

		int value;
	};
}
