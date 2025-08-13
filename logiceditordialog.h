#ifndef LOGICEDITORDIALOG_H
#define LOGICEDITORDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QVBoxLayout>
#include "draggablegate.h"
#include <wireitem.h>
#include <QGraphicsLineItem>
class LogicEditorDialog : public QDialog {
    Q_OBJECT
public:
    explicit LogicEditorDialog(const QSet<QString> &availableGates, QWidget *parent = nullptr);
    bool isCompleted() const;

private:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *player;
    QGraphicsPixmapItem *mapItem;
    QVector<QGraphicsPixmapItem*> chests;
    QGraphicsPixmapItem *chest1;
    QGraphicsPixmapItem *chest2;
    bool chestOpened[2];
    QGraphicsScene *editScene;
    QGraphicsEllipseItem *inputA, *inputB, *output;
    QGraphicsItem *selectedPoint = nullptr;

    QList<DraggableGate*> placedGates;
    QList<QGraphicsLineItem*> testWires;
    bool completed = false;

    void setupScene();
    void testForORCompletion();
protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    QGraphicsItem* dragStartPoint = nullptr;  // 当前拖拽起点
    QGraphicsLineItem* tempLine = nullptr;            // 临时线条显示拖拽过程
    QMap<QGraphicsEllipseItem*, int> inputSignals;

};

#endif // LOGICEDITORDIALOG_H
