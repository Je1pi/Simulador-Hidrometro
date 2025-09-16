#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cmath>

class Display {
public:
    void show(double total_volume_l, double fluxoAtual, long long tsec);
    void clearScreen();
        void show(double total_volume_l, double fluxoAtual, long long tsec, int fail_duration = 0, int fail_remaining = 0);
};

#endif