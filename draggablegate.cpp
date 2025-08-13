#include <draggablegate.h>
#include <QGraphicsScene>
#include <QLineF>

DraggableGate::DraggableGate(const QPixmap &pixmap, const QString &gateType)
    : QGraphicsPixmapItem(pixmap), m_gateType(gateType) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptedMouseButtons(Qt::LeftButton);
}

QString DraggableGate::gateType() const {
    return m_gateType;
}

void DraggableGate::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    dragStartPos = event->pos();
    // 调用基类处理选中、拖动准备等
    QGraphicsPixmapItem::mousePressEvent(event);
}

void DraggableGate::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    // 计算鼠标相对按下时的偏移，得到新的位置
    QPointF delta = event->pos() - dragStartPos;
    QPointF newPos = pos() + delta;

    // 吸附阈值
    const qreal snapDistance = 5.0;

    QGraphicsScene *scene = this->scene();
    if (!scene) {
        // 没有场景时直接设置位置
        setPos(newPos);
        QGraphicsPixmapItem::mouseMoveEvent(event);
        return;
    }

    // 收集场景内所有可能的连接端口（QGraphicsEllipseItem）
    QList<QGraphicsEllipseItem*> ports;
    for (QGraphicsItem *item : scene->items()) {
        auto *ellipse = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
        if (!ellipse) continue;
        ports.append(ellipse);
    }

    // 当前Gate中心点，基于新位置计算（boundingRect的中心）
    QPointF gateCenter = newPos + boundingRect().center();

    // 遍历端口找距离最近且在阈值内的点，实现吸附
    for (QGraphicsEllipseItem *port : ports) {
        QPointF portCenter = port->sceneBoundingRect().center();
        if (QLineF(gateCenter, portCenter).length() < snapDistance) {
            // 计算吸附后Gate左上角的位置，使中心和端口重合
            QPointF offset = boundingRect().center();
            QPointF snappedPos = portCenter - offset;
            setPos(snappedPos);
            // 调用基类处理后直接返回，防止重复移动
            QGraphicsPixmapItem::mouseMoveEvent(event);
            return;
        }
    }

    // 没有吸附时，正常移动
    setPos(newPos);
    QGraphicsPixmapItem::mouseMoveEvent(event);
}
