#pragma once

namespace cmpt {
struct Health {
    Health(int current, int max) : current(current), max(max) {}

    int current;
    int max;
};
}
