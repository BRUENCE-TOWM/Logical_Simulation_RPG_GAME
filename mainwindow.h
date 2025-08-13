#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QSet>
#include <QList>
#include <QKeyEvent>
#include "gatemanager.h"
#include <draggablegate.h>
#include "wireitem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_openChestButton_clicked();
    void on_combineButton_clicked();
    void on_testGateButton_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *player;
    QList<QGraphicsPixmapItem*> chests;
    QSet<QString> inventory;
    QList<WireItem*> wires;
    GateManager gateManager;
    int chapter = 1;
    void playFailureVideo() ;
    bool launchMiniGame(int gameType) ;
    void updateInventoryDisplay();
    void loadChapterMap();
    void advanceChapter();
    void addGateToScene(const QString &gateType);
    void connectGates(QGraphicsItem *start, QGraphicsItem *end);
    void spawnRandomChests();
    void checkForChestInteraction();
    QMap<int, QString> chestTypes;
    bool chestOpened[2];
    void startMiniGameAndAward(const QString &awardGateType);

};

#endif // MAINWINDOW_H
