// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

#include <ui/LedIndicator.h>
#include <QPainter>
#include <QRadialGradient>

namespace EvH::MasterControl::UI {

LedIndicator::LedIndicator(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoFillBackground(false);
    setFixedSize(sizeHint());
}

QSize LedIndicator::sizeHint() const
{
    return QSize(26, 26);
}

void LedIndicator::setColor(Color color)
{
    m_color = color;
    update();
}

void LedIndicator::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF r = rect().adjusted(4, 4, -4, -4);
    QPointF center = r.center();
    qreal radius = r.width() / 2.0;

    QColor baseColor, glowColor;
    switch (m_color) {
    case Green:
        baseColor = QColor(20,  180,  70);
        glowColor = QColor(50,  255, 110);
        break;
    case Amber:
        baseColor = QColor(210, 130,   0);
        glowColor = QColor(255, 185,  30);
        break;
    case Red:
        baseColor = QColor(190,  25,  25);
        glowColor = QColor(255,  60,  60);
        break;
    default:
        baseColor = QColor(45, 45, 45);
        glowColor = QColor(65, 65, 65);
        break;
    }

    // Glow rings
    if (m_color != Off) {
        for (int i = 3; i >= 1; --i) {
            QColor gc = glowColor;
            gc.setAlpha(25 * i);
            p.setPen(Qt::NoPen);
            p.setBrush(gc);
            p.drawEllipse(center, radius + i * 3.0, radius + i * 3.0);
        }
    }

    // Housing
    p.setPen(QPen(QColor(15, 15, 15), 1.5));
    p.setBrush(QColor(28, 28, 28));
    p.drawEllipse(r);

    // LED body
    QRadialGradient rg(center - QPointF(radius * 0.3, radius * 0.35), radius * 1.1);
    rg.setColorAt(0.0, m_color == Off ? QColor(60, 60, 60) : glowColor.lighter(120));
    rg.setColorAt(0.5, baseColor);
    rg.setColorAt(1.0, baseColor.darker(200));
    p.setPen(Qt::NoPen);
    p.setBrush(rg);
    p.drawEllipse(r.adjusted(2, 2, -2, -2));

    // Specular highlight
    if (m_color != Off) {
        QRectF hl(center.x() - radius * 0.32, center.y() - radius * 0.60,
                  radius * 0.45, radius * 0.32);
        QRadialGradient hg(hl.center(), hl.width() / 2.0);
        hg.setColorAt(0.0, QColor(255, 255, 255, 190));
        hg.setColorAt(1.0, QColor(255, 255, 255,   0));
        p.setBrush(hg);
        p.drawEllipse(hl);
    }
}

} // namespace EvH::MasterControl::UI
