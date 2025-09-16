#ifndef CONTROLADORA_HPP
#define CONTROLADORA_HPP

#include "Configuracao.hpp"
#include "Display.hpp"
#include "Relogio.hpp"
#include "Entrada.hpp"
#include "Hidrometro.hpp"
#include <atomic>
#include <thread>
#include <random>

class Controladora {
public:
    Controladora(const Configuracao &cfg);
    void start();
    void stop();
    void set_flow(double v);
    void toggle_fail_forced();
    void save_config(const std::string &fn);
    void set_minmax(double mn, double mx);
    double getVolumeTotal() const;
    double getFluxoAtual() const;
    bool isCurrentlyFailed() const;

private:
    void run_loop();

    Configuracao cfg;
    Entrada entrada;
    Display display;
    Relogio relogio;
    Hidrometro hidrometro;
    std::atomic<bool> running;
    std::thread sim_thread;
    std::mt19937 rng;
    double per_tick_fail_prob = 0.0;
    std::atomic<bool> currently_failed_flag{false};
};

#endif