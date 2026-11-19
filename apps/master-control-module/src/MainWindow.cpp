// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

#include <MainWindow.h>
#include <ui/FlipSwitch.h>
#include <ui/ImageToggleSwitch.h>
#include <ui/LedIndicator.h>
#include <CanBus.h>
#include <RegisterMap.h>
#include <QPainter>
#include <QLinearGradient>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

namespace EvH::MasterControl {

namespace {

constexpr quint32 CAN_WRITE_REG  = 0x100;
constexpr quint32 CAN_READ_REQ   = 0x101;
constexpr quint32 CAN_READ_RESP  = 0x102;
constexpr quint32 CAN_SW_BCAST   = 0x200;

QLabel* makeLabel(const QString &text, QWidget *parent)
{
    QLabel *lbl = new QLabel(text, parent);
    lbl->setAlignment(Qt::AlignHCenter);
    QFont f;
    f.setPointSize(8);
    f.setLetterSpacing(QFont::AbsoluteSpacing, 2);
    lbl->setFont(f);
    lbl->setStyleSheet("color: #999999; background: transparent;");
    return lbl;
}

QWidget* makeTransparentPanel(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    w->setAttribute(Qt::WA_TranslucentBackground);
    w->setAttribute(Qt::WA_NoSystemBackground);
    w->setAutoFillBackground(false);
    return w;
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , m_can(new CanBus("vcan0", this))
    , m_regs(new RegisterMap(this))
{
    // Fixed width; height may grow (e.g. when tiled full-height by the compositor)
    setFixedWidth(280);
    resize(280, 680);
    setWindowTitle("xECU-pro");

    connect(m_regs, &RegisterMap::registerChanged, this, &MainWindow::onRegisterChanged);
    connect(m_can,  &CanBus::frameReceived,         this, &MainWindow::onFrameReceived);
    connect(m_can,  &CanBus::errorOccurred, [](const QString &msg) {
        qWarning("CAN: %s", qPrintable(msg));
    });

    QHBoxLayout *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // --- Left panel ---
    QWidget *leftPanel = makeTransparentPanel(this);
    QVBoxLayout *left  = new QVBoxLayout(leftPanel);
    left->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    left->setContentsMargins(16, 24, 8, 24);
    left->setSpacing(4);

    m_powerSwitch = new UI::FlipSwitch(leftPanel);
    left->addWidget(m_powerSwitch, 0, Qt::AlignHCenter);
    left->addWidget(makeLabel("POWER", leftPanel), 0, Qt::AlignHCenter);
    left->addSpacing(8);

    connect(m_powerSwitch, &UI::FlipSwitch::toggled, this, [this](bool on) {
        const quint8 reg = 0x10;
        const quint8 val = on ? 1 : 0;
        m_regs->write(reg, val);
        m_can->sendFrame(CAN_SW_BCAST, QByteArray({char(reg), char(val)}));
    });

    const QStringList names = { "CH 1", "CH 2", "CH 3", "CH 4" };
    for (int i = 0; i < names.size(); ++i) {
        auto *sw = new UI::ImageToggleSwitch(
            ":/switches/switch_off.png",
            ":/switches/switch_on.png",
            leftPanel);
        m_channelSwitches.append(sw);
        left->addWidget(sw, 0, Qt::AlignHCenter);
        left->addWidget(makeLabel(names[i], leftPanel), 0, Qt::AlignHCenter);
        left->addSpacing(2);

        connect(sw, &UI::ImageToggleSwitch::toggled, this, [this, i](bool on) {
            const quint8 reg = 0x11 + i;
            const quint8 val = on ? 1 : 0;
            m_regs->write(reg, val);
            m_can->sendFrame(CAN_SW_BCAST, QByteArray({char(reg), char(val)}));
        });
    }

    left->addStretch();
    QLabel *buildInfo = new QLabel("xECU-pro  v1.0.0\nBuild 2026-11-12", leftPanel);
    buildInfo->setAlignment(Qt::AlignHCenter);
    QFont bf;
    bf.setPointSize(7);
    bf.setLetterSpacing(QFont::AbsoluteSpacing, 1);
    buildInfo->setFont(bf);
    buildInfo->setStyleSheet("color: #555555; background: transparent;");
    left->addWidget(buildInfo, 0, Qt::AlignHCenter);

    // --- Right panel ---
    QWidget *rightPanel = makeTransparentPanel(this);
    QVBoxLayout *right  = new QVBoxLayout(rightPanel);
    right->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    right->setContentsMargins(8, 24, 16, 24);
    right->setSpacing(2);

    const QStringList ledNames = { "L1", "L2", "L3", "L4" };
    for (const QString &lname : ledNames) {
        auto *led = new UI::LedIndicator(rightPanel);
        m_leds.append(led);
        right->addWidget(led, 0, Qt::AlignHCenter);
        right->addWidget(makeLabel(lname, rightPanel), 0, Qt::AlignHCenter);
        right->addSpacing(6);
    }
    right->addStretch();

    root->addWidget(leftPanel, 1);
    root->addWidget(rightPanel, 0);
    setLayout(root);
}

void MainWindow::onRegisterChanged(quint8 reg, quint8 value)
{
    if (reg > 0x03) return;

    static const UI::LedIndicator::Color colorMap[] = {
        UI::LedIndicator::Off, UI::LedIndicator::Green, UI::LedIndicator::Amber, UI::LedIndicator::Red
    };
    m_leds[reg]->setColor(colorMap[qMin(value, quint8(3))]);
}

void MainWindow::onFrameReceived(quint32 canId, QByteArray data)
{
    if (canId == CAN_WRITE_REG && data.size() >= 2) {
        m_regs->write(static_cast<quint8>(data[0]),
                      static_cast<quint8>(data[1]));
    }
    else if (canId == CAN_READ_REQ && data.size() >= 1) {
        quint8 reg   = static_cast<quint8>(data[0]);
        auto result = m_regs->read(reg);
        if (!result.has_value())
            return;
        quint8 value = result.value();
        m_can->sendFrame(CAN_READ_RESP, QByteArray({char(reg), char(value)}));
    }
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QLinearGradient gradient(0, 0, width(), 0);
    gradient.setColorAt(0.0, QColor(0x23, 0x23, 0x23));
    gradient.setColorAt(1.0, QColor(0x3d, 0x3d, 0x3d));
    p.fillRect(rect(), gradient);
}

} // namespace EvH::MasterControl
