#include "HidrometroUI.hpp"
#include <iostream>

HidrometroUI::HidrometroUI(QWidget *parent) 
    : QWidget(parent) {

    QString imagePath = QString::fromStdString(Routes::IMG_HIDROMETRO);
    fundo = QPixmap(imagePath);

    QString ponteiroPath = QString::fromStdString(Routes::IMG_PONT);
    ponteiro = QPixmap(ponteiroPath);
    ponteiro = ponteiro.scaled(20, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    if(fundo.isNull()) {
        std::cerr << "Erro ao carregar a imagem de fundo: " << imagePath.toStdString() << std::endl;
    }

    if (ponteiro.isNull()) {
        std::cerr << "Erro ao carregar a imagem do ponteiro: " << ponteiroPath.toStdString() << std::endl;
    }
    
    loadPointerConfig(Routes::CONFIG_PONT_FILE);

    setFixedSize(726, 479);
    
    setWindowTitle("Simulador de Hidrômetro");

    update();
}

void HidrometroUI::setVolume(double volume) {
    currentVolume = volume;
    update();
}

void HidrometroUI::drawDigits(QPainter &painter, double total_volume_l) {
    long long total_litros = static_cast<long long>(floor(total_volume_l));

    long long m3 = total_litros / 1000;
    int centenas_litros = (total_litros / 100) % 10;
    int dezenas_litros = (total_litros / 10) % 10;
    int unidades_litros = total_litros % 10;

    QFont font("Arial", 18, QFont::Bold);
    painter.setFont(font);
    painter.setPen(Qt::black);

    int x_offset = 307;
    int y_offset = 191;

    painter.drawText(x_offset, y_offset, QString::number((m3 / 100) % 10));
    painter.drawText(x_offset + 22, y_offset, QString::number((m3 / 10) % 10));
    painter.drawText(x_offset + 44, y_offset, QString::number(m3 % 10));

    painter.drawText(x_offset + 65, y_offset, QString::number(centenas_litros));

    painter.setPen(Qt::red);

    painter.drawText(x_offset + 87, y_offset, QString::number(dezenas_litros));
    painter.drawText(x_offset + 109, y_offset, QString::number(unidades_litros));

    painter.setPen(Qt::black);
}

void HidrometroUI::drawPointers(QPainter &painter, double total_volume_l) {
    long long total_litros = static_cast<long long>(floor(total_volume_l));
    int unidades_litros = total_litros % 10;
    double fracao = total_volume_l - floor(total_volume_l);
    int decimos = static_cast<int>(floor(fracao * 10));

    drawPointer(painter, unidades_litros, false);
    drawPointer(painter, decimos, true);
}

void HidrometroUI::drawPointer(QPainter &painter, int value, bool isDecimos) {
    if (ponteiro.isNull()) return;

    const PointerPosition &pos = isDecimos ? decimosPositions[value] : litrosPositions[value];

    painter.save();
    painter.translate(pos.x, pos.y);
    painter.rotate(pos.angle);
    painter.scale(0.7, 0.7);
    painter.drawPixmap(0, 0, ponteiro);
    painter.restore();
}

void HidrometroUI::savePointerConfig(const std::string& filename) {
    using json = nlohmann::json;

    json j;
    j["litros"] = json::array();
    j["decimos"] = json::array();

    for (int i = 0; i < 10; ++i) {
        j["litros"].push_back({
            {"x", litrosPositions[i].x},
            {"y", litrosPositions[i].y},
            {"angle", litrosPositions[i].angle}
        });
        j["decimos"].push_back({
            {"x", decimosPositions[i].x},
            {"y", decimosPositions[i].y},
            {"angle", decimosPositions[i].angle}
        });
    }

    std::ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
        qDebug() << "Configuração salva em:" << filename.c_str();
    } else {
        qDebug() << "Erro ao salvar configuração:" << filename.c_str();
    }
}

void HidrometroUI::loadPointerConfig(const std::string& filename) {
    using json = nlohmann::json;

    std::ifstream file(filename);
    if (!file.is_open()) {
        qDebug() << "Arquivo não encontrado:" << filename.c_str();
        return;
    }

    try {
        json j;
        file >> j;

        if (j.contains("litros") && j["litros"].size() == 10) {
            for (int i = 0; i < 10; ++i) {
                litrosPositions[i].x = j["litros"][i]["x"];
                litrosPositions[i].y = j["litros"][i]["y"];
                litrosPositions[i].angle = j["litros"][i]["angle"];
            }
        }

        if (j.contains("decimos") && j["decimos"].size() == 10) {
            for (int i = 0; i < 10; ++i) {
                decimosPositions[i].x = j["decimos"][i]["x"];
                decimosPositions[i].y = j["decimos"][i]["y"];
                decimosPositions[i].angle = j["decimos"][i]["angle"];
            }
        }

        qDebug() << "Configuração carregada de:" << filename.c_str();
        update();

    } catch (const std::exception& e) {
        qDebug() << "Erro ao carregar JSON:" << e.what();
    }
}

void HidrometroUI::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    if (!fundo.isNull()) {
        painter.drawPixmap(rect(), fundo);
    } else {
        painter.fillRect(rect(), Qt::lightGray);
    }

    drawDigits(painter, currentVolume);
    drawPointers(painter, currentVolume);
}