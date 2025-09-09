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
};

#endif