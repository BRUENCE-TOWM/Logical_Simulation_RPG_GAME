#ifndef MINESWEEPERGAME_H
#define MINESWEEPERGAME_H

#include <QDialog>
#include <QPushButton>
#include <QVector>

class MinesweeperGame : public QDialog {
    Q_OBJECT
public:
    explicit MinesweeperGame(QWidget *parent = nullptr);
    bool isPassed() const;

private slots:
    void reveal();

private:
    void setupGame();
    int countAdjacentMines(int idx) const;

    int size;                      // 网格尺寸（5x5）
    int mineCount;                // 地雷数
    int revealedSafe;             // 已揭示安全格子数
    bool passed;                  // 游戏是否通关

    QVector<QPushButton*> buttons;  // 按钮集合
    QVector<bool> isMine;           // 是否是地雷
    QVector<bool> isRevealed;       // 是否被揭示
};

#endif // MINESWEEPERGAME_H
