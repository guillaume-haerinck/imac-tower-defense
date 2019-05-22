#pragma once

enum CursorType {
	ROTATION,
	ARROW,
	LOADING,
	NO,
	CLICK
};

namespace evnt {
	struct ChangeCursor {
		ChangeCursor(CursorType cursor) : cursor(cursor) {}

		CursorType cursor;
	};
}
