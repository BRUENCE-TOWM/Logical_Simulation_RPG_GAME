#include "gatemanager.h"

bool GateManager::canCombine(const QSet<QString> &inventory, const QString &resultGate) {
    if (resultGate == "OR") {
        return inventory.contains("AND") && inventory.contains("NOT");
    } else if (resultGate == "XOR") {
        return inventory.contains("OR") && inventory.contains("AND") && inventory.contains("NOT");
    } else if (resultGate == "ADDER") {
        return inventory.contains("XOR") && inventory.contains("AND");
    } else if (resultGate == "CLOCK") {
        return inventory.contains("ADDER") && inventory.contains("NOT");
    }
    return false;
}
