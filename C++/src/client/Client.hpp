#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <atomic>
#include "../facade/SystemFacade.hpp"

using namespace std;

class Client {
public:
    Client(int argc, char *argv[]);
    ~Client();
    
    // Método principal para executar o cliente CLI
    int run();
    
    // Método para executar CLI em thread separada
    void runCLIThread();

private:
    std::unique_ptr<SystemFacade> facade;
    std::vector<int> sha_instances; // IDs das instâncias SHA criadas
    std::atomic<bool> running;
    std::unique_ptr<std::thread> cli_thread;
    
    // Menu e Interface CLI
    void showMainMenu();
    void clearScreen();
    void pauseScreen();
    int getIntInput(const string& prompt, int min = INT_MIN, int max = INT_MAX);
    double getDoubleInput(const string& prompt, double min = 0.0, double max = 1000.0);
    string getStringInput(const string& prompt);
    
    // Funcionalidades da Facade
    void configSimulatorMenu();           // 1) configSimulatorSHA
    void createSHAMenu();                 // 2) criaSHA
    void finalizeSHAMenu();               // 3) finalizaSHA
    void modifyFlowMenu();                // 4) modificaVazaoSHA
    void enableImageGenerationMenu();     // 5) habilitaGeracaoImagemSHA
    
    // Funcionalidades auxiliares
    void listActiveSHAs();
    void showSHADetails(int sha_id);
    
    // Gerenciamento de estado
    bool isValidSHAId(int sha_id);
    void addSHAInstance(int sha_id);
    void removeSHAInstance(int sha_id);
};

#endif // CLIENT_HPP
