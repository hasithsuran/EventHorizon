#pragma once
#include <QWidget>
#include <QPropertyAnimation>

class FlipSwitch : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal thumbPos READ thumbPos WRITE setThumbPos)

public:
    explicit FlipSwitch(QWidget *parent = nullptr);
    QSize sizeHint() const override;
    bool isOn() const { return m_on; }

signals:
    void toggled(bool on);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;

private:
    qreal thumbPos() const { return m_thumbPos; }
    void  setThumbPos(qreal v) { m_thumbPos = v; update(); }

    bool    m_on       = false;
    qreal   m_thumbPos = 0.0;   // 0 = OFF, 1 = ON
    QPropertyAnimation *m_anim;
};
