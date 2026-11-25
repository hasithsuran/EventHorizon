// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

/**
 * @file ControlPanel.h
 * @brief Controller behind the QML control panel: wires CAN traffic, the register map and the UI.
 */

#pragma once
#include <QList>
#include <QObject>
#include <QString>
#include <QtQml/qqmlregistration.h>

namespace EvH::MasterControl {

class CanBus;
class RegisterMap;

/**
 * @class ControlPanel
 * @brief Exposes the master control ECU state to QML and applies UI and CAN requests.
 *
 * Replaces the controller half of the former Qt Widgets MainWindow. It owns the
 * CanBus and the RegisterMap; the RegisterMap is the single source of truth, so
 * every property below is derived from register contents and its change signal
 * is emitted from RegisterMap::registerChanged(), whatever caused the change
 * (a click in the UI or a write over CAN).
 *
 * Protocol (see Protocol.h):
 * - CAN 0x100 [reg, value] writes a register.
 * - CAN 0x101 [reg] requests a register; answered on 0x102 [reg, value].
 * - Toggling a switch writes its register and broadcasts it on 0x200 [reg, value].
 *
 * The object is created by main() and handed to Main.qml as a required
 * property, so it is not creatable from QML.
 */
class ControlPanel : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("ControlPanel is owned by the application and passed to Main.qml")

    Q_PROPERTY(bool powerOn READ powerOn NOTIFY powerOnChanged FINAL)
    Q_PROPERTY(QList<bool> channelStates READ channelStates NOTIFY channelStatesChanged FINAL)
    Q_PROPERTY(QList<int> ledColors READ ledColors NOTIFY ledColorsChanged FINAL)
    Q_PROPERTY(int channelCount READ channelCount CONSTANT FINAL)
    Q_PROPERTY(int ledCount READ ledCount CONSTANT FINAL)
    Q_PROPERTY(QString canInterface READ canInterface CONSTANT FINAL)
    Q_PROPERTY(bool busOnline READ busOnline NOTIFY busOnlineChanged FINAL)

public:
    /// LED colours. Values match the LED register encoding and LedIndicator.Color
    /// (EvH.MasterControl.UI).
    enum LedColor { Off = 0, Green = 1, Amber = 2, Red = 3 };
    Q_ENUM(LedColor)

    /**
     * @brief Creates the register map and opens @p canInterface.
     *
     * A missing or unusable CAN interface is not fatal: the panel keeps
     * working locally and busOnline stays false.
     */
    explicit ControlPanel(const QString &canInterface, QObject *parent = nullptr);
    ~ControlPanel() override;

    bool        powerOn() const;
    QList<bool> channelStates() const;
    QList<int>  ledColors() const;
    int         channelCount() const;
    int         ledCount() const;
    QString     canInterface() const { return m_canInterface; }
    bool        busOnline() const;

    /// Sets the power switch register and broadcasts the new state.
    Q_INVOKABLE void setPower(bool on);

    /// Sets channel @p index (0-based) and broadcasts the new state.
    Q_INVOKABLE void setChannel(int index, bool on);

signals:
    void powerOnChanged();
    void channelStatesChanged();
    void ledColorsChanged();
    void busOnlineChanged();

private slots:
    void onRegisterChanged(quint8 reg, quint8 value);
    void onFrameReceived(quint32 canId, QByteArray data);

private:
    void setSwitchRegister(quint8 reg, bool on);

    CanBus      *m_can;
    RegisterMap *m_regs;
    QString      m_canInterface;
};

} // namespace EvH::MasterControl
