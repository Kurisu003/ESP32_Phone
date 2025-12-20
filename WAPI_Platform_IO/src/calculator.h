#ifndef CALCULATOR_H
#define CALCULATOR_H
#include "display.h"
#include "input.h"
#include "variables.h"
#include <iostream>
#include <string>
#include "tinyexpr.h"

int selected_x = 0;
int selected_y = 0;
#define max_x 4
#define max_y 4
String expr = "";
int err;

// flag that gets set to true
// if calcultor should quit
bool exit_calc = false;

//! Private
double eval_expression()
{
    const char *expr_cstr = expr.c_str();
    double result = te_interp(expr_cstr, &err);
    if (err == 0)
    {
        return (result);
    }
    else
    {
        display_simple_text("Error parsing expression");
    }
    return 0.0;
}

//! Private
void handle_calculator_selection_input()
{
    char key = keypad_get_key();
    if (key == '\0')
        return;

    if (key == '6')
        selected_x = (selected_x + 1) % max_x;

    if (key == '4')
        selected_x = (selected_x - 1) % max_x;

    if (key == '2')
        selected_y = (selected_y - 1) % max_y;

    if (key == '8')
        selected_y = (selected_y + 1) % max_y;

    if (selected_y < 0)
        selected_y = max_y - 1;

    if (selected_x < 0)
        selected_x = max_x - 1;

    if (key == '5')
    {
        char c = *calculator_keys[selected_y][selected_x];
        if (c == '=')
        {
            double result = eval_expression();

            draw_calculator_result(String(result));
        }
        else
        {
            expr += calculator_keys[selected_y][selected_x];
        }
    }

    if (key == '*')
    {
        if (expr == "")
            exit_calc = true;

        expr = "";
    }
}

//! Public
void calculator_main()
{
    int previous_x = -1;
    int previous_y = -1;
    String prev_expr = "0";

    while (true)
    {
        delay(20);
        handle_calculator_selection_input();

        if (exit_calc)
        {
            Serial.println("exiting");
            // reset to false because if re-entering
            // then it would still be set to true
            // from before and immediatly exit
            exit_calc = false;
            return;
        }

        if (previous_x == selected_x && previous_y == selected_y && expr == prev_expr)
            continue;

        draw_calculator_interface();
        draw_selected_calc_square(selected_x, selected_y);
        draw_calculator_expression(expr);

        previous_x = selected_x;
        previous_y = selected_y;
        prev_expr = expr;
    }
}

#endif