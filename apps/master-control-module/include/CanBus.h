// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

/**
 * @file CanBus.h
 * @brief Thin SocketCAN (raw, classic CAN) wrapper integrated with the Qt event loop.
 */

#pragma once
#include <QByteArray>
#include <QObject>
#include <QString>

class QSocketNotifier;

namespace EvH::MasterControl {

/**
 * @class CanBus
 * @brief Sends and receives classic CAN frames on a SocketCAN interface.
 *
 * Opening is a separate step from construction, so callers can connect to
 * errorOccurred() before anything can fail. The socket is non-blocking;
 * received frames are drained from the event loop via QSocketNotifier.
 *
 * Every received frame is logged on the @c evh.mcm.can category at debug
 * level. Silence it with @c QT_LOGGING_RULES="evh.mcm.can.debug=false".
 */
class CanBus : public QObject
{
    Q_OBJECT
public:
    explicit CanBus(QObject *parent = nullptr);
    ~CanBus() override;

    CanBus(const CanBus &)            = delete;
    CanBus &operator=(const CanBus &) = delete;

    /**
     * @brief Opens and binds a raw CAN socket on @p interface (e.g. "vcan0").
     *
     * Closes any previously open socket first.
     * @return true on success; on failure errorOccurred() is emitted.
     */
    bool open(const QString &interface);

    /// Closes the socket. Safe to call when already closed.
    void close();

    bool    isOpen() const { return m_fd >= 0; }
    QString interfaceName() const { return m_interface; }

    /**
     * @brief Sends one data frame.
     * @param canId  11-bit or 29-bit identifier; IDs above 0x7FF are sent as extended frames.
     * @param data   Payload, at most 8 bytes.
     * @return true if the whole frame was queued to the socket.
     */
    bool sendFrame(quint32 canId, const QByteArray &data);

signals:
    /// A data frame arrived. @p canId has the EFF/RTR/ERR flag bits stripped.
    void frameReceived(quint32 canId, QByteArray data);
    void errorOccurred(const QString &message);
    void openChanged(bool open);

private slots:
    void onReadyRead();

private:
    void fail(const QString &what);

    int              m_fd       = -1;
    QSocketNotifier *m_notifier = nullptr;
    QString          m_interface;
};

} // namespace EvH::MasterControl
