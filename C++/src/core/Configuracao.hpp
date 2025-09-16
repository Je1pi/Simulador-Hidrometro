#ifndef CONFIGURACAO_HPP
#define CONFIGURACAO_HPP

#include <string>
#include <nlohmann/json.hpp>

class Configuracao {
public:
    int tick_ms = 1000;
    double fluxo_inicial_mm = 20.0;
    double fluxo_minimo_mm = 0.0;
    double fluxo_maximo_mm = 100.0;
    double probabilidade_falha_aleatoria_por_min = 0.70;
    int fail_duration_sec_min = 5;
    int fail_duration_sec_max = 30;
    bool simulate_pressure_variation = true;
    int log_interval_sec = 5;
    bool simulate_air_volume = true;
    double air_volume_rate = 0.1;

    bool load_from_file(const std::string &filename);
    void save_to_file(const std::string &filename) const;
};

#endif