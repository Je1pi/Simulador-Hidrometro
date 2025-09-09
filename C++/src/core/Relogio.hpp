#ifndef RELOGIO_HPP
#define RELOGIO_HPP

#include <atomic>

class Relogio {
public:
    Relogio();
    void tick(int sec = 1);
    long long now() const;

private:
    std::atomic<long long> seconds_elapsed;
};

#endif