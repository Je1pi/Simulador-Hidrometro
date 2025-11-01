#ifndef SYSTEM_FACADE_HPP
#define SYSTEM_FACADE_HPP

#include <string>
#include <QApplication>
#include <QTimer>
#include <QDebug>
#include <QMetaObject>
#include <vector>
#include <memory>
#include <cstring>
#include <map>
#include "../routes/routes.hpp"
#include "../core/Controladora.hpp"
#include "../ui/HidrometroUI.hpp"

using namespace std;

class SystemFacade {
public:
    SystemFacade(int argc, char *argv[]){
        // Criar argumentos dummy seguros para o Qt
        static int dummy_argc = 1;
        static char app_name[] = "hidrometro_sim";
        static char* dummy_argv[] = { app_name, nullptr };
        
        this->app = std::make_unique<QApplication>(dummy_argc, dummy_argv);
        this->next_sha_id = 0;
        setNumberOfThreads(argc > 1 ? atoi(argv[1]) : 1);
        configSimulatorSHA();
    }

    ~SystemFacade() {
        // Parar todas as controladoras primeiro (thread-safe)
        for (std::unique_ptr<Controladora> &ctrl : this->ctrls) {
            if (ctrl) {
                ctrl->stop();
            }
        }
        
        // Usar deleteLater() para UIs - é thread-safe e evita problemas com timers
        for (std::unique_ptr<HidrometroUI> &ui : this->uis) {
            if (ui) {
                ui->deleteLater();
                ui.release(); // Liberar sem chamar destrutor
            }
        }
        
        this->ctrls.clear();
        this->uis.clear();
        this->id_to_index_map.clear();
    }

    bool configSimulatorSHA(string filename = Routes::CONFIG_FILE);
    
    // Métodos adicionais para configuração granular
    bool configSimulatorSHA(int tick_ms, double fluxo_inicial, double fluxo_min, double fluxo_max);
    bool setParametroTempo(int tick_ms);
    bool setParametroFluxo(double inicial, double minimo, double maximo);

    int criaSHA(bool display_state = true);

    bool finalizaSHA(int sha_id);

    bool modificaVazaoSHA(int sha_id, double novaVazao);

    bool habilitaGeracaoImagemSHA(int sha_id);
    
    // Método para executar o loop de eventos do Qt
    int exec();
    
    // Método para encerrar a aplicação
    void quit();

private:
    std::vector<std::unique_ptr<Controladora>> ctrls;
    std::vector<std::unique_ptr<HidrometroUI>> uis;
    std::map<int, int> id_to_index_map; // Mapeia ID para índice no vetor
    std::unique_ptr<QApplication> app;
    int numberOfThreads;
    int next_sha_id; // Próximo ID a ser atribuído
    Configuracao cfg;

    inline void setNumberOfThreads(int inputNumberOfThreads) {
        this->numberOfThreads = std::max(1, std::min(5, inputNumberOfThreads));
    }

    void createConectWindow(int shaIndex, Configuracao &cfg);
    void createTimerForSHA(int shaIndex);
    int getIndexFromId(int sha_id);
};

#endif // SYSTEM_FACADE_HPP