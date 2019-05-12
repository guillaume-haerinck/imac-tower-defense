#pragma once

namespace evnt {
	struct DeleteEntity {
		DeleteEntity(std::uint32_t entityId) : entityId(entityId) {}

		std::uint32_t entityId;
	};
}
