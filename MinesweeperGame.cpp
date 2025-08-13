#include "MinesweeperGame.h"
#include <QGridLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QFont>

MinesweeperGame::MinesweeperGame(QWidget *parent)
    : QDialog(parent), size(5), mineCount(5), revealedSafe(0), passed(false) {
    setWindowTitle("扫雷小游戏");
    setFixedSize(400, 400);
    setupGame();
}

void MinesweeperGame::setupGame() {
    QGridLayout *layout = new QGridLayout(this);
    layout->setSpacing(2);
    int totalCells = size * size;

    isMine = QVector<bool>(totalCells, false);
    isRevealed = QVector<bool>(totalCells, false);
    buttons.clear();

    // 随机布雷
    for (int i = 0; i < mineCount;) {
        int idx = QRandomGenerator::global()->bounded(totalCells);
        if (!isMine[idx]) {
            isMine[idx] = true;
            ++i;
        }
    }

    // 创建按钮
    for (int i = 0; i < totalCells; ++i) {
        QPushButton *btn = new QPushButton(" ");
        btn->setFixedSize(60, 60);
        btn->setFont(QFont("Arial", 16, QFont::Bold));
        btn->setProperty("index", i);
        connect(btn, &QPushButton::clicked, this, &MinesweeperGame::reveal);
        layout->addWidget(btn, i / size, i % size);
        buttons.append(btn);
    }

    setLayout(layout);
}

void MinesweeperGame::reveal() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int idx = btn->property("index").toInt();
    if (isRevealed[idx]) return;

    isRevealed[idx] = true;

    if (isMine[idx]) {
        btn->setText("💣");
        btn->setStyleSheet("color: red;");
        QMessageBox::warning(this, "失败", "你踩到地雷了！");
        reject();
        return;
    }

    int count = countAdjacentMines(idx);
    if (count > 0) {
        btn->setText(QString::number(count));
        btn->setStyleSheet("color: blue; font-weight: bold;");
    } else {
        btn->setText(" ");
    }

    btn->setEnabled(false);
    revealedSafe++;

    if (revealedSafe == buttons.size() - mineCount) {
        QMessageBox::information(this, "成功", "你成功扫雷！");
        passed = true;
        accept();
    }
}

int MinesweeperGame::countAdjacentMines(int idx) const {
    int x = idx % size;
    int y = idx / size;
    int count = 0;

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx;
            int ny = y + dy;
            if (nx < 0 || nx >= size || ny < 0 || ny >= size) continue;
            int nIdx = ny * size + nx;
            if (isMine[nIdx]) count++;
        }
    }
    return count;
}

bool MinesweeperGame::isPassed() const {
    return passed;
}
