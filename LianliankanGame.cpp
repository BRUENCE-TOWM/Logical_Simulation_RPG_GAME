#include <QVBoxLayout>
#include <QRandomGenerator>
#include <QMessageBox>
#include <LianliankanGame.h>

LianliankanGame::LianliankanGame(QWidget *parent) : QDialog(parent) {
    setWindowTitle("连连看小游戏");
    resize(400, 400);

    gridLayout = new QGridLayout;
    createCards();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(gridLayout);
    setLayout(mainLayout);
}

void LianliankanGame::createCards() {
    QStringList symbols = {"A", "B", "C", "D", "E", "F", "G", "H"};
    symbols += symbols;  // 8 对

    std::shuffle(symbols.begin(), symbols.end(), *QRandomGenerator::global());

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            int idx = i * 4 + j;
            auto *btn = new QPushButton;
            btn->setFixedSize(80, 80);
            btn->setText("");
            btn->setProperty("type", symbols[idx]);
            connect(btn, &QPushButton::clicked, this, [=]() { handleCardClicked(btn); });
            gridLayout->addWidget(btn, i, j);
            buttons.append(btn);
        }
    }
}

void LianliankanGame::handleCardClicked(QPushButton *btn) {
    if (firstCard && secondCard) return;

    QString type = btn->property("type").toString();
    btn->setText(type);

    if (!firstCard) {
        firstCard = btn;
        firstType = type;
    } else if (btn != firstCard) {
        secondCard = btn;
        secondType = type;

        if (firstType == secondType) {
            QTimer::singleShot(300, this, [=]() {
                firstCard->setEnabled(false);
                secondCard->setEnabled(false);
                matchedPairs++;
                if (matchedPairs == 8) {
                    accept();  // 游戏通关
                }
                firstCard = secondCard = nullptr;
            });
        } else {
            QTimer::singleShot(600, this, [=]() {
                firstCard->setText("");
                secondCard->setText("");
                firstCard = secondCard = nullptr;
            });
        }
    }
}
