#include <stdio.h>
#include <zos_sys.h>
#include <zgdk.h>

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

    uint16_t input1 = 0, input2 = 0;
    uint16_t prev_input1 = 0, prev_input2 = 0;
    while(1) {
        input1 = controller_read_port1();
        input2 = controller_read_port2();
        if(!input1 && !input2) continue;
        if((input1 == prev_input1) && (input2 == prev_input2)) continue;

        if(START1) break;

        if(input1) {
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
            printf("\n");
        }

        if(input2) {
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
            printf("\n");
        }

        prev_input1 = input1;
        prev_input2 = input2;
    }

    return 0;
}