#include "HidrometroUI.hpp"
#include <iostream>
#include <QImage>
#include <QDir>
#include <QPainter>

void HidrometroUI::saveCurrentImage(int m3) {
    QString dirName = QString("Medições_202311250036");
    QDir dir;
    if (!dir.exists(dirName)) {
        dir.mkpath(dirName);
    }
    QString fileName = QString("%1/%2.jpeg").arg(dirName).arg(m3, 2, 10, QChar('0'));

    QImage image(this->size(), QImage::Format_RGB32);
    QPainter painter(&image);
    if (!this->fundo.isNull()) {
        painter.drawPixmap(this->rect(), this->fundo);
    } else {
        painter.fillRect(this->rect(), Qt::lightGray);
    }
    this->drawDigits(painter, this->currentVolume);
    this->drawPointers(painter, this->currentVolume);
    painter.end();
    image.save(fileName, "JPEG");
    qDebug() << "Imagem salva em:" << fileName;
}

HidrometroUI::HidrometroUI(double initialFlow, QWidget *parent)
    : QWidget(parent), currentFlow(initialFlow) {
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
    setWindowTitle("SHA");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    lblFlow = new QLabel("Vazão:", this);
    lblFlowValue = new QLabel(QString("%1 mm").arg(currentFlow), this);
    flowLayout = new QHBoxLayout();
    flowLayout->setAlignment(Qt::AlignCenter);
    flowLayout->addWidget(lblFlow);
    flowLayout->addWidget(lblFlowValue);
    btnMais = new QPushButton("+", this);
    btnMenos = new QPushButton("-", this);
    btnMais->setFixedSize(50, 32);
    btnMenos->setFixedSize(50, 32);
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(btnMenos);
    btnLayout->addLayout(flowLayout);
    btnLayout->addWidget(btnMais);
    mainLayout->addLayout(btnLayout);

    timerMais = new QTimer(this);
    timerMenos = new QTimer(this);
    timerMais->setInterval(100);
    timerMenos->setInterval(100);

    auto incFlow = [this]() {
        double maxFlow = 100.0;
    if (QFile::exists(QString::fromStdString(Routes::CONFIG_FILE))) {
            std::ifstream f(Routes::CONFIG_FILE);
            nlohmann::json j; f >> j;
            if (j.contains("fluxo_maximo_mm")) maxFlow = j["fluxo_maximo_mm"].get<double>();
        }
        if (currentFlow < maxFlow) {
            currentFlow += 1.0;
            if (currentFlow > maxFlow) currentFlow = maxFlow;
            lblFlowValue->setText(QString("%1 mm").arg(currentFlow));
            ajustarCentralizacaoFlow();
            emit flowChanged(currentFlow);
        }
    };
    auto decFlow = [this]() {
        currentFlow -= 1.0;
        if (currentFlow < 0) currentFlow = 0;
    lblFlowValue->setText(QString("%1 mm").arg(currentFlow));
    ajustarCentralizacaoFlow();
        emit flowChanged(currentFlow);
    };

    connect(btnMais, &QPushButton::pressed, this, [this, incFlow]() {
        incFlow();
        timerMais->start();
    });
    connect(btnMais, &QPushButton::released, this, [this]() {
        timerMais->stop();
    });
    connect(timerMais, &QTimer::timeout, this, incFlow);

    connect(btnMenos, &QPushButton::pressed, this, [this, decFlow]() {
        decFlow();
        timerMenos->start();
    });
    connect(btnMenos, &QPushButton::released, this, [this]() {
        timerMenos->stop();
    });
    connect(timerMenos, &QTimer::timeout, this, decFlow);

    update();
}

void HidrometroUI::ajustarCentralizacaoFlow() {
    lblFlow->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    lblFlowValue->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
}

void HidrometroUI::setVolume(double volume) {
    currentVolume = volume;
    update();
}

void HidrometroUI::setFaltaAgua(bool falta) {
    faltaAgua = falta;
    update();
}

void HidrometroUI::drawDigits(QPainter &painter, double total_volume_l) {
    long long litros_int = static_cast<long long>(floor(total_volume_l));

    int m3              = static_cast<int>(litros_int / 1000);
    int centenas_litros = (litros_int / 100) % 10;
    int dezenas_litros  = (litros_int / 10) % 10;

    QFont font("Arial", 18, QFont::Bold);
    painter.setFont(font);
    painter.setPen(Qt::black);

    int x_offset = 307;
    int y_offset = 191;

    painter.drawText(x_offset, y_offset, QString::number((m3 / 1000) % 10));
    painter.drawText(x_offset + 22, y_offset, QString::number((m3 / 100) % 10));
    painter.drawText(x_offset + 44, y_offset, QString::number((m3 / 10) % 10));
    painter.drawText(x_offset + 65, y_offset, QString::number((m3 % 10)));

    painter.setPen(Qt::red);
    painter.drawText(x_offset + 87, y_offset, QString::number(centenas_litros));
    painter.drawText(x_offset + 109, y_offset, QString::number(dezenas_litros));

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

    if (faltaAgua) {
        int raio = 18;
        int x = 20, y = height() - 40;
        painter.setBrush(QBrush(Qt::red));
        painter.setPen(Qt::red);
        painter.drawEllipse(x, y, raio, raio);
        painter.setPen(Qt::black);
        QFont font("Arial", 14, QFont::Bold);
        painter.setFont(font);
        painter.drawText(x + raio + 10, y + raio - 2, "Faltando água!");
    }
}