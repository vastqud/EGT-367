#include <msp430.h>
#include <stdio.h>
#include <math.h>

int display_codes[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67
};
int input_masks[4] = {
    0x01, 0x02, 0x04, 0x08
};
int button_states[4] = {
    1, 1, 1, 1
};
int previous_states[4] = {
    1, 1, 1, 1
};
long int last_times[4] = {
    0, 0, 0, 0
};
int held_pins[4] = {
    0, 0, 0, 0
};

int num = 0;
double decimal = 0;

int timer_counter = 0;
int enabled = 0;

long int debounce_cycles = 0;
int debounce_time = 5; //debounce cycles

void update_display(tens, ones, tenths, hundredths) {
    P8OUT = display_codes[tens]; //display tens place
    P7OUT |= 0x04;
    P7OUT &= ~0x04;

    P8OUT = 0xF7;
    P7OUT |= 0x08;
    P7OUT &= ~0x08;

    _delay_cycles(2000);

    int ones_place = display_codes[ones];
    ones_place |= 0x80; //add decimal point

    P8OUT = ones_place; //display ones place with decimal point
    P7OUT |= 0x04;
    P7OUT &= ~0x04;

    P8OUT = 0xFB;
    P7OUT |= 0x08;
    P7OUT &= ~0x08;

    _delay_cycles(2000);

    P8OUT = display_codes[tenths]; //display tenths place
    P7OUT |= 0x04;
    P7OUT &= ~0x04;

    P8OUT = 0xFD;
    P7OUT |= 0x08;
    P7OUT &= ~0x08;

    _delay_cycles(2000);

    P8OUT = display_codes[hundredths]; //display hundredths place
    P7OUT |= 0x04;
    P7OUT &= ~0x04;

    P8OUT = 0xFE;
    P7OUT |= 0x08;
    P7OUT &= ~0x08;

    _delay_cycles(2000);
}

void update_inputs() {
    int pin = 0;
    for (pin = 0; pin < 4; pin++) {
        int pin_mask = input_masks[pin];
        int button_state = pin_mask & P2IN;

        if (button_state > 0) { //off
            button_state = 1; //0x02 & 0xFD (P2IN) for example is 0000 0010 (2 in decimal). so if it's greater than 0, means the button is not pressed
        }

        if (previous_states[pin] != button_state) { //state changed (button was pressed or is currently bouncing)
            last_times[pin] = debounce_cycles; //reset debounce timer
        }

        if ((debounce_cycles - last_times[pin]) > debounce_time) { //debounced (state is stable)
            if (button_state != button_states[pin]) { //if the new debounced state is different than the current stable state
                button_states[pin] = button_state; //set new current stable state

                if (button_state == 1) { //if pin is being turned off, unflag it as being held
                    held_pins[pin] = 0;
                }
            }
        }

        previous_states[pin] = button_state;
    }
}

int main() {
    P8DIR |= 0xFF; //p8 as output
    P2DIR &= ~0xFF; //p2 as input
    P7DIR |= 0x0C; //display latch setup

    WDTCTL = WDTPW + WDTHOLD;
    TA0CCTL0 = CCIE;
    //TODO: set up TimerA register
    while (1) {
        if (num < 0) {
            num = 59;
        } else if (num > 59) {
            num = 0;
            decimal = 0;
        }
        if (decimal >= 1) {
            decimal = 0;
        }

        debounce_cycles++;
        int tens = num / 10;
        int ones = num % 10;
        int decimal_whole = decimal * 100;
        int tenths = decimal_whole / 10;
        int hundredths = decimal_whole % 10;

        update_display(tens, ones, tenths, hundredths);
        update_inputs();

        if ((button_states[0] == 0) && (held_pins[0] == 0)) { //if button is pressed and it's not being held
            num++; //increment
            held_pins[0] = 1; //flag button as being held down
        }
        if ((button_states[1] == 0) && (held_pins[1] == 0)) {
            num--; //decrement
            held_pins[1] = 1;
        }
        if ((button_states[2] == 0) && (held_pins[2] == 0)) {
            num = 0;
            decimal = 0; //reset
            held_pins[2] = 1;
        }
        if ((button_states[3] == 0) && (held_pins[3] == 0)) {
            enabled = !enabled; //toggle stopwatch
            held_pins[3] = 1;
        }
    }

    return 0;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TimerA(void) {
    if (enabled == 1) {
        timer_counter++
        decimal = decimal + 0.05; //increase decimal precision by 50ms
    }

    if (timer_counter >= 20) {
        num++ //increment whole number seconds
        decimal = 0; //reset decimal
        timer_counter = 0
    }
}
