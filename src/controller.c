/**
 * SPDX-FileCopyrightText: 2024 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <zos_errors.h>
#include <zos_time.h>
#include <zos_keyboard.h>
#include "input/controller.h"

/**
 * @brief Define some variables to access the PIO from C code
 */
__sfr __at(0xd0) IO_PIO_DATA_A;
__sfr __at(0xd2) IO_PIO_CTRL_A;

#define CLOCK_ONCE()                   \
    do {                               \
        IO_PIO_DATA_A = 0;             \
        IO_PIO_DATA_A = 1 << IO_CLOCK; \
    } while (0)
#define GET_DATA(port) (IO_PIO_DATA_A & (1 << port))

#define IO_PIO_DISABLE_INT 0x03
#define IO_PIO_BITCTRL     0xcf

#define IO_DATA1 0
#define IO_DATA2 1
#define IO_LATCH 2
#define IO_CLOCK 3

#define PAD_1   0x8000
#define MOUSE_1 0x80000000

uint16_t CONTROLLER1_bits = 0; // nothing
uint16_t CONTROLLER2_bits = 0; // nothing
uint32_t MOUSE_bits       = 0; // nothing

zos_err_t controller_flush(void)
{
    CONTROLLER1_bits = 0;
    CONTROLLER2_bits = 0;
    MOUSE_bits       = 0;

    return ERR_SUCCESS;
}

zos_err_t controller_init(void)
{
    /**
     * Initialize the user port (port A) of the PIO
     * Set it to bit control mode so that each I/O can be controlled independently.
     */
    IO_PIO_CTRL_A = IO_PIO_BITCTRL;
    /**
     * After setting the port as a bit-controlled one, we need to give a bitmask of
     * pins that needs to be output (0) and input (1).
     * Set them all to output except DATA pins.
     */
    IO_PIO_CTRL_A = 3;
    /* Disable the interrupts for this port just in case it was activated */
    IO_PIO_CTRL_A = IO_PIO_DISABLE_INT;
    /**
     * Set the default value of each pin:
     *  - LATCH must be LOW (0)
     *  - CLOCK must be HIGH (1)
     * Set other pins to 0, not very important
     */
    IO_PIO_DATA_A = 1 << IO_CLOCK;

    return ERR_SUCCESS;
}

/**
 * @brief Read the controller state
 */
uint16_t controller_read(void)
{
    return controller_read_port(1);
}

uint16_t controller_read_port(uint8_t port)
{
    /**
     * Generate a pulse on the LATCH pin, CLOCK must remain high during this process
     * Thanks to the preconfigured registers, this takes 24 T-States (2.4 microseconds @ 10MHz)
     */
    IO_PIO_DATA_A = 1 << IO_CLOCK | 1 << IO_LATCH;
    IO_PIO_DATA_A = 1 << IO_CLOCK;
    // Now, the DATA lines contain the first button (B) state.

    CONTROLLER1_bits = GET_DATA(IO_DATA1) == 0 ? PAD_1 : 0;
    CONTROLLER2_bits = GET_DATA(IO_DATA2) == 0 ? PAD_1 : 0;
    // process the remaining 1 buttons (last 4 are unused)
    for (uint8_t i = 0; i < 15; ++i) {
        CONTROLLER1_bits = CONTROLLER1_bits >> 1;
        CONTROLLER2_bits = CONTROLLER2_bits >> 1;
        CLOCK_ONCE();                                             // pulse the clock
        CONTROLLER1_bits |= GET_DATA(IO_DATA1) == 0 ? PAD_1 : 0; // OR the current button
        CONTROLLER2_bits |= GET_DATA(IO_DATA2) == 0 ? PAD_1 : 0; // OR the current button
    }

    switch (port) {
        case 1: return CONTROLLER1_bits;
        case 2: return CONTROLLER2_bits;
    }
    return 0;
}

uint16_t controller_read_mouse(uint8_t port)
{
    (void)port; // unreferenced

    /**
     * Generate a pulse on the LATCH pin, CLOCK must remain high during this process
     * Thanks to the preconfigured registers, this takes 24 T-States (2.4 microseconds @ 10MHz)
     */
    IO_PIO_DATA_A = 1 << IO_CLOCK | 1 << IO_LATCH;
    IO_PIO_DATA_A = 1 << IO_CLOCK;
    // Now, the DATA lines contain the first button (B) state.

    uint8_t i;
    // ignore first byte
    for(i = 0; i < 7; i++) {
        CLOCK_ONCE();
    }

    // second byte (R, L, Speed, ID)
    for (uint8_t i = 0; i < 8; ++i) {
        MOUSE_bits = MOUSE_bits >> 1;
        CLOCK_ONCE();                                             // pulse the clock
        MOUSE_bits |= GET_DATA(IO_DATA2) == 0 ? MOUSE_1 : 0; // OR the current button
    }
    __asm__(
        "nop\n" // 4 cycles
        "nop\n" // 4 cycles
        "nop\n" // 4 cycles
        "nop\n" // 4 cycles
        "nop\n" // 4 cycles
        "nop\n" // 4 cycles
        "nop\n" // 4 cycles
        "nop\n" // 4 cycles
    );

    // third (Y) and fourth (X) byte
    for (uint8_t i = 0; i < 16; ++i) {
        MOUSE_bits = MOUSE_bits >> 1;
        CLOCK_ONCE();                                             // pulse the clock
        MOUSE_bits |= GET_DATA(IO_DATA2) == 0 ? MOUSE_1 : 0; // OR the current button
    }

    return MOUSE_bits;
}

uint8_t controller_is(uint8_t port, ControllerType type)
{
    uint16_t bits = controller_read_port(port);
    printf("controller_is(%d, %d): %04x\n", port, type, bits);

    switch (type) {
        case SNES_PAD: return (bits & 0xF000) == 0;
        case SNES_MOUSE: return (bits & 0xF000) == MOUSE_ID;
        default: return 0;
    }
}
