// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

#include <ImageToggleSwitch.h>
#include <QPainter>
#include <QMouseEvent>

ImageToggleSwitch::ImageToggleSwitch(const QString &offPath,
                                     const QString &onPath,
                                     QWidget *parent)
    : QWidget(parent)
    , m_pixOff(offPath)
    , m_pixOn(onPath)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoFillBackground(false);
    setCursor(Qt::PointingHandCursor);
    setFixedSize(sizeHint());

    if (m_pixOff.isNull()) qWarning("switch_off.png failed to load: %s", qPrintable(offPath));
    if (m_pixOn.isNull())  qWarning("switch_on.png failed to load: %s",  qPrintable(onPath));
}

QSize ImageToggleSwitch::sizeHint() const
{
    return QSize(90, 45);
}

void ImageToggleSwitch::mousePressEvent(QMouseEvent *)
{
    m_on = !m_on;
    update();
    emit toggled(m_on);
}

void ImageToggleSwitch::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    const QPixmap &pix = m_on ? m_pixOn : m_pixOff;
    if (pix.isNull()) return;

    QPixmap scaled = pix.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    p.drawPixmap((width()  - scaled.width())  / 2,
                 (height() - scaled.height()) / 2,
                 scaled);
}
