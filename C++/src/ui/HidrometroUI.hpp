#ifndef HIDROMETRO_UI_HPP
#define HIDROMETRO_UI_HPP

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QString>
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <math.h>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "../routes/routes.hpp"

class HidrometroUI : public QWidget {
    Q_OBJECT

public:
    HidrometroUI(double initialFlow, QWidget *parent = nullptr);
    void saveCurrentImage(int m3);
    void ajustarCentralizacaoFlow();
    void setFaltaAgua(bool falta);
    void drawDigits(QPainter &painter, double total_volume_l);
    void drawPointers(QPainter &painter, double total_volume_l);
    void drawPointer(QPainter &painter, int value, bool isDecimos);
    void setVolume(double volume);
    void savePointerConfig(const std::string& filename);
    void loadPointerConfig(const std::string& filename);

signals:
    void flowChanged(double newFlow);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap fundo;
    QPixmap ponteiro;
    double currentVolume = 0.0;

    double currentFlow = 0.0;
    bool faltaAgua = false;
    QPushButton *btnMais = nullptr;
    QPushButton *btnMenos = nullptr;
    QLabel *lblFlow = nullptr;
    QLabel *lblFlowValue = nullptr;
    QHBoxLayout *flowLayout = nullptr;
    QTimer *timerMais = nullptr;
    QTimer *timerMenos = nullptr;

    struct PointerPosition {
        int x;
        int y;
        double angle;
    };

    PointerPosition litrosPositions[10];
    PointerPosition decimosPositions[10];
};

#endif