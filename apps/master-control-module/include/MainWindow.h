// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

#pragma once
#include <QWidget>
#include <QList>

namespace EvH::MasterControl::UI {
class FlipSwitch;
class ImageToggleSwitch;
class LedIndicator;
} // namespace EvH::MasterControl::UI

namespace EvH::MasterControl {

class CanBus;
class RegisterMap;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    UI::LedIndicator *led(int index) const { return m_leds.value(index); }

protected:
    void paintEvent(QPaintEvent *) override;

private slots:
    void onRegisterChanged(quint8 reg, quint8 value);
    void onFrameReceived(quint32 canId, QByteArray data);

private:
    UI::FlipSwitch                 *m_powerSwitch;
    QList<UI::ImageToggleSwitch *> m_channelSwitches;
    QList<UI::LedIndicator *>      m_leds;
    CanBus                         *m_can;
    RegisterMap                    *m_regs;
};

} // namespace EvH::MasterControl
