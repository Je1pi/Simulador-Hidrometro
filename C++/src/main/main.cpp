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

    HidrometroUI ui;
    ui.show();

    QTimer *timer = new QTimer(&ui);
    QObject::connect(timer, &QTimer::timeout, [&]() {
        double currentVolume = ctrl.getVolumeTotal();
        ui.setVolume(currentVolume);
    });
    timer->start(1000);

    QObject::connect(&ui, &HidrometroUI::destroyed, [&]() {
        ctrl.stop();
        qDebug() << "Simulacao encerrada.";
    });

    return app.exec();
}