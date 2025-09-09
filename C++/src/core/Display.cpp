#include "Display.hpp"

void Display::show(double total_volume_l, double fluxoAtual, long long tsec) {
    clearScreen();
    
    std::cout << "\n===== HIDROMETRO (t=" << tsec << "s) =====\n";
    std::cout << "Volume total: " << std::fixed << std::setprecision(2)
    << total_volume_l << " m3\n";
    std::cout << "Fluxo atual: " << std::fixed << std::setprecision(2)
    << fluxoAtual << " mm\n";
    std::cout << "=====================================\n";

    /* long long total_litros = static_cast<long long>(floor(total_volume_l));
    long long m3 = total_litros / 1000;
    int centenas_litros = (total_litros / 100) % 10;
    int dezenas_litros = (total_litros / 10) % 10;
    int unidades_litros = total_litros % 10;

    int ponteiro_litros = unidades_litros;
    double ang_litros = ponteiro_litros * 36.0;

    double fracao = total_volume_l - floor(total_volume_l);
    int ponteiro_decimos = static_cast<int>(floor(fracao * 10));
    double ang_decimos = ponteiro_decimos * 36.0;

    std::cout << "\n===== HIDROMETRO (t=" << tsec << "s) =====\n";
    std::cout << "Volume total: " << std::fixed << std::setprecision(3)
              << total_volume_l << " L\n";

    std::cout << "\n--- Visor Principal ---\n";
    std::cout << "[ " << (m3 / 100) % 10
              << " ][ " << (m3 / 10) % 10
              << " ][ " << m3 % 10
              << " ][ " << centenas_litros
              << " ][ " << dezenas_litros
              << " ][ " << unidades_litros
              << " ]  (m³ e litros)\n";

    std::cout << "\n--- Ponteiros ---\n";
    std::cout << "Litros : " << ponteiro_litros
              << " (~" << ang_litros << "°)\n";
    std::cout << "Décimos: " << ponteiro_decimos
              << " (~" << ang_decimos << "°)\n";
    std::cout << "=====================================\n"; */
}

void Display::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}