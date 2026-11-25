// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

/**
 * @file Protocol.h
 * @brief CAN identifiers and register addresses of the master control ECU.
 *
 * Single definition of the wire protocol documented in the project README.
 */

#pragma once
#include <QtGlobal> // quint8/quint32; <QtTypes> exists only from Qt 6.5

namespace EvH::MasterControl::Protocol {

/// CAN identifiers used by the register protocol.
namespace CanId {
inline constexpr quint32 WriteRegister   = 0x100; ///< host → ECU: [reg, value]
inline constexpr quint32 ReadRequest     = 0x101; ///< host → ECU: [reg]
inline constexpr quint32 ReadResponse    = 0x102; ///< ECU → host: [reg, value]
inline constexpr quint32 SwitchBroadcast = 0x200; ///< ECU → host: [reg, value]
} // namespace CanId

/// Register addresses.
namespace Register {
inline constexpr quint8 LedBase      = 0x00; ///< LED L1..L4 colour: 0x00–0x03
inline constexpr int    LedCount     = 4;
inline constexpr quint8 Power        = 0x10; ///< Power switch state
inline constexpr quint8 ChannelBase  = 0x11; ///< Channel CH1..CH4 state: 0x11–0x14
inline constexpr int    ChannelCount = 4;
} // namespace Register

} // namespace EvH::MasterControl::Protocol
