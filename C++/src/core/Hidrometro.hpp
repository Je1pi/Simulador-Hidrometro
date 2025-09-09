#ifndef HIDROMETRO_HPP
#define HIDROMETRO_HPP

#include "Entrada.hpp"
#include "Display.hpp"
#include "Relogio.hpp"

class Hidrometro {
public:
    Hidrometro(Entrada &entrada, Relogio &relogio);

    void update(int tick_sec, bool water_available, double air_volume_rate = 0.0);
    double get_total_volume_l() const;

private:
    Entrada &entrada;
    Relogio &relogio;
    double total_volume_l;
};

#endif