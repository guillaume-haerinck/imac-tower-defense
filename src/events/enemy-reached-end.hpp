#pragma once

namespace evnt {
	struct EnemyReachedEnd {
		EnemyReachedEnd(std::uint32_t entity) : entity(entity) {}

		std::uint32_t entity;
	};
}