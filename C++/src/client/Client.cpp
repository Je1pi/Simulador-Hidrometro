#include "Client.hpp"
#include <iostream>
#include <iomanip>
#include <limits>
#include <thread>
#include <chrono>
#include <QApplication>

using namespace std;

Client::Client(int argc, char *argv[]) : running(true), cli_thread(nullptr) {
    try {
        facade = std::make_unique<SystemFacade>(argc, argv);
        cout << "✓ SystemFacade inicializada com sucesso!" << endl;
    } catch (const std::exception &e) {
        cout << "✗ Erro ao inicializar SystemFacade: " << e.what() << endl;
        throw;
    }
}

Client::~Client() {
    running.store(false);
    if (cli_thread && cli_thread->joinable()) {
        cli_thread->join();
    }
}

int Client::run() {
    try {
        cout << "=== SIMULADOR SHA - CLIENTE CLI ===" << endl;
        cout << "Sistema SHA inicializado via SystemFacade." << endl;
        cout << "Todas as 5 funcionalidades principais estão disponíveis:" << endl;
        cout << "1) configSimulatorSHA  2) criaSHA  3) finalizaSHA" << endl;
        cout << "4) modificaVazaoSHA   5) habilitaGeracaoImagemSHA" << endl;
        cout << endl;
        
        // Iniciar CLI em thread separada
        cli_thread = std::make_unique<std::thread>(&Client::runCLIThread, this);
        
        cout << "CLI iniciado em thread separada." << endl;
        cout << "Thread principal dedicada ao Qt para máxima responsividade das UIs." << endl;
        
        // Thread principal executa o Qt
        int qt_result = facade->exec();
        
        // Quando Qt terminar, sinalizar para CLI thread parar
        running.store(false);
        
        // Aguardar CLI thread finalizar
        if (cli_thread && cli_thread->joinable()) {
            cli_thread->join();
        }
        
        return qt_result;
        
    } catch (const std::exception &e) {
        cout << "Erro no cliente: " << e.what() << endl;
        running.store(false);
        return -1;
    }
}

void Client::runCLIThread() {
    try {
        clearScreen();
        cout << "=======================================" << endl;
        cout << "    SIMULADOR HIDRÔMETRO - CLIENTE CLI" << endl;
        cout << "=======================================" << endl;
        cout << "Bem-vindo ao Sistema de Simulação SHA!" << endl;
        pauseScreen();
        
        while (running.load()) {
            showMainMenu();
            
            int choice = getIntInput("Digite sua escolha: ", 0, 9);
            
            switch (choice) {
                case 1:
                    configSimulatorMenu();
                    break;
                case 2:
                    createSHAMenu();
                    break;
                case 3:
                    finalizeSHAMenu();
                    break;
                case 4:
                    modifyFlowMenu();
                    break;
                case 5:
                    enableImageGenerationMenu();
                    break;
                case 6:
                    listActiveSHAs();
                    break;
                case 0:
                    cout << "Encerrando cliente..." << endl;
                    running.store(false);
                    facade->quit(); // Sinalizar para Qt sair
                    break;
                default:
                    cout << "Opção inválida!" << endl;
            }
            
            if (running.load()) {
                pauseScreen();
            }
        }
    } catch (const std::exception &e) {
        cout << "Erro na thread CLI: " << e.what() << endl;
        running.store(false);
        facade->quit();
    }
}

void Client::showMainMenu() {
    clearScreen();
    cout << "========================================" << endl;
    cout << "           MENU PRINCIPAL" << endl;
    cout << "========================================" << endl;
    cout << "SHAs Ativos: " << sha_instances.size() << endl;
    cout << "----------------------------------------" << endl;
    cout << "1. Configurar Simulador SHA" << endl;
    cout << "2. Criar Nova Instância SHA" << endl;
    cout << "3. Finalizar Instância SHA" << endl;
    cout << "4. Modificar Vazão SHA" << endl;
    cout << "5. Habilitar Geração de Imagem" << endl;
    cout << "6. Listar SHAs Ativos" << endl;
    cout << "0. Sair" << endl;
    cout << "========================================" << endl;
}

void Client::configSimulatorMenu() {
    clearScreen();
    cout << "=== CONFIGURAÇÃO DO SIMULADOR SHA ===" << endl;
    cout << "Escolha o tipo de configuração:" << endl;
    cout << "1. Carregar configuração de arquivo (padrão)" << endl;
    cout << "2. Configuração personalizada" << endl;
    cout << "3. Configurar apenas tempo de tick" << endl;
    cout << "4. Configurar apenas parâmetros de fluxo" << endl;
    
    int choice = getIntInput("Digite sua escolha: ", 1, 4);
    bool success = false;
    
    switch (choice) {
        case 1: {
            string filename = getStringInput("Digite o nome do arquivo (Enter para padrão): ");
            if (filename.empty()) {
                success = facade->configSimulatorSHA();
            } else {
                success = facade->configSimulatorSHA(filename);
            }
            break;
        }
        case 2: {
            int tick_ms = getIntInput("Tempo de tick (ms): ", 100, 5000);
            double fluxo_inicial = getDoubleInput("Fluxo inicial (L/min): ", 0.0, 200.0);
            double fluxo_min = getDoubleInput("Fluxo mínimo (L/min): ", 0.0, 50.0);
            double fluxo_max = getDoubleInput("Fluxo máximo (L/min): ", fluxo_min, 500.0);
            
            success = facade->configSimulatorSHA(tick_ms, fluxo_inicial, fluxo_min, fluxo_max);
            break;
        }
        case 3: {
            int tick_ms = getIntInput("Novo tempo de tick (ms): ", 100, 5000);
            success = facade->setParametroTempo(tick_ms);
            break;
        }
        case 4: {
            double fluxo_inicial = getDoubleInput("Fluxo inicial (L/min): ", 0.0, 200.0);
            double fluxo_min = getDoubleInput("Fluxo mínimo (L/min): ", 0.0, 50.0);
            double fluxo_max = getDoubleInput("Fluxo máximo (L/min): ", fluxo_min, 500.0);
            
            success = facade->setParametroFluxo(fluxo_inicial, fluxo_min, fluxo_max);
            break;
        }
    }
    
    if (success) {
        cout << "✓ Configuração aplicada com sucesso!" << endl;
    } else {
        cout << "✗ Erro ao aplicar configuração!" << endl;
    }
}

void Client::createSHAMenu() {
    clearScreen();
    cout << "=== CRIAR NOVA INSTÂNCIA SHA ===" << endl;
    
    int sha_id = facade->criaSHA(false);
    
    if (sha_id >= 0) {
        addSHAInstance(sha_id);
        cout << "✓ SHA criado com sucesso!" << endl;
        cout << "ID da Instância: " << sha_id << endl;
        cout << "Total de SHAs ativos: " << sha_instances.size() << endl;
    } else {
        cout << "✗ Erro ao criar SHA!" << endl;
    }
}

void Client::finalizeSHAMenu() {
    if (sha_instances.empty()) {
        cout << "Nenhuma instância SHA ativa para finalizar." << endl;
        return;
    }
    
    clearScreen();
    cout << "=== FINALIZAR INSTÂNCIA SHA ===" << endl;
    listActiveSHAs();
    
    int sha_id = getIntInput("Digite o ID da instância SHA para finalizar: ");
    
    if (!isValidSHAId(sha_id)) {
        cout << "✗ ID de SHA inválido!" << endl;
        return;
    }
    
    bool success = facade->finalizaSHA(sha_id);
    
    if (success) {
        removeSHAInstance(sha_id);
        cout << "✓ SHA " << sha_id << " finalizado com sucesso!" << endl;
        cout << "SHAs restantes: " << sha_instances.size() << endl;
    } else {
        cout << "✗ Erro ao finalizar SHA " << sha_id << "!" << endl;
    }
}

void Client::modifyFlowMenu() {
    if (sha_instances.empty()) {
        cout << "Nenhuma instância SHA ativa para modificar." << endl;
        return;
    }
    
    clearScreen();
    cout << "=== MODIFICAR VAZÃO SHA ===" << endl;
    listActiveSHAs();
    
    int sha_id = getIntInput("Digite o ID da instância SHA: ");
    
    if (!isValidSHAId(sha_id)) {
        cout << "✗ ID de SHA inválido!" << endl;
        return;
    }
    
    double nova_vazao = getDoubleInput("Digite a nova vazão (L/min): ", 0.0, 500.0);
    
    bool success = facade->modificaVazaoSHA(sha_id, nova_vazao);
    
    if (success) {
        cout << "✓ Vazão do SHA " << sha_id << " alterada para " 
             << nova_vazao << " L/min!" << endl;
    } else {
        cout << "✗ Erro ao modificar vazão do SHA " << sha_id << "!" << endl;
    }
}

void Client::enableImageGenerationMenu() {
    if (sha_instances.empty()) {
        cout << "Nenhuma instância SHA ativa para habilitar geração de imagem." << endl;
        return;
    }
    
    clearScreen();
    cout << "=== HABILITAR GERAÇÃO DE IMAGEM ===" << endl;
    listActiveSHAs();
    
    int sha_id = getIntInput("Digite o ID da instância SHA: ");
    
    if (!isValidSHAId(sha_id)) {
        cout << "✗ ID de SHA inválido!" << endl;
        return;
    }
    
    bool success = facade->habilitaGeracaoImagemSHA(sha_id);
    
    if (success) {
        cout << "✓ Geração de imagem habilitada para SHA " << sha_id << "!" << endl;
        cout << "A interface gráfica foi criada e deve estar totalmente responsiva." << endl;
        cout << "Qt está executando na thread principal dedicada para máxima performance." << endl;
    } else {
        cout << "✗ Erro ao habilitar geração de imagem para SHA " << sha_id << "!" << endl;
    }
}

void Client::listActiveSHAs() {
    clearScreen();
    cout << "=== INSTÂNCIAS SHA ATIVAS ===" << endl;
    
    if (sha_instances.empty()) {
        cout << "Nenhuma instância SHA ativa." << endl;
        return;
    }
    
    cout << "Total de instâncias: " << sha_instances.size() << endl;
    cout << "IDs das instâncias: ";
    for (size_t i = 0; i < sha_instances.size(); ++i) {
        cout << sha_instances[i];
        if (i < sha_instances.size() - 1) cout << ", ";
    }
    cout << endl;
}


// Métodos auxiliares
void Client::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void Client::pauseScreen() {
    cout << "Pressione Enter para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int Client::getIntInput(const string& prompt, int min, int max) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= min && value <= max) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cout << "Entrada inválida! Digite um número entre " << min << " e " << max << "." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double Client::getDoubleInput(const string& prompt, double min, double max) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= min && value <= max) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cout << "Entrada inválida! Digite um número entre " << min << " e " << max << "." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string Client::getStringInput(const string& prompt) {
    cout << prompt;
    string input;
    
    // Limpar o buffer antes de getline
    if (cin.peek() == '\n') {
        cin.ignore();
    }
    
    getline(cin, input);
    return input;
}

bool Client::isValidSHAId(int sha_id) {
    return find(sha_instances.begin(), sha_instances.end(), sha_id) != sha_instances.end();
}

void Client::addSHAInstance(int sha_id) {
    sha_instances.push_back(sha_id);
}

void Client::removeSHAInstance(int sha_id) {
    sha_instances.erase(
        remove(sha_instances.begin(), sha_instances.end(), sha_id),
        sha_instances.end()
    );
}
