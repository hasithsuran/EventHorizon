// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

/**
 * @file RegisterMap.cpp
 * @brief RegisterMap implementation.
 */

#include <RegisterMap.h>

RegisterMap::RegisterMap(QObject *parent)
    : QObject(parent)
{
    for (quint8 i = 0x00; i <= 0x03; ++i) m_regs[i] = 0;
    for (quint8 i = 0x10; i <= 0x14; ++i) m_regs[i] = 0;
}

void RegisterMap::write(quint8 reg, quint8 value)
{
    auto it = m_regs.find(reg);
    if (it != m_regs.end() && it->second == value) return;
    m_regs[reg] = value;
    emit registerChanged(reg, value);
}

std::optional<uint8_t> RegisterMap::read(quint8 reg) const
{
    auto it = m_regs.find(reg);
    if (it == m_regs.end()) return std::nullopt;
    return it->second;
}
