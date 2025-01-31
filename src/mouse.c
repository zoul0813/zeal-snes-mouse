#include <stdio.h>
#include <zos_sys.h>
// #include <zgdk.h>
#include "controller.h"

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

void deinit(void) {
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

int main(void) {
    zos_err_t err;

    printf("snes mouse test\n");

    err = controller_init();
    handle_error(err, "failed to init controller 1", 1);
    err = controller_flush();
    handle_error(err, "failed to flush controller 1", 1);

    uint8_t mousePort = 0;
    if(controller_is(1, SNES_MOUSE)) {
        printf("SNES Mouse Detected: %d\n", 1);
        mousePort = 1;
    }
    if(controller_is(2, SNES_MOUSE)) {
        printf("SNES Mouse Detected: %d\n", 2);
        mousePort = 2;
    }

    uint16_t input1 = 0, input2 = 0;
    uint32_t mouse = 0, prev_mouse = 0;
    uint16_t prev_input1 = 0, prev_input2 = 0;
    while(1) {
        if(mousePort != 1) input1 = controller_read_port(1);
        if(mousePort != 2) input2 = controller_read_port(2);
        if(mousePort) mouse = controller_read_mouse(mousePort);

        // no input
        // if(!input1 && !input2 && !mouse) continue;

        // input hasn't changed
        // if(
        //     (input1 == prev_input1)
        //     && (input2 == prev_input2)
        //     && (mouse == prev_mouse)
        // ) continue;

        if(input1 != prev_input1) {
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

        if(input2 != prev_input2) {
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

        if(mouse != prev_mouse) {
            uint8_t right = ((mouse >> 8) & 0x1);
            uint8_t left = (mouse >> 9) & 0x1;
            uint8_t speed = (mouse >> 10) & 0xC;
            uint8_t y = (mouse >> 16) & 0xFF;
            uint8_t x = (mouse >> 24) & 0xFF;
            printf("mouse: %02x %02x %02x %02x L: %d R: %d S: %d X: %d Y: %d\n",
                (mouse >> 24) & 0xFF,
                (mouse >> 16) & 0xFF,
                (mouse >> 8) & 0xFF,
                (mouse >> 0) & 0xFF,
                left,
                right,
                speed,
                x,
                y
            );
        }

        if(START1) break;

        prev_input1 = input1;
        prev_input2 = input2;
        prev_mouse = mouse;
    }

    return 0;
}