// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

#pragma once
#include <QWidget>

namespace EvH::MasterControl::UI {

class LedIndicator : public QWidget
{
    Q_OBJECT
public:
    enum Color { Off, Green, Amber, Red };

    explicit LedIndicator(QWidget *parent = nullptr);
    QSize sizeHint() const override;
    void setColor(Color color);
    Color color() const { return m_color; }

protected:
    void paintEvent(QPaintEvent *) override;

private:
    Color m_color = Off;
};

} // namespace EvH::MasterControl::UI
