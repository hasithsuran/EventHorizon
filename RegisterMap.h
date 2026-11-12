#pragma once
#include <QObject>
#include <QMap>

class RegisterMap : public QObject
{
    Q_OBJECT
public:
    explicit RegisterMap(QObject *parent = nullptr);

    void   write(quint8 reg, quint8 value);
    quint8 read(quint8 reg) const;

signals:
    void registerChanged(quint8 reg, quint8 value);

private:
    QMap<quint8, quint8> m_regs;
};
