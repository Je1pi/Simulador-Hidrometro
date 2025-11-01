#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cmath>

class Display {
public:
    Display(bool state = true) : active(state) {}
    void show(double total_volume_l, double fluxoAtual, long long tsec);
    void clearScreen();
    void show(double total_volume_l, double fluxoAtual, long long tsec, int fail_duration = 0, int fail_remaining = 0);
    bool isActive() const { return active; }
private:
    bool active = true;
};

#endif