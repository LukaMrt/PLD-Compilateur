/*
** EPITECH PROJECT, 2021
** Star
** File description:
** Main function to display the star !
*/

#include <stdio.h>

void display_line(int size)
{
    int count = 0;
    while (count < 2 * size + 1) {
        putchar('*');
        count = count + 1;
    }

    count = 0;
    while (count < 2 * size - 3) {
        putchar(' ');
        count = count + 1;
    }

    count = 0;
    while (count < 2 * size + 1) {
        putchar('*');
        count = count + 1;
    }

    putchar('\n');
}

void draw_space_star(int space)
{
    int count = 0;
    while (count < space) {
        putchar(' ');
        count = count + 1;
    }
    putchar('*');
}

void draw_space_side(int size)
{
    int count = 0;
    while (count < 2 * size) {
        putchar(' ');
        count = count + 1;
    }
}

void display_tiny_line()
{
    int count = 0;
    while (count < 7) {
        if (count == 3)
            putchar(' ');
        else
            putchar('*');
        count = count + 1;
    }

    putchar('\n');
}

void little_star()
{
    draw_space_star(3);
    putchar('\n');

    display_tiny_line();

    draw_space_star(1);
    draw_space_star(3);
    putchar('\n');

    display_tiny_line();

    draw_space_star(3);
    putchar('\n');
}

void cone(int size)
{
    int count = 1;
    while (count < size) {
        draw_space_side(size);
        draw_space_star(size - count - 1);
        //draw_space_star(2 * count - 1);
        putchar('\n');
        count = count + 1;
    }
}

void hopper(int size)
{
    int count = 1;
    while(count < size) {
        draw_space_star(count);
        draw_space_star(6 * size - 1 - 2 * count - 2);
        putchar('\n');
        count = count + 1;
    }

    count = size;
    while(count > 0) {
        draw_space_star(count);
        draw_space_star(6 * size - 1 - 2 * count - 2);
        putchar('\n');
        count = count - 1;
    }
}

void reverse_cone(int size)
{
    int count = size - 1;
    while (count > 0) {
        draw_space_side(size);
        draw_space_star(size - count - 1);
        draw_space_star(2 * count - 1);
        putchar('\n');
        count = count - 1;
    }
}

void star(int size)
{
    if (size == 1) {
        little_star();
    } else if (size != 0) {
        draw_space_side(size);
        draw_space_star(size - 1);
        putchar('\n');
        cone(size);

        display_line(size);

        hopper(size);

        display_line(size);

        reverse_cone(size);

        draw_space_side(size);
        draw_space_star(size - 1);
        putchar('\n');
    }
}

int main() {
    star(4);
}