#include "Hidrometro.hpp"

Hidrometro::Hidrometro(Entrada &entrada, Relogio &relogio)
    : entrada(entrada), relogio(relogio), total_volume_l(0.0) {}

void Hidrometro::update(int tick_sec, bool water_available, double air_volume_rate) {
    if (water_available) {
        double flow_l_per_sec = entrada.get_flow_mm() / 1000.0;
        total_volume_l += flow_l_per_sec * tick_sec;
    } else {
        total_volume_l += air_volume_rate * tick_sec;
    }
}

double Hidrometro::get_total_volume_l() const {
    return total_volume_l;
}