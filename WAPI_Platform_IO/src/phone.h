#ifndef PHONE_H
#define PHONE_H

#include "variables.h"
#include "display.h"
#include "sim_shenanigans.h"

bool calling = false;

//! Public
void phone_main()
{
    static String phone_number = "004915561423885";
    while (1)
    {
        char key = keypad_get_key();
        // compose number from key presses
        if (key != '\0')
        {
            // if calling only accept hang up button
            if (calling)
            {
                if (key == '#') // '#' to call
                {
                    hangup_call();
                    calling = false;
                    display_simple_text("Call ended.\nEnter number:");
                }
            }
            else
            {
                if (key == '#') // '#' to call
                {
                    display_simple_text("Calling " + phone_number + "...");
                    calling = true;
                    call_number(phone_number);
                    phone_number = "";
                }
                else if (key == '*') // '*' to clear
                {
                    if (phone_number.length() == 0 && !calling)
                        return; // exit app if no number being composed
                    phone_number = "";
                    display_simple_text("Enter number:");
                }
                else
                {
                    phone_number += key;
                    display_simple_text("Number: " + phone_number);
                }
            }
        }
    }
}

#endif
