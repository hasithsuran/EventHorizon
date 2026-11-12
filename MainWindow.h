#pragma once
#include <QWidget>
#include <QList>

class FlipSwitch;
class ImageToggleSwitch;
class LedIndicator;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    LedIndicator *led(int index) const { return m_leds.value(index); }

protected:
    void paintEvent(QPaintEvent *) override;

private:
    FlipSwitch               *m_powerSwitch;
    QList<ImageToggleSwitch *> m_channelSwitches;
    QList<LedIndicator *>      m_leds;
};
