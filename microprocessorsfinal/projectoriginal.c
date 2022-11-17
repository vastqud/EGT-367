#include <msp430.h>
#include <stdio.h>

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
int last_times[4] = {
    0, 0, 0, 0
};
int flagged_pins[4] = {
    0, 0, 0, 0
};
int previous_unbounced[4] = {
    1, 1, 1, 1
};

int num = 11;
int timer_counter = 0;
int milliseconds = 0;
int debounce_time = 10; //milliseconds
int enabled = 1;

void update_display(tens, ones) {
    P8OUT = display_codes[tens]; //display tens place
    P7OUT |= 0x04;
    P7OUT &= ~0x04;

    P8OUT = 0xFD;
    P7OUT |= 0x08;
    P7OUT &= ~0x08;

    _delay_cycles(2000);

    P8OUT = display_codes[ones]; //display ones place
    P7OUT |= 0x04;
    P7OUT &= ~0x04;

    P8OUT = 0xFE;
    P7OUT |= 0x08;
    P7OUT &= ~0x08;

    _delay_cycles(2000);
}

void update_inputs() {
    int pin = 0;
    for (pin = 0; pin < 3; pin++) {
        int pin_mask = input_masks[pin];
        int button_state = pin_mask & P2IN;
        int previous_state = previous_states[pin];
        int final_state = 1;

        if (previous_state != button_state) { //state changed
            last_times[pin] = milliseconds;
            previous_states[pin] = button_state;
        }

        if ((milliseconds - last_times[pin]) >= debounce_time) { //debounced
            if ((flagged_pins[pin] == 0) && (button_state == 0)) {
                final_state = 0;
            } else if ((flagged_pins[pin] == 1) && (button_state == 1)) {
                final_state = 1;
                flagged_pins[pin] = 0;
            }
        }

        button_states[pin] = final_state;
    }
}

int main() {
    P8DIR |= 0xFF; //p8 as output
    P2DIR &= ~0xFF; //p2 as input
    P7DIR |= 0x0C;
    WDTCTL = WDTPW + WDTHOLD;

    TA0CCTL0 = CCIE;

    while (1) {
        if (num < 0) {
            num = 0;
        } else if (num > 59) {
            num = 0;
        }

        milliseconds = milliseconds + 1;
        int tens = num / 10;
        int ones = num % 10;

        update_display(tens, ones);
        update_inputs();

        if (button_states[0] == 0) {
            num++;
            flagged_pins[0] = 1;
        }
        if (button_states[1] == 0) {
            num--;
            flagged_pins[1] = 1;
        }
        if (button_states[2] == 0) {
            num = 0;
            flagged_pins[2] = 1;
        }
    }

    return 0;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TimerA(void) {
    //milliseconds = milliseconds + 50;
}
