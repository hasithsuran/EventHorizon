#include "CanBus.h"
#include <QSocketNotifier>
#include <cstring>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

CanBus::CanBus(const QString &interface, QObject *parent)
    : QObject(parent)
{
    m_fd = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (m_fd < 0) {
        emit errorOccurred("Failed to open CAN socket");
        return;
    }

    unsigned int ifindex = if_nametoindex(interface.toLocal8Bit().constData());
    if (ifindex == 0) {
        emit errorOccurred(QString("Interface not found: %1").arg(interface));
        close(m_fd);
        m_fd = -1;
        return;
    }

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifindex;

    if (bind(m_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        emit errorOccurred("Failed to bind CAN socket");
        close(m_fd);
        m_fd = -1;
        return;
    }

    m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, &CanBus::onReadyRead);
}

CanBus::~CanBus()
{
    if (m_fd >= 0)
        close(m_fd);
}

bool CanBus::sendFrame(quint32 canId, const QByteArray &data)
{
    if (m_fd < 0) return false;

    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));
    frame.can_id  = canId;
    frame.can_dlc = static_cast<quint8>(qMin(data.size(), 8));
    memcpy(frame.data, data.constData(), frame.can_dlc);

    return write(m_fd, &frame, sizeof(frame)) == sizeof(frame);
}

void CanBus::onReadyRead()
{
    struct can_frame frame;
    int nbytes = read(m_fd, &frame, sizeof(frame));
    if (nbytes < 0) {
        emit errorOccurred("CAN read error");
        return;
    }

    // Log to console
    QByteArray hex;
    for (int i = 0; i < frame.can_dlc; ++i)
        hex += QString(" %1").arg(frame.data[i], 2, 16, QChar('0')).toUpper().toLocal8Bit();
    fprintf(stdout, "CAN RX  ID: 0x%03X  DLC: %d  Data:%s\n",
            frame.can_id & CAN_EFF_MASK, frame.can_dlc, hex.constData());
    fflush(stdout);

    emit frameReceived(frame.can_id & CAN_EFF_MASK,
                       QByteArray(reinterpret_cast<char *>(frame.data), frame.can_dlc));
}
