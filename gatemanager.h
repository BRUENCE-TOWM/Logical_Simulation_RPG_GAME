#ifndef GATEMANAGER_H
#define GATEMANAGER_H

#include <QString>
#include <QSet>

class GateManager {
public:
    bool canCombine(const QSet<QString> &inventory, const QString &resultGate);
};

#endif // GATEMANAGER_H
