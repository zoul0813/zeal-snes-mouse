/* SPDX-FileCopyrightText: 2023 Zeal 8-bit Computer <contact@zeal8bit.com>
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <zos_errors.h>
#include <zos_vfs.h>
#include <zos_sys.h>
#include <zos_video.h>
#include <zvb_gfx.h>

#include "controller.h"

#define DEBUG_MODE 0

static const uint8_t hand_point_zts[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04,
    0x04, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x04, 0x04, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x04, 0x04, 0x04, 0x02,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03,
    0x04, 0x04, 0x04, 0x03, 0x04, 0x04, 0x04, 0x04, 0x02, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
    0x02, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x03, 0x01, 0x00,
    0x00, 0x01, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x01, 0x00, 0x00, 0x01, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x03, 0x01, 0x00, 0x00, 0x01, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
    0x03, 0x03, 0x03, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

static const uint8_t hand_point_ztp[] = {
    0x00, 0x00, 0x00, 0x00, 0xcb, 0x5a, 0x96, 0xb5, 0xff, 0xff
};

gfx_context vctx;
__sfr __at(0xF2) mmu_page2;

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480

int paint(void)
{
    /* Switch to graphics mode, 320x240px, 8-bit mode */
    gfx_enable_screen(0);
    gfx_error err = gfx_initialize(ZVB_CTRL_VID_MODE_GFX_640_8BIT, &vctx);
    if (err) return err;

    /* Load the palette */
    err = gfx_palette_load(&vctx, hand_point_ztp, sizeof(hand_point_ztp), 0);
    if (err) return err;

    mmu_page2 = VID_MEM_TILESET_ADDR >> 14;
    memcpy((void*) 0x8200, hand_point_zts, 256);

    /* Load the cursor tile */
    gfx_tileset_add_color_tile(&vctx, 0, 0);
    gfx_tileset_add_color_tile(&vctx, 1, 3);

#define COLOR_COUNT 8
    for (uint8_t i = 0; i < COLOR_COUNT; i++) {
        gfx_tileset_add_color_tile(&vctx, 3 + i, 0x40 + i);
    }

    /* Load both tilemaps with tile0 */
    static uint8_t trans[SCREEN_WIDTH / 16] = { 0 };
    static uint8_t color[SCREEN_WIDTH / 16] = { 1 };

    for (uint8_t i = 0; i < SCREEN_WIDTH / 16; i++) {
        color[i] = 1;
    }

    for (uint8_t i = 0; i < SCREEN_HEIGHT / 16; i++) {
        gfx_tilemap_load(&vctx, color,  sizeof(color), 0, 0, i);
        gfx_tilemap_load(&vctx, trans, sizeof(trans), 1, 0, i);
    }

    /* Load the sprite */
    gfx_sprite spr = {
        .y = 16,
        .x = 16,
        .tile = 2,
        .flags = 0
    };
    gfx_sprite_render(&vctx, 0, &spr);
    /* Preview color */
    spr.x = 32;
    spr.y = 32;
    spr.tile = 3;
    gfx_sprite_render(&vctx, 1, &spr);

    gfx_enable_screen(1);

    int16_t x = 0;
    int16_t y = 0;
    uint8_t cur_col = 0;

    uint16_t input1 = 0, input2 = 0, prev_input1 = 0, prev_input2 = 0;
    int8_t mx, my;
    while (1) {
        input1 = controller_read_port(SNES_PORT1);
        if(BUTTON1_X) break;
        controller_read_mouse(SNES_PORT2);
        input2 = controller_get(SNES_PORT2);
        mx = controller_get_mousex();
        my = controller_get_mousey();

        // left/right button clicks
        if(MOUSE2_L) {
            gfx_tilemap_place(&vctx, cur_col + 3, 1, x / 16, y / 16);
        } else if (MOUSE2_R != 0) {
            gfx_tilemap_place(&vctx, 1, 1, x / 16, y / 16);
        }

        if (prev_input1 != input1) {
            if((cur_col > 0) && BUTTON1_L) cur_col--;
            if((cur_col < COLOR_COUNT-1) && BUTTON1_R) cur_col++;
            gfx_sprite_set_tile(&vctx, 1, cur_col + 3);
        }
        prev_input1 = input1;
        prev_input2 = input2;

        uint8_t diff = ((uint8_t)mx & 0x7F) >> 2;
        if (mx < 0) {
            x -= diff;
        } else if(mx > 0) {
            x += diff;
        }

        diff = (my & 0x7F) >> 2;
        if (my < 0) {
            y -=  diff;
        } else if(my > 0) {
            y +=  diff;
        }


        // screen boundaries
        if (x <= 0) {
            x = 0;
        } else if (x > SCREEN_WIDTH - 16) {
            x = SCREEN_WIDTH - 16;
        }

        if (y <= 0) {
            y = 0;
        } else if (y > SCREEN_HEIGHT - 16) {
            y = SCREEN_HEIGHT - 16;
        }

        gfx_wait_vblank(&vctx);
        gfx_sprite_set_x(&vctx, 0, x + 16);
        gfx_sprite_set_y(&vctx, 0, y + 16);
        /* Preview color sprite */
        gfx_sprite_set_x(&vctx, 1, x + 32);
        gfx_sprite_set_y(&vctx, 1, y + 32);
        gfx_wait_vblank(&vctx);
    }

    // reset screen
    ioctl(DEV_STDOUT, CMD_RESET_SCREEN, NULL);

    return 0;
}
