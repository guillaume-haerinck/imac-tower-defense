#pragma once

namespace cmpt {
struct Age {
	//Entity dies when age reaches lifespan
    Age(float lifespan) 
    : age(0), lifespan(lifespan) {}

	float age;
	float lifespan;
};
}
