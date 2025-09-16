#include <QApplication>
#include <QTimer>
#include <QDebug>
#include "../routes/routes.hpp"
#include "../core/Controladora.hpp"
#include "../ui/HidrometroUI.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Configuracao cfg;
    if (cfg.load_from_file(Routes::CONFIG_FILE)) {
        qDebug() << "Configuracao carregada de:" << Routes::CONFIG_FILE.c_str();
    } else {
        qDebug() << "Nenhuma configuracao encontrada. Usando valores padrao.";
    }

    Controladora ctrl(cfg);
    ctrl.start();

    HidrometroUI ui(ctrl.getFluxoAtual());
    ui.show();

    QTimer *timer = new QTimer(&ui);
    static int lastSavedM3 = -1;
    QObject::connect(timer, &QTimer::timeout, [&]() {
        double currentVolume = ctrl.getVolumeTotal();
        ui.setVolume(currentVolume);
        bool faltaAgua = ctrl.isCurrentlyFailed();
        ui.setFaltaAgua(faltaAgua);
        int m3 = static_cast<int>(currentVolume / 1000.0);
        if (static_cast<int>((currentVolume - ctrl.getFluxoAtual() / 1000.0) / 1000.0) < m3 && m3 != lastSavedM3) {
            ui.saveCurrentImage(m3);
            lastSavedM3 = m3;
        }
    });
    timer->start(1000);

    QObject::connect(&ui, &HidrometroUI::flowChanged, [&](double newFlow) {
        // if (ctrl.isCurrentlyFailed()) return;
        if (newFlow > cfg.fluxo_maximo_mm) newFlow = cfg.fluxo_maximo_mm;
        ctrl.set_flow(newFlow);
    });

    QObject::connect(&ui, &HidrometroUI::destroyed, [&]() {
        ctrl.stop();
        qDebug() << "Simulacao encerrada.";
    });

    return app.exec();
}