// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

/**
 * @file RegisterMap.cpp
 * @brief RegisterMap implementation.
 */

#include <Protocol.h>
#include <RegisterMap.h>

namespace EvH::MasterControl {

RegisterMap::RegisterMap(QObject *parent)
    : QObject(parent)
{
    using namespace Protocol::Register;
    for (int i = 0; i < LedCount; ++i) m_regs[LedBase + i] = 0;
    m_regs[Power] = 0;
    for (int i = 0; i < ChannelCount; ++i) m_regs[ChannelBase + i] = 0;
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

} // namespace EvH::MasterControl
