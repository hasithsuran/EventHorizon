// Copyright (c) 2026 Don Hasitha Suran Mahawithana
// SPDX-License-Identifier: MIT

/**
 * @file RegisterMap.h
 * @brief master control ECU register map — stores and notifies on register changes.
 */

#pragma once
#include <QObject>
#include <cstdint>
#include <map>
#include <optional>

/**
 * @class RegisterMap
 * @brief Models the master controller ECU's internal register space.
 *
 * Maintains a map of 8-bit address → 8-bit value pairs representing
 * the ECU register set. Emits registerChanged() only when a value
 * actually changes, avoiding spurious UI updates.
 *
 * Registers are pre-populated at construction:
 * - 0x00–0x03: control registers
 * - 0x10–0x14: status registers
 *
 * @note This class is a pure data model.
 */
class RegisterMap : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructs the register map and zeroes all known registers.
     * @param parent  Optional Qt parent for memory management.
     */
    explicit RegisterMap(QObject *parent = nullptr);

    /**
     * @brief Writes a value to a register.
     *
     * If the register already holds @p value, the write is a no-op
     * and registerChanged() is not emitted.
     *
     * @param reg    Register address.
     * @param value  Value to write.
     */
    void   write(quint8 reg, quint8 value);

    /**
     * @brief Reads the current value of a register.
     *
     * Returns std::nullopt if @p reg is not a known register address.
     * This distinguishes a missing register from one that holds 0x00.
     *
     * @param reg  Register address to read.
     * @return     The register value, or std::nullopt if not found.
     */
    std::optional<uint8_t> read(quint8 reg) const;

signals:
    void registerChanged(quint8 reg, quint8 value);

private:
    std::map<uint8_t, uint8_t> m_regs;
};
