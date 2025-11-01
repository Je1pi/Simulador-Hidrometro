#include "../client/Client.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    try {
        std::cout << "Iniciando Cliente CLI do Simulador SHA..." << std::endl;
        
        // Criar cliente e iniciar CLI em thread separada
        Client client(argc, argv);
        
        // O método run() agora gerencia threads e executa o Qt na main thread
        return client.run();
        
    } catch (const std::exception &e) {
        std::cerr << "Erro fatal no cliente CLI: " << e.what() << std::endl;
        return -1;
    }
}