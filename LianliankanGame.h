#ifndef LIANLIANKANGAME_H
#define LIANLIANKANGAME_H

#include <QDialog>
#include <QPushButton>
#include <QGridLayout>
#include <QTimer>

class LianliankanGame : public QDialog {
    Q_OBJECT
public:
    explicit LianliankanGame(QWidget *parent = nullptr);

private:
    QGridLayout *gridLayout;
    QVector<QPushButton*> buttons;
    QPushButton *firstCard = nullptr, *secondCard = nullptr;
    QString firstType, secondType;
    int matchedPairs = 0;

    void createCards();
    void handleCardClicked(QPushButton *btn);
};

#endif // LIANLIANKANGAME_H
