#pragma once

namespace cmpt {
struct Health {
    Health(float maxHealth) : current(maxHealth), max(maxHealth) {}

    float current;
    float max;
};
}
