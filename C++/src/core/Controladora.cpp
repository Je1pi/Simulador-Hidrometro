#include "Controladora.hpp"
#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>

Controladora::Controladora(const Configuracao &cfg)
    : cfg(cfg),
      entrada(cfg.fluxo_inicial_mm, cfg.fluxo_minimo_mm, cfg.fluxo_maximo_mm),
      hidrometro(entrada, relogio),
      running(false), rng(std::random_device{}()) {
    double prob_per_min = cfg.probabilidade_falha_aleatoria_por_min;
    double tick_seconds = cfg.tick_ms / 1000.0;
    if (prob_per_min <= 0) per_tick_fail_prob = 0.0;
    else {
        double per_second = 1.0 - std::pow(1.0 - prob_per_min, 1.0 / 60.0);
        per_tick_fail_prob = 1.0 - std::pow(1.0 - per_second, tick_seconds);
    }
}

void Controladora::run_loop() {
    auto tick_ms = cfg.tick_ms;
    auto tick_sec = std::max(1, (int)std::round(tick_ms / 1000.0));
    std::uniform_real_distribution<double> dist01(0.0, 1.0);
    std::uniform_int_distribution<int> failDur(cfg.fail_duration_sec_min, cfg.fail_duration_sec_max);
    int log_counter = 0;
    int log_interval_ticks = std::max(1, cfg.log_interval_sec / std::max(1, tick_sec));
    bool currently_failed = false;
    int fail_remaining_sec = 0;

    while (running) {
        auto start = std::chrono::steady_clock::now();        

        if (!entrada.is_forced_fail()) {
            if (!currently_failed) {
                double r = dist01(rng);
                if (r < per_tick_fail_prob) {
                    currently_failed = true;
                    fail_remaining_sec = failDur(rng);
                }
            }
        } else {
            if (!currently_failed) {
                currently_failed = true;
                fail_remaining_sec = cfg.fail_duration_sec_max;
            }
        }

        entrada.simulate_variation(cfg.simulate_pressure_variation);

        bool water_available = !currently_failed && !entrada.is_forced_fail();
        if (entrada.is_forced_fail()) water_available = false;

        if (!water_available && cfg.simulate_air_volume) {
            hidrometro.update(tick_sec, true, cfg.air_volume_rate);
        } else {
            hidrometro.update(tick_sec, water_available);
        }

        relogio.tick(tick_sec);

        if (currently_failed && !entrada.is_forced_fail()) {
            fail_remaining_sec -= tick_sec;
            if (fail_remaining_sec <= 0) {
                currently_failed = false;                
            }
        }

        log_counter++;
        if (log_counter >= log_interval_ticks) {
            log_counter = 0;
        }

        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        int sleep_ms = tick_ms - (int)elapsed;
        if (sleep_ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
        display.show(hidrometro.get_total_volume_l(), entrada.get_flow_mm(), relogio.now());
    }
}

void Controladora::start() {
    running = true;
    sim_thread = std::thread(&Controladora::run_loop, this);
}

void Controladora::stop() {
    running = false;
    if (sim_thread.joinable()) sim_thread.join();
}

void Controladora::set_flow(double v) {
    entrada.set_flow_mm(v);
}

void Controladora::toggle_fail_forced() {
    entrada.force_fail(!entrada.is_forced_fail());
}

void Controladora::save_config(const std::string &fn) {
    cfg.save_to_file(fn);
}

void Controladora::set_minmax(double mn, double mx) {
    entrada.set_min_max(mn, mx);
}

double Controladora::getVolumeTotal() const {
    return hidrometro.get_total_volume_l();
}

double Controladora::getFluxoAtual() const {
    return entrada.get_flow_mm();
}