#pragma once

#include <entt/entt.hpp>

#define TAG(__name__) using __name__ = entt::label<#__name__ ## _hs>
#pragma warning(push)
#pragma warning(disable: 4307) // EnTT throws a warning with the hashing in VS2017

namespace renderOrderTag {
	TAG(o_Tile);
	TAG(o_Building);
	TAG(o_Enemy);
	TAG(o_Enemy2); //For enemy eye
	TAG(o_Projectile);
	TAG(o_VFX);
}

namespace renderTag {
    TAG(Atlas);
	TAG(OneTimeAtlas);
    TAG(Single);
};

namespace tileTag {
	TAG(Constructible);
	TAG(Path);
}

namespace targetingTag {
	TAG(LookAt);
	TAG(Follow);
}

namespace entityTag {
	TAG(Tower);
	TAG(Mirror);
	TAG(Enemy);
	TAG(SlowProjectile);
	TAG(Tile);
}

namespace enemyTag {
	TAG(Robot);
	TAG(Kamikaze);
}

namespace projectileType {
	TAG(Slow);
	TAG(Damage);
}

namespace stateTag {
	TAG(IsBeingControlled);
	TAG(RotateableByMouse);
}

namespace positionTag {
	TAG(IsOnHoveredTile);
}