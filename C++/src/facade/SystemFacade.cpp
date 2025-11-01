#include "SystemFacade.hpp"

using namespace std;

bool SystemFacade::configSimulatorSHA(string filename) {
    try {
        if(this->cfg.load_from_file(filename)){
            qDebug() << "Configuracao carregada com sucesso.";
            return true;
        } else {
            qDebug() << "Erro ao carregar configuracao.";
            return false;
        }
    } catch (const std::exception &e) {
        qDebug() << "Erro ao inicializar configuracao padrao:" << e.what();
        return false;
    }
}

bool SystemFacade::configSimulatorSHA(int tick_ms, double fluxo_inicial, double fluxo_min, double fluxo_max) {
    try {
        cfg.tick_ms = tick_ms;
        cfg.fluxo_inicial_mm = fluxo_inicial;
        cfg.fluxo_minimo_mm = fluxo_min;
        cfg.fluxo_maximo_mm = fluxo_max;
        qDebug() << "Configuracao personalizada aplicada com sucesso.";
        return true;
    } catch (const std::exception &e) {
        qDebug() << "Erro ao aplicar configuracao personalizada:" << e.what();
        return false;
    }
}

bool SystemFacade::setParametroTempo(int tick_ms) {
    try {
        cfg.tick_ms = tick_ms;
        qDebug() << "Parametro de tempo configurado para:" << tick_ms << "ms";
        return true;
    } catch (const std::exception &e) {
        qDebug() << "Erro ao configurar parametro de tempo:" << e.what();
        return false;
    }
}

bool SystemFacade::setParametroFluxo(double inicial, double minimo, double maximo) {
    try {
        cfg.fluxo_inicial_mm = inicial;
        cfg.fluxo_minimo_mm = minimo;
        cfg.fluxo_maximo_mm = maximo;
        qDebug() << "Parametros de fluxo configurados - Inicial:" << inicial 
                 << "Min:" << minimo << "Max:" << maximo;
        return true;
    } catch (const std::exception &e) {
        qDebug() << "Erro ao configurar parametros de fluxo:" << e.what();
        return false;
    }
}

int SystemFacade::criaSHA(bool display_state) {
    try {
        qDebug() << "Criando novo SHA com display_state:" << display_state;
        
        std::unique_ptr<Controladora> ctrl = std::make_unique<Controladora>(cfg, display_state);
        qDebug() << "Controladora criada, iniciando...";
        
        ctrls.push_back(std::move(ctrl));
        ctrls.back()->start();
        qDebug() << "Controladora iniciada com sucesso";
        
        // Verificar se a controladora está realmente funcionando
        double volume_inicial = ctrls.back()->getVolumeTotal();
        double fluxo_inicial = ctrls.back()->getFluxoAtual();
        qDebug() << "Volume inicial:" << volume_inicial << "Fluxo inicial:" << fluxo_inicial;
        
        // Criar UI como nullptr inicialmente - será criada quando necessário
        uis.push_back(nullptr);
        
        int index = ctrls.size() - 1;
        int sha_id = next_sha_id++;
        
        // Mapear o ID para o índice
        id_to_index_map[sha_id] = index;
        
        qDebug() << "SHA criado com ID:" << sha_id << "no índice:" << index;
        return sha_id;
    } catch (const std::exception &e) {
        qDebug() << "Erro ao criar SHA:" << e.what();
        return -1;
    }
}

int SystemFacade::getIndexFromId(int sha_id) {
    auto it = id_to_index_map.find(sha_id);
    if (it != id_to_index_map.end()) {
        return it->second;
    }
    return -1; // ID não encontrado
}

bool SystemFacade::finalizaSHA(int sha_id){
    try {
        // Converter ID para índice
        int index = getIndexFromId(sha_id);
        if(index < 0 || index >= static_cast<int>(ctrls.size())){
            qDebug() << "ID de SHA invalido para finalizar:" << sha_id;
            return false;
        }
        
        // Parar a controladora primeiro (thread-safe)
        ctrls[index]->stop();
        
        // Se existe UI, usar deleteLater() que é thread-safe
        if (index < static_cast<int>(uis.size()) && uis[index]) {
            uis[index]->deleteLater();
            uis[index].release(); // Liberar sem chamar destrutor
        }
        
        // Remover da lista de controladoras
        ctrls.erase(ctrls.begin() + index);
        
        // Ajustar vetor de UIs se necessário
        if (index < static_cast<int>(uis.size())) {
            uis.erase(uis.begin() + index);
        }
        
        // Remover do mapeamento
        id_to_index_map.erase(sha_id);
        
        // Atualizar os índices no mapeamento para todos os SHAs após o removido
        for (auto& pair : id_to_index_map) {
            if (pair.second > index) {
                pair.second--;
            }
        }
        
        return true;
    } catch (const std::exception &e) {
        qDebug() << "Erro ao finalizar SHA:" << e.what();
        return false;
    }
}

bool SystemFacade::modificaVazaoSHA(int sha_id, double novaVazao){
    try {
        // Converter ID para índice
        int index = getIndexFromId(sha_id);
        if(index < 0 || index >= static_cast<int>(ctrls.size())){
            qDebug() << "ID de SHA invalido para modificar vazao:" << sha_id;
            return false;
        }
        
        // A controladora é thread-safe, então podemos chamar diretamente
        ctrls[index]->set_flow(novaVazao);
        return true;
    } catch (const std::exception &e) {
        qDebug() << "Erro ao modificar vazao SHA:" << e.what();
        return false;
    }
}

bool SystemFacade::habilitaGeracaoImagemSHA(int sha_id){
    try {
        // Converter ID para índice
        int index = getIndexFromId(sha_id);
        if(index < 0 || index >= static_cast<int>(ctrls.size())){
            qDebug() << "ID de SHA invalido para habilitar geracao de imagem:" << sha_id;
            return false;
        }
        
        qDebug() << "Habilitando geração de imagem para SHA ID" << sha_id << "(índice" << index << ")";
        
        // Usar QMetaObject::invokeMethod para executar tudo na thread principal
        QMetaObject::invokeMethod(app.get(), [this, index]() {
            try {
                    // Criar UI se ainda não foi criada
                if (index >= static_cast<int>(uis.size()) || !uis[index]) {
                    // Redimensionar vetor se necessário
                    if (index >= static_cast<int>(uis.size())) {
                        uis.resize(index + 1);
                    }
                    
                    // Criar UI com valores atuais da controladora
                    double fluxoAtual = ctrls[index]->getFluxoAtual();
                    double volumeAtual = ctrls[index]->getVolumeTotal();
                    
                    uis[index] = std::make_unique<HidrometroUI>(fluxoAtual);
                    
                    // Configurar estado inicial da UI
                    uis[index]->setVolume(volumeAtual);
                    bool faltaAgua = ctrls[index]->isCurrentlyFailed();
                    uis[index]->setFaltaAgua(faltaAgua);
                    
                    // Criar timer diretamente na thread principal
                    createTimerForSHA(index);
                } else {
                    // Mesmo se a UI já existe, garantir que tenha valores atuais
                    double volumeAtual = ctrls[index]->getVolumeTotal();
                    bool faltaAgua = ctrls[index]->isCurrentlyFailed();
                    uis[index]->setVolume(volumeAtual);
                    uis[index]->setFaltaAgua(faltaAgua);
                }
                
                uis[index]->show();
                
            } catch (const std::exception &e) {
                qDebug() << "Erro ao criar UI na thread principal para SHA" << index << ":" << e.what();
            }
        }, Qt::QueuedConnection);
        
        return true;
    } catch (const std::exception &e) {
        qDebug() << "Erro ao habilitar geracao de imagem SHA:" << e.what();
        return false;
    }
}

void SystemFacade::createTimerForSHA(int shaIndex) {
    try {
        if (shaIndex < 0 || shaIndex >= static_cast<int>(ctrls.size()) || 
            shaIndex >= static_cast<int>(uis.size()) || !uis[shaIndex]) {
            return;
        }
        
        // Obter ponteiro da UI
        HidrometroUI* ui = uis[shaIndex].get();
        
        // Criar timer como filho da UI para garantir cleanup automático
        QTimer *timer = new QTimer(ui);
        
        // Usar shared_ptr para evitar problemas com referências
        std::shared_ptr<int> lastSavedM3 = std::make_shared<int>(-1);
        
        // Conectar o timer diretamente - agora estamos na thread principal
        QObject::connect(timer, &QTimer::timeout, [this, shaIndex, lastSavedM3]() {
            try {
                // Verificar se o índice ainda é válido e objetos existem
                if (shaIndex < 0 || shaIndex >= static_cast<int>(ctrls.size()) || 
                    shaIndex >= static_cast<int>(uis.size()) || 
                    !ctrls[shaIndex] || !uis[shaIndex]) {
                    return;
                }
                
                double currentVolume = this->ctrls[shaIndex]->getVolumeTotal();
                
                this->uis[shaIndex]->setVolume(currentVolume);
                bool faltaAgua = this->ctrls[shaIndex]->isCurrentlyFailed();
                this->uis[shaIndex]->setFaltaAgua(faltaAgua);
                
                int m3 = static_cast<int>(currentVolume / 1000.0);
                if (static_cast<int>((currentVolume - this->ctrls[shaIndex]->getFluxoAtual() / 1000.0) / 1000.0) < m3 && m3 != *lastSavedM3) {
                    this->uis[shaIndex]->saveCurrentImage(m3);
                    *lastSavedM3 = m3;
                }
            } catch (const std::exception &e) {
                qDebug() << "Erro no timer de atualização para SHA" << shaIndex << ":" << e.what();
            }
        });
        
        // Conectar signal flowChanged
        QObject::connect(ui, &HidrometroUI::flowChanged, [this, shaIndex](double newFlow) {
            try {
                // Verificar se o índice ainda é válido
                if (shaIndex < 0 || shaIndex >= static_cast<int>(ctrls.size()) || !ctrls[shaIndex]) {
                    return;
                }
                
                if (newFlow > cfg.fluxo_maximo_mm) newFlow = cfg.fluxo_maximo_mm;
                this->ctrls[shaIndex]->set_flow(newFlow);
            } catch (const std::exception &e) {
                qDebug() << "Erro ao alterar fluxo do SHA" << shaIndex << ":" << e.what();
            }
        });

        // Conectar signal destroyed
        QObject::connect(ui, &HidrometroUI::destroyed, [this, shaIndex]() {
            try {
                // Verificar se o índice ainda é válido
                if (shaIndex >= 0 && shaIndex < static_cast<int>(ctrls.size()) && ctrls[shaIndex]) {
                    this->ctrls[shaIndex]->stop();
                }
            } catch (const std::exception &e) {
                qDebug() << "Erro ao encerrar simulação do SHA" << shaIndex << ":" << e.what();
            }
        });
        
        // Fazer a primeira atualização imediatamente
        double currentVolume = this->ctrls[shaIndex]->getVolumeTotal();
        bool faltaAgua = this->ctrls[shaIndex]->isCurrentlyFailed();
        
        this->uis[shaIndex]->setVolume(currentVolume);
        this->uis[shaIndex]->setFaltaAgua(faltaAgua);
        
        timer->start(500);
        
    } catch (const std::exception &e) {
        qDebug() << "Erro ao criar timer para SHA" << shaIndex << ":" << e.what();
    }
}

void SystemFacade::createConectWindow(int shaIndex, Configuracao &cfg) {
    try {
        if (shaIndex < 0 || shaIndex >= static_cast<int>(ctrls.size()) || 
            shaIndex >= static_cast<int>(uis.size()) || !uis[shaIndex]) {
            qDebug() << "Índice inválido ou UI não existe:" << shaIndex;
            return;
        }
        
        qDebug() << "Criando conexões para SHA" << shaIndex;
        
        // Obter ponteiro da UI
        HidrometroUI* ui = uis[shaIndex].get();
        
        // Usar QMetaObject::invokeMethod para garantir execução na thread principal
        QMetaObject::invokeMethod(ui, [this, shaIndex, &cfg, ui]() {
            try {
                qDebug() << "Executando configuração de timer na thread principal para SHA" << shaIndex;
                
                // Criar timer como filho da UI para garantir cleanup automático
                QTimer *timer = new QTimer(ui);
                
                // Usar shared_ptr para evitar problemas com referências
                std::shared_ptr<int> lastSavedM3 = std::make_shared<int>(-1);
                
                qDebug() << "Configurando timer para SHA" << shaIndex;
                
                // Capturar apenas o índice e usar this para acessar os vetores
                QObject::connect(timer, &QTimer::timeout, [this, shaIndex, lastSavedM3]() {
                    try {
                        // Verificar se o índice ainda é válido e objetos existem
                        if (shaIndex < 0 || shaIndex >= static_cast<int>(ctrls.size()) || 
                            shaIndex >= static_cast<int>(uis.size()) || 
                            !ctrls[shaIndex] || !uis[shaIndex]) {
                            qDebug() << "SHA" << shaIndex << "não é mais válido no timer";
                            return;
                        }
                        
                        qDebug() << "Timer tick para SHA" << shaIndex;
                        
                        double currentVolume = this->ctrls[shaIndex]->getVolumeTotal();
                        qDebug() << "Volume atual SHA" << shaIndex << ":" << currentVolume;
                        
                        this->uis[shaIndex]->setVolume(currentVolume);
                        bool faltaAgua = this->ctrls[shaIndex]->isCurrentlyFailed();
                        this->uis[shaIndex]->setFaltaAgua(faltaAgua);
                        
                        int m3 = static_cast<int>(currentVolume / 1000.0);
                        if (static_cast<int>((currentVolume - this->ctrls[shaIndex]->getFluxoAtual() / 1000.0) / 1000.0) < m3 && m3 != *lastSavedM3) {
                            this->uis[shaIndex]->saveCurrentImage(m3);
                            *lastSavedM3 = m3;
                            qDebug() << "Imagem salva para SHA" << shaIndex << "M3:" << m3;
                        }
                    } catch (const std::exception &e) {
                        qDebug() << "Erro no timer de atualização para SHA" << shaIndex << ":" << e.what();
                    }
                });
                
                // Fazer a primeira atualização imediatamente
                qDebug() << "Fazendo atualização inicial da UI...";
                double currentVolume = this->ctrls[shaIndex]->getVolumeTotal();
                double currentFlow = this->ctrls[shaIndex]->getFluxoAtual();
                bool faltaAgua = this->ctrls[shaIndex]->isCurrentlyFailed();
                
                this->uis[shaIndex]->setVolume(currentVolume);
                this->uis[shaIndex]->setFaltaAgua(faltaAgua);
                qDebug() << "Atualização inicial concluída - Volume:" << currentVolume << "Fluxo:" << currentFlow << "Falta água:" << faltaAgua;
                
                QObject::connect(ui, &HidrometroUI::flowChanged, [this, shaIndex, &cfg](double newFlow) {
                    try {
                        // Verificar se o índice ainda é válido
                        if (shaIndex < 0 || shaIndex >= static_cast<int>(ctrls.size()) || !ctrls[shaIndex]) {
                            qDebug() << "SHA" << shaIndex << "não é mais válido no flowChanged";
                            return;
                        }
                        
                        // if (this->ctrls[shaIndex]->isCurrentlyFailed()) return;
                        if (newFlow > cfg.fluxo_maximo_mm) newFlow = cfg.fluxo_maximo_mm;
                        this->ctrls[shaIndex]->set_flow(newFlow);
                        qDebug() << "Fluxo do SHA" << shaIndex << "alterado para:" << newFlow;
                    } catch (const std::exception &e) {
                        qDebug() << "Erro ao alterar fluxo do SHA" << shaIndex << ":" << e.what();
                    }
                });

                QObject::connect(ui, &HidrometroUI::destroyed, [this, shaIndex]() {
                    try {
                        // Verificar se o índice ainda é válido
                        if (shaIndex >= 0 && shaIndex < static_cast<int>(ctrls.size()) && ctrls[shaIndex]) {
                            this->ctrls[shaIndex]->stop();
                            qDebug() << "Simulacao do SHA" << shaIndex << "encerrada.";
                        }
                    } catch (const std::exception &e) {
                        qDebug() << "Erro ao encerrar simulação do SHA" << shaIndex << ":" << e.what();
                    }
                });
                
                qDebug() << "Iniciando timer com intervalo de 500ms para melhor responsividade";
                timer->start(500);
                qDebug() << "Timer iniciado com sucesso para SHA" << shaIndex;
                
            } catch (const std::exception &e) {
                qDebug() << "Erro ao configurar timer na thread principal para SHA" << shaIndex << ":" << e.what();
            }
        }, Qt::QueuedConnection);
        
        qDebug() << "Conexões da UI estabelecidas com sucesso para SHA" << shaIndex;
    } catch (const std::exception &e) {
        qDebug() << "Erro ao criar janela de conexao para SHA" << shaIndex << ":" << e.what();
    }
}

int SystemFacade::exec() {
    try {
        if (!app) {
            qDebug() << "QApplication nao foi inicializada.";
            return -1;
        }
        qDebug() << "Iniciando loop de eventos do Qt...";
        return app->exec();
    } catch (const std::exception &e) {
        qDebug() << "Erro ao executar aplicacao:" << e.what();
        return -1;
    }
}

void SystemFacade::quit() {
    try {
        if (app) {
            qDebug() << "Encerrando aplicacao...";
            app->quit();
        }
    } catch (const std::exception &e) {
        qDebug() << "Erro ao encerrar aplicacao:" << e.what();
    }
}