/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#pragma once

#include <stdint.h>
#include <zos_errors.h>
// #include <input/button_map.h>

/*                         */ // Bit
#define BUTTON_B       0x0001 // 0
#define BUTTON_Y       0x0002 // 1
#define BUTTON_SELECT  0x0004 // 2
#define BUTTON_START   0x0008 // 3
#define BUTTON_UP      0x0010 // 4
#define BUTTON_DOWN    0x0020 // 5
#define BUTTON_LEFT    0x0040 // 6
#define BUTTON_RIGHT   0x0080 // 7
#define BUTTON_A       0x0100 // 8
#define BUTTON_X       0x0200 // 9
#define BUTTON_L       0x0400 // 10
#define BUTTON_R       0x0800 // 11
#define BUTTON_UNUSED1 0x1000 // 12
#define BUTTON_UNUSED2 0x2000 // 13
#define BUTTON_UNUSED3 0x4000 // 14
#define BUTTON_UNUSED4 0x8000 // 15

/*                       */ // Bits
#define MOUSE_IGNORE 0x00FF // 0-7
#define MOUSE_RIGHT  0x0100 // 8
#define MOUSE_LEFT   0x0200 // 9
#define MOUSE_SPEED  0x0C00 // 10-11
#define MOUSE_ID     0x8000 // 12-15
// #define MOUSE_Y      0x00FF // 16-23
// #define MOUSE_X      0xFF00 // 24-31

#define LEFT1     (input1 & BUTTON_LEFT)
#define RIGHT1    (input1 & BUTTON_RIGHT)
#define UP1       (input1 & BUTTON_UP)
#define DOWN1     (input1 & BUTTON_DOWN)
#define BUTTON1_B (input1 & BUTTON_B)
#define BUTTON1_A (input1 & BUTTON_A)
#define BUTTON1_X (input1 & BUTTON_X)
#define BUTTON1_Y (input1 & BUTTON_Y)
#define BUTTON1_L (input1 & BUTTON_L)
#define BUTTON1_R (input1 & BUTTON_R)
#define START1    (input1 & BUTTON_START)
#define SELECT1   (input1 & BUTTON_SELECT)


#define LEFT2     (input2 & BUTTON_LEFT)
#define RIGHT2    (input2 & BUTTON_RIGHT)
#define UP2       (input2 & BUTTON_UP)
#define DOWN2     (input2 & BUTTON_DOWN)
#define BUTTON2_B (input2 & BUTTON_B)
#define BUTTON2_A (input2 & BUTTON_A)
#define BUTTON2_X (input2 & BUTTON_X)
#define BUTTON2_Y (input2 & BUTTON_Y)
#define BUTTON2_L (input2 & BUTTON_L)
#define BUTTON2_R (input2 & BUTTON_R)
#define START2    (input2 & BUTTON_START)
#define SELECT2   (input2 & BUTTON_SELECT)

#define MOUSE1_R   (input1 & MOUSE_RIGHT)
#define MOUSE1_L   (input1 & MOUSE_LEFT)
#define MOUSE1_SPD ((input1 & MOUSE_SPEED) >> 2)

#define MOUSE2_R   (input2 & MOUSE_RIGHT)
#define MOUSE2_L   (input2 & MOUSE_LEFT)
#define MOUSE2_SPD (((input2 & MOUSE_SPEED) >> 10) & 0xFF)

#define SNES_PORT1 0
#define SNES_PORT2 1

extern uint16_t PORT1_bits;
extern uint16_t PORT2_bits;
extern int8_t MOUSE_y;
extern int8_t MOUSE_x;

typedef enum {
    SNES_PAD   = 0,
    SNES_MOUSE = 1,
} ControllerType;

typedef enum {
    MOUSE_LOW    = 0,
    MOUSE_MEDIUM = 1,
    MOUSE_HIGH   = 2,
} MouseSensitivity;

zos_err_t controller_init(void);
zos_err_t controller_flush(void);

uint16_t controller_read(void);
uint16_t controller_read_port(uint8_t port);

uint8_t controller_read_mouse(uint8_t port);
uint8_t controller_set_mouse_sensitivity(uint8_t port, MouseSensitivity s);
int8_t controller_get_mousey(void);
int8_t controller_get_mousex(void);

uint8_t controller_is(uint8_t port, ControllerType type);
uint16_t controller_get(uint8_t port);

static inline uint8_t controller_pressed_port1(uint16_t button)
{
    return PORT1_bits & button;
}

static inline uint8_t controller_pressed_port2(uint16_t button)
{
    return PORT2_bits & button;
}

static inline uint8_t controller_pressed(uint16_t button)
{
    return controller_pressed_port1(button);
}


static inline uint16_t controller_get_port1(void)
{
    return PORT1_bits;
}

static inline uint16_t controller_get_port2(void)
{
    return PORT2_bits;
}
