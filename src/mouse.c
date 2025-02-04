#include <stdio.h>
#include <zos_sys.h>
#include <zos_vfs.h>
#include <zos_video.h>
#include <zvb_gfx.h>
// #include <zgdk.h>
#include "controller.h"

int paint(void);

void deinit(void)
{
    // do nothing
}

void handle_error(zos_err_t err, const char* message, uint8_t fatal)
{
    if (err != ERR_SUCCESS) {
        if (fatal)
            deinit();
        printf("\nError[%d] (%02x) %s", err, err, message);
        if (fatal)
            exit(err);
    }
}

int main(void)
{
    zos_err_t err;

    // // third (Y) byte
    // int8_t MOUSE_y = 0;
    // uint8_t a = 0;
    // printf("MOUSE_y: ");
    // for (uint8_t i = 0; i < 8; ++i) {
    //     MOUSE_y = MOUSE_y << 1;
    //     a = i % 2 == 0 ? 1 : 0;
    //     MOUSE_y |= a;
    //     printf("%d", a);
    // }
    // if(MOUSE_y & 0x80) MOUSE_y = -(MOUSE_y & 0x7F);
    // printf("  %02x %d\n", MOUSE_y & 0xFF, MOUSE_y);
    // exit(1);


    printf("snes mouse test\n");

    err = controller_init();
    handle_error(err, "failed to init controller 1", 1);

    err = controller_flush();
    handle_error(err, "failed to flush controller 1", 1);


    uint8_t mousePort = 0xFF;

    if (controller_is(SNES_PORT1, SNES_MOUSE)) {
        printf("SNES Mouse Detected: %d\n", SNES_PORT1);
        mousePort = SNES_PORT1;
    }

    if (controller_is(SNES_PORT2, SNES_MOUSE)) {
        printf("SNES Mouse Detected: %d\n", SNES_PORT2);
        mousePort = SNES_PORT2;
    }

    if(mousePort != 0xFF) {
        controller_set_mouse_sensitivity(mousePort, MOUSE_LOW);
    }

    uint16_t input1 = 0, input2 = 0;
    uint16_t prev_input1 = 0, prev_input2 = 0;
    uint8_t mouse = 0, prev_mouse = 0;
    uint8_t right = 0, left  = 0, sensitivity = 0;
    int8_t y = 0, x = 0;

    while (1) {
        // wait on vblank to simulate actual read speed for "distance"
        gfx_wait_vblank(NULL);
        gfx_wait_end_vblank(NULL);

        controller_read();
        if (mousePort != 0xFF) {
            msleep(1);
            mouse = controller_read_mouse(mousePort);
        }
        input1 = controller_get(SNES_PORT1);
        input2 = controller_get(SNES_PORT2);

        if ((mousePort != SNES_PORT1) && (input1 != prev_input1)) {
            printf("P1: %04x ", input1);
            if(LEFT1) printf("Left ");
            if(RIGHT1) printf("Right ");
            if(UP1) printf("Up ");
            if(DOWN1) printf("Down ");
            if(BUTTON1_A) printf("A ");
            if(BUTTON1_B) printf("B ");
            if(BUTTON1_X) printf("X ");
            if(BUTTON1_Y) printf("Y ");
            if(BUTTON1_L) printf("L ");
            if(BUTTON1_R) printf("R ");
            if(SELECT1) printf("Select ");
            if(START1) printf("Start ");
            printf("\n");
        }

        if ((mousePort != SNES_PORT2) && (input2 != prev_input2)) {
            printf("P2: %04x ", input2);
            if(LEFT2) printf("Left ");
            if(RIGHT2) printf("Right ");
            if(UP2) printf("Up ");
            if(DOWN2) printf("Down ");
            if(BUTTON2_A) printf("A ");
            if(BUTTON2_B) printf("B ");
            if(BUTTON2_X) printf("X ");
            if(BUTTON2_Y) printf("Y ");
            if(BUTTON2_L) printf("L ");
            if(BUTTON2_R) printf("R ");
            if(SELECT2) printf("Select ");
            if(START2) printf("Start ");
            printf("\n");
        }

        if ((mousePort != 0xFF) && ((mouse != prev_mouse) || (input2 != prev_input2))) {
            right = (MOUSE2_R != 0);
            left  = (MOUSE2_L != 0);
            sensitivity = MOUSE2_SPD;
            y = controller_get_mousey();
            x = controller_get_mousex();
            printf("mouse: %02x %02x %02x %04x ",
                mouse & 0xFF, // %02x
                y & 0xFF,     // %02x
                x & 0xFF,     // %02x
                input2        // %04x
            );
            printf("L: %03d R: %03d S: %03d Y: %04d X: %04d \n",
                left,           // %03d
                right,          // %03d
                sensitivity,          // %03d
                y,
                x
            );
        }

        if ((mousePort != SNES_PORT1)) {
            if(START1) goto exit_program;
            if(BUTTON1_Y) {
                paint();
                ioctl(DEV_STDOUT, CMD_RESET_SCREEN, NULL);
                controller_flush();
            }
        }
        if ((mousePort == SNES_PORT1)) {
            if(START2) goto exit_program;
        }

        prev_input1 = input1;
        prev_input2 = input2;
        prev_mouse  = mouse;
    }

exit_program:
    return 0;
}
