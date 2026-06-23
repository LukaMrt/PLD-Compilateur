/*
** EPITECH PROJECT, 2021
** Star
** File description:
** Main function to display the star !
*/

#include <stdio.h>

void draw_space_star(int space)
{
    int count = 0;
    //putchar('B');
    while (count < space)
    {
        putchar(' ');
        count = count + 1;
    }
    putchar('*');
}

int main()
{
    int count = 1;
    draw_space_star(6 - count - 1);
}