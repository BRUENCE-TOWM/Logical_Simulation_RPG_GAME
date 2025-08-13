#include "wireitem.h"
#include <QPen>

WireItem::WireItem(QGraphicsItem *start, QGraphicsItem *end)
    : startItem(start), endItem(end) {
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(2);
    setPen(pen);
    updatePosition();
}

void WireItem::updatePosition() {
    if (startItem && endItem) {
        QPointF p1 = startItem->sceneBoundingRect().center();
        QPointF p2 = endItem->sceneBoundingRect().center();
        setLine(QLineF(p1, p2));
    }
}
