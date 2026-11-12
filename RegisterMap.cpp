#include "RegisterMap.h"

RegisterMap::RegisterMap(QObject *parent)
    : QObject(parent)
{
    for (quint8 i = 0x00; i <= 0x03; ++i) m_regs[i] = 0;
    for (quint8 i = 0x10; i <= 0x14; ++i) m_regs[i] = 0;
}

void RegisterMap::write(quint8 reg, quint8 value)
{
    if (m_regs.value(reg, 0) == value) return;
    m_regs[reg] = value;
    emit registerChanged(reg, value);
}

quint8 RegisterMap::read(quint8 reg) const
{
    return m_regs.value(reg, 0);
}
