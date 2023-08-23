# EGT-367

This repository contains various code that I wrote for EGT 367 - Microprocessors. 
The most interesting file is 'microprocessorsfinal.c', which contains code I alone wrote for the final project.
The final project was to create a stopwatch using an MSP430 microprocessor, 4 buttons, and several 7-segment displays. 
The displays were to be multiplexed and the buttons were to be debounced in software.


Each button had a purpose: one button to step the timer by 1 second, one button to decrement the timer by 1 second, one button to reset, and one button to start.
The timer was to count one minute, and then loop back to 0. You were supposed to use 2 of the displays to show the seconds.
However, I extended the project by instead using 4 of the displays to count seconds, and a decimal up to hundredths of seconds. 
This was not in the project description and nobody else did it, I did it because I thought it would be cool (and also not that hard to extend the existing code to do that).


The button debouncing code is also something I alone wrote. Instead of debouncing by busy waiting the CPU, my code debounces each button by counting the number of cycles since the last
state change. If the number of cycles is greater than some threshold, then the button is stable (no longer bouncing) and the final state is written to the variable.
