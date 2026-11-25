// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

#include <CanBus.h>
#include <ControlPanel.h>
#include <Protocol.h>
#include <QLoggingCategory>
#include <RegisterMap.h>

Q_LOGGING_CATEGORY(lcPanel, "evh.mcm.panel")

namespace EvH::MasterControl {

using namespace Protocol;

namespace {

bool isLedRegister(quint8 reg)
{
    return reg >= Register::LedBase && reg < Register::LedBase + Register::LedCount;
}

bool isChannelRegister(quint8 reg)
{
    return reg >= Register::ChannelBase && reg < Register::ChannelBase + Register::ChannelCount;
}

/// Two-byte [register, value] payload used by the broadcast and read-response frames.
QByteArray registerPayload(quint8 reg, quint8 value)
{
    QByteArray payload(2, Qt::Uninitialized);
    payload[0] = static_cast<char>(reg);
    payload[1] = static_cast<char>(value);
    return payload;
}

} // namespace

ControlPanel::ControlPanel(const QString &canInterface, QObject *parent)
    : QObject(parent)
    , m_can(new CanBus(this))
    , m_regs(new RegisterMap(this))
    , m_canInterface(canInterface)
{
    connect(m_regs, &RegisterMap::registerChanged, this, &ControlPanel::onRegisterChanged);
    connect(m_can, &CanBus::frameReceived, this, &ControlPanel::onFrameReceived);
    connect(m_can, &CanBus::openChanged, this, &ControlPanel::busOnlineChanged);
    connect(m_can, &CanBus::errorOccurred, this,
            [](const QString &message) { qCWarning(lcPanel, "CAN: %s", qPrintable(message)); });

    m_can->open(canInterface);
}

ControlPanel::~ControlPanel() = default;

bool ControlPanel::powerOn() const
{
    return m_regs->read(Register::Power).value_or(0) != 0;
}

QList<bool> ControlPanel::channelStates() const
{
    QList<bool> states;
    states.reserve(Register::ChannelCount);
    for (int i = 0; i < Register::ChannelCount; ++i)
        states.append(m_regs->read(Register::ChannelBase + i).value_or(0) != 0);
    return states;
}

QList<int> ControlPanel::ledColors() const
{
    QList<int> colors;
    colors.reserve(Register::LedCount);
    for (int i = 0; i < Register::LedCount; ++i) {
        // Values above Red saturate to Red
        const quint8 value = m_regs->read(Register::LedBase + i).value_or(0);
        colors.append(qMin<int>(value, Red));
    }
    return colors;
}

int ControlPanel::channelCount() const
{
    return Register::ChannelCount;
}

int ControlPanel::ledCount() const
{
    return Register::LedCount;
}

bool ControlPanel::busOnline() const
{
    return m_can->isOpen();
}

void ControlPanel::setPower(bool on)
{
    setSwitchRegister(Register::Power, on);
}

void ControlPanel::setChannel(int index, bool on)
{
    if (index < 0 || index >= Register::ChannelCount) {
        qCWarning(lcPanel, "setChannel: index %d out of range [0, %d)", index,
                  Register::ChannelCount);
        return;
    }
    setSwitchRegister(static_cast<quint8>(Register::ChannelBase + index), on);
}

void ControlPanel::setSwitchRegister(quint8 reg, bool on)
{
    const quint8 value = on ? 1 : 0;
    m_regs->write(reg, value);
    m_can->sendFrame(CanId::SwitchBroadcast, registerPayload(reg, value));
}

void ControlPanel::onRegisterChanged(quint8 reg, quint8 /*value*/)
{
    if (isLedRegister(reg))
        emit ledColorsChanged();
    else if (reg == Register::Power)
        emit powerOnChanged();
    else if (isChannelRegister(reg))
        emit channelStatesChanged();
}

void ControlPanel::onFrameReceived(quint32 canId, QByteArray data)
{
    if (canId == CanId::WriteRegister && data.size() >= 2) {
        m_regs->write(static_cast<quint8>(data[0]), static_cast<quint8>(data[1]));
    } else if (canId == CanId::ReadRequest && data.size() >= 1) {
        const quint8 reg    = static_cast<quint8>(data[0]);
        const auto   result = m_regs->read(reg);
        if (!result.has_value()) {
            qCDebug(lcPanel, "Read request for unknown register 0x%02X ignored", reg);
            return;
        }
        m_can->sendFrame(CanId::ReadResponse, registerPayload(reg, *result));
    }
}

} // namespace EvH::MasterControl
