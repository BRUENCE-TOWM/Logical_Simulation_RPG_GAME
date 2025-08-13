#include "logiceditordialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QGraphicsView>
#include <QMessageBox>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QDebug>
#include <QRandomGenerator>

LogicEditorDialog::LogicEditorDialog(const QSet<QString> &availableGates, QWidget *parent)
    : QDialog(parent), completed(false), selectedPoint(nullptr), dragStartPoint(nullptr), tempLine(nullptr),
    inputA(nullptr), inputB(nullptr), output(nullptr) {

    setWindowTitle("拼图编辑器 - 合成目标: OR门");
    resize(600, 400);

    QVBoxLayout *layout = new QVBoxLayout(this);

    // 顶部按钮栏
    QHBoxLayout *buttonBar = new QHBoxLayout();

    // 你传入的availableGates中可能包含 "AND" 和 "NOT"
    // 这里额外添加两个按钮，确保无论availableGates如何都支持
    QStringList fixedGates = {"AND", "NOT"};

    // 先添加availableGates里的按钮
    for (const QString &gate : availableGates) {
        QPixmap icon(QString(":/assets/icons/%1_gate.png").arg(gate.toLower()));
        if (icon.isNull()) {
            qWarning() << "未找到图标：" << gate;
            continue;
        }
        QToolButton *btn = new QToolButton();
        btn->setIcon(QIcon(icon));
        btn->setIconSize(QSize(32, 32));
        btn->setToolTip(gate);
        buttonBar->addWidget(btn);

        connect(btn, &QToolButton::clicked, this, [=]() {
            auto *gateItem = new DraggableGate(icon, gate);
            gateItem->setPos(100 + QRandomGenerator::global()->bounded(200),
                             100 + QRandomGenerator::global()->bounded(100));
            gateItem->setData(0, gate);
            gateItem->setFlag(QGraphicsItem::ItemIsMovable);
            gateItem->setFlag(QGraphicsItem::ItemIsSelectable);  // ✅ 可选中

            editScene->addItem(gateItem);
            placedGates.append(gateItem);

        });
    }

    // 如果availableGates中没有 AND 或 NOT，补加上这两个按钮
    for (const QString &fixedGate : fixedGates) {
        if (!availableGates.contains(fixedGate)) {
            QPixmap icon(QString(":/assets/icons/%1_gate.png").arg(fixedGate.toLower()));
            if (!icon.isNull()) {
                QToolButton *btn = new QToolButton();
                btn->setIcon(QIcon(icon));
                btn->setIconSize(QSize(32, 32));
                btn->setToolTip(fixedGate);
                buttonBar->addWidget(btn);

                connect(btn, &QToolButton::clicked, this, [=]() {
                    auto *gateItem = new DraggableGate(icon, fixedGate);
                    gateItem->setPos(100 + QRandomGenerator::global()->bounded(200),
                                     100 + QRandomGenerator::global()->bounded(100));
                    gateItem->setData(0, fixedGate);
                    editScene->addItem(gateItem);
                    placedGates.append(gateItem);
                });
            }
        }
    }

    // 输入输出点按钮
    QPushButton *addInput1Btn = new QPushButton("输入点1");
    QPushButton *addInput2Btn = new QPushButton("输入点2");
    QPushButton *addOutputBtn = new QPushButton("输出点");
    buttonBar->addWidget(addInput1Btn);
    buttonBar->addWidget(addInput2Btn);
    buttonBar->addWidget(addOutputBtn);

    connect(addInput1Btn, &QPushButton::clicked, this, [this]() {
        // 删除旧的
        if (inputA) {
            editScene->removeItem(inputA);
            inputSignals.remove(inputA);
            delete inputA;
            inputA = nullptr;
        }
        // 新建输入点1（初始为0，红色）
        inputA = editScene->addEllipse(20, 200, 20, 20, QPen(Qt::blue), QBrush(Qt::red));
        inputSignals[inputA] = 0;
    });

    connect(addInput2Btn, &QPushButton::clicked, this, [this]() {
        if (inputB) {
            editScene->removeItem(inputB);
            inputSignals.remove(inputB);
            delete inputB;
            inputB = nullptr;
        }
        inputB = editScene->addEllipse(20, 250, 20, 20, QPen(Qt::blue), QBrush(Qt::red));
        inputSignals[inputB] = 0;
    });

    connect(addOutputBtn, &QPushButton::clicked, this, [this]() {
        if (output) {
            editScene->removeItem(output);
            delete output;
            output = nullptr;
        }
        output = editScene->addEllipse(550, 225, 20, 20, QPen(Qt::red), QBrush(Qt::red));
    });

    layout->addLayout(buttonBar);

    // 编辑区
    editScene = new QGraphicsScene(this);
    QGraphicsView *view = new QGraphicsView(editScene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setMinimumHeight(300);
    layout->addWidget(view);

    view->viewport()->installEventFilter(this);
    view->setFocusPolicy(Qt::StrongFocus);

    // 测试按钮
    QPushButton *testBtn = new QPushButton("测试组合结果");
    testBtn->setStyleSheet("font-weight: bold; padding: 8px;");
    layout->addWidget(testBtn);
    connect(testBtn, &QPushButton::clicked, this, &LogicEditorDialog::testForORCompletion);
}

bool LogicEditorDialog::isCompleted() const {
    return completed;
}

void LogicEditorDialog::testForORCompletion() {
    int fromInputA = 0, fromInputB = 0, toOutput = 0;
    for (auto *item : editScene->items()) {
        auto *line = qgraphicsitem_cast<QGraphicsLineItem*>(item);
        if (!line) continue;
        QPointF p1 = line->line().p1();
        QPointF p2 = line->line().p2();
        if (inputA && (inputA->contains(inputA->mapFromScene(p1)) || inputA->contains(inputA->mapFromScene(p2))))
            fromInputA++;
        if (inputB && (inputB->contains(inputB->mapFromScene(p1)) || inputB->contains(inputB->mapFromScene(p2))))
            fromInputB++;
        if (output && (output->contains(output->mapFromScene(p1)) || output->contains(output->mapFromScene(p2))))
            toOutput++;
    }

    if ((fromInputA > 0 || fromInputB > 0) && toOutput > 0) {
        completed = true;
        accept();
    } else {
        QMessageBox::warning(this, "组合失败", "请连接至少一个输入点和输出点形成 OR 门逻辑");
    }
}

bool LogicEditorDialog::eventFilter(QObject *obj, QEvent *event) {
    auto *view = qobject_cast<QGraphicsView*>(obj->parent());
    if (!view) return QObject::eventFilter(obj, event);

    if (event->type() == QEvent::MouseButtonPress) {
        auto *me = static_cast<QMouseEvent*>(event);
        QPointF scenePos = view->mapToScene(me->pos());
        QGraphicsItem *clickedItem = editScene->itemAt(scenePos, QTransform());
        bool ctrlPressed = me->modifiers() & Qt::ControlModifier;
        if (me->button() == Qt::LeftButton) {

            if (clickedItem && (clickedItem == inputA || clickedItem == inputB || clickedItem == output || dynamic_cast<DraggableGate*>(clickedItem))) {

                if (dynamic_cast<DraggableGate*>(clickedItem) && !ctrlPressed) {
                    return false;  // 👉 不按 Ctrl 点 Gate → 允许拖动
                }

                // 👉 按了 Ctrl，或点击的是输入/输出点 → 处理连线
                if (!selectedPoint) {
                    selectedPoint = clickedItem;
                } else if (selectedPoint != clickedItem) {
                    auto *line = editScene->addLine(QLineF(
                                                        selectedPoint->sceneBoundingRect().center(),
                                                        clickedItem->sceneBoundingRect().center()), QPen(Qt::black, 2));
                    selectedPoint = nullptr;
                }
                return true;
            }

        } else if (me->button() == Qt::RightButton) {
            if (tempLine) {
                editScene->removeItem(tempLine);
                delete tempLine;
                tempLine = nullptr;
                dragStartPoint = nullptr;
            }
            return true;
        }
    } else if (event->type() == QEvent::MouseMove) {
        if (dragStartPoint && tempLine) {
            auto *me = static_cast<QMouseEvent*>(event);
            QPointF scenePos = view->mapToScene(me->pos());
            QLineF newLine(dragStartPoint->sceneBoundingRect().center(), scenePos);
            tempLine->setLine(newLine);
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        if (dragStartPoint && tempLine) {
            auto *me = static_cast<QMouseEvent*>(event);
            QPointF scenePos = view->mapToScene(me->pos());

            for (auto *point : {inputA, inputB, output}) {
                if (point && point != dragStartPoint && point->contains(point->mapFromScene(scenePos))) {
                    editScene->addLine(QLineF(dragStartPoint->sceneBoundingRect().center(),
                                              point->sceneBoundingRect().center()), QPen(Qt::black, 2));
                    break;
                }
            }

            editScene->removeItem(tempLine);
            delete tempLine;
            tempLine = nullptr;
            dragStartPoint = nullptr;
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}

void LogicEditorDialog::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_H) {
        QList<QGraphicsItem*> itemsToDelete;
        for (auto *item : editScene->items()) {
            if (qgraphicsitem_cast<QGraphicsLineItem*>(item) ||
                qgraphicsitem_cast<DraggableGate*>(item)) {
                itemsToDelete.append(item);
            }
        }
        for (auto *item : itemsToDelete) {
            editScene->removeItem(item);
            delete item;
        }
        // 删除输入输出点，并清理信号映射
        if (inputA) {
            editScene->removeItem(inputA);
            delete inputA;
            inputA = nullptr;
        }
        if (inputB) {
            editScene->removeItem(inputB);
            delete inputB;
            inputB = nullptr;
        }
        if (output) {
            editScene->removeItem(output);
            delete output;
            output = nullptr;
        }
        inputSignals.clear();
    }

    if (event->key() == Qt::Key_Q) {
        for (auto it = inputSignals.begin(); it != inputSignals.end(); ++it) {
            QGraphicsEllipseItem *item = it.key();
            if (!item) continue;
            int val = it.value();
            val = 1 - val;
            it.value() = val;
            item->setBrush(val ? QBrush(Qt::blue) : QBrush(Qt::red));
        }
        return; // 不继续传递事件
    }

    // 其他按键可自行添加
    QDialog::keyPressEvent(event);
}
