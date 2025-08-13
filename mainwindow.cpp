#include <mainwindow.h>
#include "ui_mainwindow.h"
#include "draggablegate.h"
#include <QMessageBox>
#include "wireitem.h"
#include <logiceditordialog.h>
#include <Qtime>
#include <QRandomGenerator>
#include <algorithm>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QDialog>
#include <QVBoxLayout>
#include "LianliankanGame.h"
#include "MinesweeperGame.h"
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    connect(ui->combineButton_5, &QPushButton::clicked, this, &MainWindow::on_combineButton_clicked);
    scene = new QGraphicsScene(this);
    ui->graphicsView_5->setScene(scene);
    player = scene->addPixmap(QPixmap(":/assets/icons/player.png"));
    player->setPos(100, 100);
    loadChapterMap();
    QPixmap chestPixmap(":/assets/icons/chest.png");
    QStringList gateOptions = {"AND", "NOT"};
    std::shuffle(gateOptions.begin(), gateOptions.end(), *QRandomGenerator::global());

    for (int i = 0; i < 2; ++i) {
        int x = 100 + QRandomGenerator::global()->bounded(400);
        int y = 100 + QRandomGenerator::global()->bounded(300);
        QGraphicsPixmapItem *chest = new QGraphicsPixmapItem(chestPixmap);
        chest->setPos(x, y);
        chest->setZValue(1);
        scene->addItem(chest);
        chests.append(chest);
        chestOpened[i] = false;
        chestTypes[i] = gateOptions[i];  // 保存每个宝箱奖励类型
    }


}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::loadChapterMap() {
    scene->clear();
    QPixmap map;
    if (chapter == 1) map.load(":/assets/maps/scene1.png");
    if (chapter == 2) map.load(":/assets/maps/scene2.png");
    if (chapter == 3) map.load(":/assets/maps/scene3.png");
    scene->addPixmap(map);
    player = scene->addPixmap(QPixmap(":/assets/icons/player.png"));
    player->setPos(100, 100);
}

void MainWindow::advanceChapter() {
    chapter++;
    inventory.clear();
    updateInventoryDisplay();
    loadChapterMap();
    QMessageBox::information(this, "章节更新", QString("进入第%1章！").arg(chapter));
}

void MainWindow::on_openChestButton_clicked() {
    if (chapter == 1) {
        inventory.insert("AND");
        inventory.insert("NOT");
        addGateToScene("AND");
        addGateToScene("NOT");
    } else if (chapter == 2) {
        inventory.insert("XOR");
        inventory.insert("HALF-ADDER");
        addGateToScene("XOR");
        addGateToScene("HALF-ADDER");
    } else if (chapter == 3) {
        inventory.insert("D-FLIP-FLOP");
        inventory.insert("CLOCK");
        addGateToScene("D-FLIP-FLOP");
        addGateToScene("CLOCK");
    }
    updateInventoryDisplay();
    QMessageBox::information(this, "宝箱", "你获得了一些新元器件！");
}

void MainWindow::on_combineButton_clicked() {
    if (chapter == 1) {
        LogicEditorDialog editor(inventory, this);
        if (editor.exec() == QDialog::Accepted && editor.isCompleted()) {
            inventory.insert("OR");
            QMessageBox::information(this, "合成成功", "你已成功合成 OR 门，已加入背包");
            updateInventoryDisplay();
        }
    } else if (chapter == 2) {
        if (inventory.contains("XOR") && inventory.contains("HALF-ADDER")) {
            inventory.insert("FULL-ADDER");
            addGateToScene("FULL-ADDER");
            QMessageBox::information(this, "合成成功", "你合成了 FULL ADDER！");
        }
    } else if (chapter == 3) {
        if (inventory.contains("D-FLIP-FLOP") && inventory.contains("CLOCK")) {
            inventory.insert("1-BIT-CLOCK");
            addGateToScene("1-BIT-CLOCK");
            QMessageBox::information(this, "合成成功", "你完成了一位时钟电路！");
        }
    }
    updateInventoryDisplay();
}

void MainWindow::on_testGateButton_clicked() {
    QList<QGraphicsItem*> items = scene->items();
    int found = 0;
    for (QGraphicsItem *item : items) {
        auto *gate = dynamic_cast<DraggableGate*>(item);
        if (gate) {
            if ((chapter == 1 && gate->gateType() == "OR") ||
                (chapter == 2 && gate->gateType() == "FULL-ADDER") ||
                (chapter == 3 && gate->gateType() == "1-BIT-CLOCK")) {
                found++;
            }
        }
    }
    if (found > 0) {
        QMessageBox::information(this, "通关", "章节完成，进入下一章！");
        advanceChapter();
    } else {
        QMessageBox::warning(this, "未完成", "你还未构建指定电路。");
    }
}

void MainWindow::updateInventoryDisplay() {
    ui->inventoryList_5->clear();
    for (const QString &item : inventory) {
        ui->inventoryList_5->addItem(item);
    }
}

void MainWindow::addGateToScene(const QString &gateType) {
    QString iconPath = QString(":/icons/%1.png").arg(gateType.toLower().replace("-", "_"));
    QPixmap pix(iconPath);
    auto *gate = new DraggableGate(pix, gateType);
    gate->setPos(player->pos().x() + 100, player->pos().y());
    scene->addItem(gate);
}
void MainWindow::connectGates(QGraphicsItem *start, QGraphicsItem *end) {
    auto *wire = new WireItem(start, end);
    scene->addItem(wire);
    wires.append(wire);
}
void MainWindow::keyPressEvent(QKeyEvent *event) {
    int step = 10;
    QPointF pos = player->pos();
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_A:
        player->setPos(pos.x() - step, pos.y());
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        player->setPos(pos.x() + step, pos.y());
        break;
    case Qt::Key_Up:
    case Qt::Key_W:
        player->setPos(pos.x(), pos.y() - step);
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        player->setPos(pos.x(), pos.y() + step);
        break;
    case Qt::Key_F:
        checkForChestInteraction();
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::playFailureVideo() {
    QString videoPath = "C:/qt_test/GAME/assets/videos/failure.mp4"; // 请修改为你实际的绝对路径

    if (!QFile::exists(videoPath)) {
        QMessageBox::critical(this, "错误", "找不到视频文件:\n" + videoPath);
        return;
    }

    QMediaPlayer *player = new QMediaPlayer(this);
    QVideoWidget *videoWidget = new QVideoWidget;

    player->setVideoOutput(videoWidget);
    player->setSource(QUrl::fromLocalFile(videoPath));

    QDialog *videoDialog = new QDialog(this);
    QVBoxLayout *layout = new QVBoxLayout(videoDialog);
    layout->addWidget(videoWidget);
    videoDialog->setLayout(layout);
    videoDialog->resize(640, 480);

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            videoDialog->accept(); // 播放完成后关闭对话框
        }
    });

    player->play();
    videoDialog->exec();

    player->stop();
    videoWidget->deleteLater();
    player->deleteLater();
}



// 修改后的开宝箱逻辑
void MainWindow::checkForChestInteraction() {
    for (int i = 0; i < chests.size(); ++i) {
        if (chests[i] && !chestOpened[i] && player->collidesWithItem(chests[i])) {
            QString gateAward = chestTypes[i];
            scene->removeItem(chests[i]);
            delete chests[i];
            chestOpened[i] = true;
            startMiniGameAndAward(gateAward);  // 启动小游戏并奖励
        }

    }
}
void MainWindow::startMiniGameAndAward(const QString &awardGateType) {
    bool success = false;

    while (true) {
        QWidget *miniGame = nullptr;

        // 随机选择小游戏
        if (QRandomGenerator::global()->bounded(2) == 0) {
            auto *game = new LianliankanGame(this);
            miniGame = game;
            success = (game->exec() == QDialog::Accepted);
            delete game;
        } else {
            auto *game = new MinesweeperGame(this);
            miniGame = game;
            success = (game->exec() == QDialog::Accepted);
            delete game;
        }

        if (success) {
            inventory.insert(awardGateType);
            updateInventoryDisplay();
            QMessageBox::information(this, "奖励", QString("你获得了 %1 元器件！").arg(awardGateType));
            break;
        } else {
            playFailureVideo();
            auto choice = QMessageBox::question(
                this,
                "失败",
                "挑战失败，是否重试？",
                QMessageBox::Yes | QMessageBox::No
                );
            if (choice == QMessageBox::No) break;
        }
    }
}
