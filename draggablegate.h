#ifndef DRAGGABLEGATE_H
#define DRAGGABLEGATE_H

#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QList>
#include <QBrush>
#include <QPen>
class ConnectionPoint : public QGraphicsEllipseItem {
public:
    enum PointType { Input, Output };

    ConnectionPoint(PointType type, QGraphicsItem *parent = nullptr)
        : QGraphicsEllipseItem(-5, -5, 10, 10, parent), pointType(type) {
        setBrush(type == Input ? QBrush(Qt::blue) : QBrush(Qt::red));
        setPen(QPen(Qt::NoPen));
        setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
        setAcceptHoverEvents(true);
    }

    PointType pointType;
};

class DraggableGate : public QGraphicsPixmapItem {
public:
    DraggableGate(const QPixmap &pixmap, const QString &gateType);

    QString gateType() const;

    QList<ConnectionPoint*> inputPoints() const { return m_inputPoints; }
    ConnectionPoint* outputPoint() const { return m_outputPoint; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QString m_gateType;
    QPointF dragStartPos;

    QList<ConnectionPoint*> m_inputPoints;
    ConnectionPoint* m_outputPoint = nullptr;

    void setupConnectionPoints();
};

#endif // DRAGGABLEGATE_H
