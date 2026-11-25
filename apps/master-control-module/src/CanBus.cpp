// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

#include <CanBus.h>
#include <QLoggingCategory>
#include <QSocketNotifier>
#include <cerrno>
#include <cstring>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>

Q_LOGGING_CATEGORY(lcCan, "evh.mcm.can")

namespace EvH::MasterControl {

CanBus::CanBus(QObject *parent)
    : QObject(parent)
{
}

CanBus::~CanBus()
{
    // Do not emit from the destructor; just release the descriptor.
    delete m_notifier;
    if (m_fd >= 0) ::close(m_fd);
}

void CanBus::fail(const QString &what)
{
    const QString message = QStringLiteral("%1 (%2): %3")
                                .arg(what, m_interface, QString::fromLocal8Bit(strerror(errno)));
    close();
    emit errorOccurred(message);
}

bool CanBus::open(const QString &interface)
{
    close();
    m_interface = interface;

    m_fd = ::socket(AF_CAN, SOCK_RAW | SOCK_NONBLOCK | SOCK_CLOEXEC, CAN_RAW);
    if (m_fd < 0) {
        fail(QStringLiteral("Failed to open CAN socket"));
        return false;
    }

    const unsigned int ifindex = if_nametoindex(interface.toLocal8Bit().constData());
    if (ifindex == 0) {
        fail(QStringLiteral("CAN interface not found"));
        return false;
    }

    sockaddr_can addr{};
    addr.can_family  = AF_CAN;
    addr.can_ifindex = static_cast<int>(ifindex);
    if (::bind(m_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        fail(QStringLiteral("Failed to bind CAN socket"));
        return false;
    }

    m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, &CanBus::onReadyRead);

    qCInfo(lcCan, "Listening on %s", qPrintable(interface));
    emit openChanged(true);
    return true;
}

void CanBus::close()
{
    const bool wasOpen = isOpen();

    delete m_notifier;
    m_notifier = nullptr;
    if (m_fd >= 0) {
        ::close(m_fd);
        m_fd = -1;
    }

    if (wasOpen) emit openChanged(false);
}

bool CanBus::sendFrame(quint32 canId, const QByteArray &data)
{
    if (m_fd < 0) return false;

    if (data.size() > CAN_MAX_DLEN) {
        emit errorOccurred(QStringLiteral("CAN payload too long (%1 bytes) for ID 0x%2")
                               .arg(data.size())
                               .arg(canId, 0, 16));
        return false;
    }

    can_frame frame{};
    frame.can_id  = canId > CAN_SFF_MASK ? ((canId & CAN_EFF_MASK) | CAN_EFF_FLAG) : canId;
    frame.can_dlc = static_cast<__u8>(data.size());
    std::memcpy(frame.data, data.constData(), frame.can_dlc);

    ssize_t written;
    do {
        written = ::write(m_fd, &frame, sizeof(frame));
    } while (written < 0 && errno == EINTR);

    if (written != static_cast<ssize_t>(sizeof(frame))) {
        // ENOBUFS: TX queue full. Report, but keep the socket open.
        emit errorOccurred(QStringLiteral("CAN send failed for ID 0x%1: %2")
                               .arg(canId, 0, 16)
                               .arg(QString::fromLocal8Bit(strerror(errno))));
        return false;
    }
    return true;
}

void CanBus::onReadyRead()
{
    // Non-blocking socket: drain everything that is queued.
    while (m_fd >= 0) {
        can_frame     frame{};
        const ssize_t n = ::read(m_fd, &frame, sizeof(frame));

        if (n < 0) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) return;
            // Persistent error (e.g. ENETDOWN): close instead of spinning on the notifier.
            fail(QStringLiteral("CAN read error"));
            return;
        }
        if (n != static_cast<ssize_t>(sizeof(frame))) {
            qCWarning(lcCan, "Discarding incomplete CAN frame (%zd bytes)", n);
            continue;
        }
        if (frame.can_id & (CAN_ERR_FLAG | CAN_RTR_FLAG)) continue;

        const quint32 id  = (frame.can_id & CAN_EFF_FLAG) ? (frame.can_id & CAN_EFF_MASK)
                                                          : (frame.can_id & CAN_SFF_MASK);
        const quint8  dlc = qMin<quint8>(frame.can_dlc, CAN_MAX_DLEN);
        const QByteArray payload(reinterpret_cast<const char *>(frame.data), dlc);

        qCDebug(lcCan, "RX  ID: 0x%03X  DLC: %u  Data: %s", id, dlc,
                payload.toHex(' ').toUpper().constData());

        emit frameReceived(id, payload);
    }
}

} // namespace EvH::MasterControl
