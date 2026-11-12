#pragma once
#include <QWidget>
#include <QPixmap>

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
