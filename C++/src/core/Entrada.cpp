#include "Entrada.hpp"
#include <cstdlib>

Entrada::Entrada(double initial_flow, double min_flow, double max_flow)
    : flow_mm(initial_flow), min_flow(min_flow), max_flow(max_flow), forced_fail(false) {}

void Entrada::set_flow_mm(double flow) {
    flow_mm = flow;
}

double Entrada::get_flow_mm() const {
    return flow_mm;
}

void Entrada::set_min_max(double min, double max) {
    min_flow = min;
    max_flow = max;
}

void Entrada::force_fail(bool fail) {
    forced_fail = fail;
}

bool Entrada::is_forced_fail() const {
    return forced_fail;
}

void Entrada::simulate_variation(bool simulate) {
    if (simulate) {
        flow_mm += (rand() % 3 - 1);
        if (flow_mm < min_flow) flow_mm = min_flow;
        if (flow_mm > max_flow) flow_mm = max_flow;
    }
}