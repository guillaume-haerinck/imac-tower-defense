#pragma once

enum CursorType {
	ROTATION,
	ARROW,
	LOADING,
	NO,
	CLICK,
	ACTIVATE,
	DESACTIVATE
};

namespace evnt {
	struct ChangeCursor {
		ChangeCursor(CursorType cursor) : cursor(cursor) {}

		CursorType cursor;
	};
}
