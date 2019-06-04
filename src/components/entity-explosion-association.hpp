#pragma once

#include <memory>

namespace cmpt {
	struct EntityExplosionAssociation {
		//Allows us to know that an entity hasn't been already damaged by an explosion
		EntityExplosionAssociation(std::uint32_t entity, std::uint32_t explosion) : entity(entity), explosion(explosion){}

		std::uint32_t entity;
		std::uint32_t explosion;
	};
}
