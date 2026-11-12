#pragma once
#include <QObject>
#include <QByteArray>

class QSocketNotifier;

class CanBus : public QObject
{
    Q_OBJECT
public:
    explicit CanBus(const QString &interface, QObject *parent = nullptr);
    ~CanBus();

    bool isOpen() const { return m_fd >= 0; }
    bool sendFrame(quint32 canId, const QByteArray &data);

signals:
    void frameReceived(quint32 canId, QByteArray data);
    void errorOccurred(const QString &message);

private slots:
    void onReadyRead();

private:
    int              m_fd       = -1;
    QSocketNotifier *m_notifier = nullptr;
};
