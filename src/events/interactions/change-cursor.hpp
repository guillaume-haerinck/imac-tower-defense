#pragma once

enum CursorType {
	ROTATION = 0,
	ARROW = 1,
	LOADING = 2,
	NO = 3,
	CLICK = 4,
	ACTIVATE = 5,
	DESACTIVATE = 6
};

namespace evnt {
	struct ChangeCursor {
		ChangeCursor(CursorType cursor) : cursor(cursor) {}

		CursorType cursor;
	};
}
