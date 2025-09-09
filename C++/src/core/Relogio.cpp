#include "Relogio.hpp"

Relogio::Relogio() : seconds_elapsed(0) {}

void Relogio::tick(int sec) {
    seconds_elapsed += sec;
}

long long Relogio::now() const {
    return seconds_elapsed;
}