#include "Configuracao.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool Configuracao::load_from_file(const std::string &filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de configuração: " << filename << "\n";
        return false;
    }

    try {
        json j;
        ifs >> j;

        tick_ms = j.at("intervalo_tick_ms").get<int>();
        fluxo_inicial_mm = j.at("fluxo_inicial_mm").get<double>();
        fluxo_minimo_mm = j.at("fluxo_minimo_mm").get<double>();
        fluxo_maximo_mm = j.at("fluxo_maximo_mm").get<double>();
        probabilidade_falha_aleatoria_por_min = j.at("probabilidade_falha_aleatoria_por_min").get<double>();
        fail_duration_sec_min = j.at("duracao_minima_falha_seg").get<int>();
        fail_duration_sec_max = j.at("duracao_maxima_falha_seg").get<int>();
        simulate_pressure_variation = j.at("simular_variacao_pressao").get<bool>();
        log_interval_sec = j.at("intervalo_log_seg").get<int>();
        simulate_air_volume = j.at("simular_volume_ar").get<bool>();
        air_volume_rate = j.at("taxa_volume_ar").get<double>();
    } catch (const std::exception &e) {
        std::cerr << "Erro ao carregar o arquivo de configuração: " << e.what() << "\n";
        return false;
    }

    return true;
}

void Configuracao::save_to_file(const std::string &filename) const {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "Erro ao abrir o arquivo para salvar a configuração: " << filename << "\n";
        return;
    }

    json j = {
        {"intervalo_tick_ms", tick_ms},
        {"fluxo_inicial_mm", fluxo_inicial_mm},
        {"fluxo_minimo_mm", fluxo_minimo_mm},
        {"fluxo_maximo_mm", fluxo_maximo_mm},
        {"probabilidade_falha_aleatoria_por_min", probabilidade_falha_aleatoria_por_min},
        {"duracao_minima_falha_seg", fail_duration_sec_min},
        {"duracao_maxima_falha_seg", fail_duration_sec_max},
        {"simular_variacao_pressao", simulate_pressure_variation},
        {"intervalo_log_seg", log_interval_sec},
        {"simular_volume_ar", simulate_air_volume},
        {"taxa_volume_ar", air_volume_rate}
    };

    ofs << j.dump(4);
}