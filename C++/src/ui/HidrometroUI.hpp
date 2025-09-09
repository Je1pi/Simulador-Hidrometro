#ifndef HIDROMETRO_UI_HPP
#define HIDROMETRO_UI_HPP

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
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
    HidrometroUI(QWidget *parent = nullptr);
    void drawDigits(QPainter &painter, double total_volume_l);
    void drawPointers(QPainter &painter, double total_volume_l);
    void drawPointer(QPainter &painter, int value, bool isDecimos);
    void setVolume(double volume);
    void savePointerConfig(const std::string& filename);
    void loadPointerConfig(const std::string& filename);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap fundo;
    QPixmap ponteiro;
    double currentVolume = 0.0;

    struct PointerPosition {
        int x;
        int y;
        double angle;
    };

    PointerPosition litrosPositions[10];
    PointerPosition decimosPositions[10];
};

#endif