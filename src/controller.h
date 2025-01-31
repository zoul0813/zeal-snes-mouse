/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#pragma once

#include <stdint.h>
#include <zos_errors.h>
#include <input/button_map.h>

extern uint16_t CONTROLLER1_bits;
extern uint16_t CONTROLLER2_bits;
extern uint32_t MOUSE_bits;

typedef enum {
    SNES_PAD   = 0,
    SNES_MOUSE = 1,
} ControllerType;

zos_err_t controller_init(void);
zos_err_t controller_flush(void);

uint16_t controller_read(void);
uint16_t controller_read_port(uint8_t port);
uint16_t controller_read_mouse(uint8_t port);
uint8_t controller_is(uint8_t port, ControllerType type);

static inline uint8_t controller_pressed_port1(uint16_t button)
{
    return CONTROLLER1_bits & button;
}

static inline uint8_t controller_pressed_port2(uint16_t button)
{
    return CONTROLLER2_bits & button;
}

static inline uint8_t controller_pressed(uint16_t button)
{
    return controller_pressed_port1(button);
}


static inline uint16_t controller_get_port1(void)
{
    return CONTROLLER1_bits;
}

static inline uint16_t controller_get_port2(void)
{
    return CONTROLLER2_bits;
}
