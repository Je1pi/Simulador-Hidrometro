#ifndef ENTRADA_HPP
#define ENTRADA_HPP

class Entrada {
public:
    Entrada(double initial_flow, double min_flow, double max_flow);

    void set_flow_mm(double flow);
    double get_flow_mm() const;
    void set_min_max(double min, double max);
    void force_fail(bool fail);
    bool is_forced_fail() const;
    void simulate_variation(bool simulate);

private:
    double flow_mm;
    double min_flow;
    double max_flow;
    bool forced_fail;
};

#endif