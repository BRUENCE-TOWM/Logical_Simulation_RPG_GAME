#ifndef WIREITEM_H
#define WIREITEM_H

#include <QGraphicsLineItem>

class WireItem : public QGraphicsLineItem {
public:
    WireItem(QGraphicsItem *startItem, QGraphicsItem *endItem);
    void updatePosition();

private:
    QGraphicsItem *startItem;
    QGraphicsItem *endItem;
};

#endif // WIREITEM_H
