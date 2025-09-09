#ifndef CONFIGURACAO_HPP
#define CONFIGURACAO_HPP

#include <string>
#include <nlohmann/json.hpp>

class Configuracao {
public:
    int tick_ms = 1000;                          // Intervalo de tempo entre os "ticks" da simulação (ms)
    double fluxo_inicial_mm = 20.0;              // Fluxo inicial de água (mm)
    double fluxo_minimo_mm = 0.0;                // Fluxo mínimo permitido (mm)
    double fluxo_maximo_mm = 100.0;              // Fluxo máximo permitido (mm)
    double probabilidade_falha_aleatoria_por_min = 0.70; // Probabilidade de falha aleatória por minuto
    int fail_duration_sec_min = 5;               // Duração mínima de uma falha (segundos)
    int fail_duration_sec_max = 30;              // Duração máxima de uma falha (segundos)
    bool simulate_pressure_variation = true;     // Simular variações de pressão
    int log_interval_sec = 5;                    // Intervalo de tempo para registrar logs (segundos)
    bool simulate_air_volume = true;             // Simular volume de ar quando faltar água
    double air_volume_rate = 0.1;                // Taxa de crescimento do volume devido ao ar (litros por segundo)

    bool load_from_file(const std::string &filename);
    void save_to_file(const std::string &filename) const;
};

#endif