// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

#include <ui/FlipSwitch.h>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QPropertyAnimation>

namespace EvH::MasterControl::UI {

FlipSwitch::FlipSwitch(QWidget *parent)
    : QWidget(parent)
{
    setCursor(Qt::PointingHandCursor);
    setFixedSize(sizeHint());

    m_anim = new QPropertyAnimation(this, "thumbPos", this);
    m_anim->setDuration(150);
    m_anim->setEasingCurve(QEasingCurve::InOutQuad);
}

QSize FlipSwitch::sizeHint() const
{
    return QSize(80, 34);
}

void FlipSwitch::mousePressEvent(QMouseEvent *)
{
    m_on = !m_on;
    m_anim->setStartValue(m_thumbPos);
    m_anim->setEndValue(m_on ? 1.0 : 0.0);
    m_anim->start();
    emit toggled(m_on);
}

void FlipSwitch::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const qreal r     = height() / 2.0;
    const qreal pad   = 3.0;
    const qreal thumbD = height() - pad * 2;

    // Track
    QRectF track(0, 0, width(), height());
    QPainterPath trackPath;
    trackPath.addRoundedRect(track, r, r);

    QColor offColor(55, 55, 55);
    QColor onColor(45, 150, 90);
    QColor trackColor(
        offColor.red()   + m_thumbPos * (onColor.red()   - offColor.red()),
        offColor.green() + m_thumbPos * (onColor.green() - offColor.green()),
        offColor.blue()  + m_thumbPos * (onColor.blue()  - offColor.blue())
    );

    p.fillPath(trackPath, trackColor);
    p.setPen(QPen(QColor(20, 20, 20), 1));
    p.drawPath(trackPath);

    // Labels
    QFont f;
    f.setPointSize(8);
    f.setBold(true);
    f.setLetterSpacing(QFont::AbsoluteSpacing, 1);
    p.setFont(f);

    p.setPen(QColor(255, 255, 255, 180));
    p.drawText(QRectF(0, 0, width() / 2.0, height()), Qt::AlignCenter, "OFF");
    p.drawText(QRectF(width() / 2.0, 0, width() / 2.0, height()), Qt::AlignCenter, "ON");

    // Thumb
    qreal travel  = width() - pad * 2 - thumbD;
    qreal thumbX  = pad + m_thumbPos * travel;

    QRectF thumb(thumbX, pad, thumbD, thumbD);

    QRadialGradient rg(thumb.center(), thumbD / 2.0);
    rg.setColorAt(0.0, QColor(240, 240, 240));
    rg.setColorAt(1.0, QColor(170, 170, 170));

    p.setPen(QPen(QColor(90, 90, 90), 1));
    p.setBrush(rg);
    p.drawEllipse(thumb);
}

} // namespace EvH::MasterControl::UI
