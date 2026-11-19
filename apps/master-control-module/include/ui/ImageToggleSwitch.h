// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

#pragma once
#include <QWidget>
#include <QPixmap>

namespace EvH::MasterControl::UI {

class ImageToggleSwitch : public QWidget
{
    Q_OBJECT
public:
    explicit ImageToggleSwitch(const QString &offPath,
                               const QString &onPath,
                               QWidget *parent = nullptr);
    QSize sizeHint() const override;
    bool isOn() const { return m_on; }

signals:
    void toggled(bool on);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;

private:
    bool    m_on = false;
    QPixmap m_pixOff;
    QPixmap m_pixOn;
};

} // namespace EvH::MasterControl::UI
