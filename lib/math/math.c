// math.c

#include "../core/print.h"
#include "../core/io.h"
#include "../keyboard/keyboard.h"
#include "math.h"

#define BUFFER_SIZE 128

extern char input_buffer[BUFFER_SIZE];
extern int extract_arguments(const char *command, char args[][BUFFER_SIZE], int max_args, int max_len);

void int_to_str(int num, char *str) {
    char temp[32];
    int i = 0, is_negative = 0;

    if (num == 0) {
        str[0] = '0'; str[1] = '\0';
        return;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num > 0) {
        temp[i++] = (num % 10) + '0';
        num /= 10;
    }

    if (is_negative) temp[i++] = '-';

    // Reverse the string
    int j = 0;
    while (i > 0) str[j++] = temp[--i];
    str[j] = '\0';
}



int parse_int(const char *str) {
    int result = 0;
    int sign = 1;

    if (*str == '-') {
        sign = -1;
        str++;
    }

    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return sign * result;
}

void add_command(void) {
    char args[10][BUFFER_SIZE];
    int count = extract_arguments("add", args, 10, BUFFER_SIZE);

    if (count < 2) {
        print(WHITE, "\nUsage: add <num1> <num2> [...]\n");
        return;
    }

    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += parse_int(args[i]);
    }

    char result[32];
    int_to_str(sum, result);
    print(WHITE, "\nResult: ");
    print(WHITE, result);
    print(WHITE, "\n");
}

void sub_command(void) {
    char args[10][BUFFER_SIZE];
    int count = extract_arguments("sub", args, 10, BUFFER_SIZE);

    if (count < 2) {
        print(WHITE, "\nUsage: sub <num1> <num2> [...]\n");
        return;
    }

    int result = parse_int(args[0]);
    for (int i = 1; i < count; i++) {
        result -= parse_int(args[i]);
    }

    char out[32];
    int_to_str(result, out);
    print(WHITE, "\nResult: ");
    print(WHITE, out);
    print(WHITE, "\n");
}

void mul_command(void) {
    char args[10][BUFFER_SIZE];
    int count = extract_arguments("mul", args, 10, BUFFER_SIZE);

    if (count < 2) {
        print(WHITE, "\nUsage: mul <num1> <num2> [...]\n");
        return;
    }

    int product = 1;
    for (int i = 0; i < count; i++) {
        product *= parse_int(args[i]);
    }

    char result[32];
    int_to_str(product, result);
    print(WHITE, "\nResult: ");
    print(WHITE, result);
    print(WHITE, "\n");
}

void div_command(void) {
    char args[2][BUFFER_SIZE];
    int count = extract_arguments("div", args, 2, BUFFER_SIZE);

    if (count < 2) {
        print(WHITE, "\nUsage: div <numerator> <denominator>\n");
        return;
    }

    int numerator = parse_int(args[0]);
    int denominator = parse_int(args[1]);

    if (denominator == 0) {
        print(WHITE, "\nError: Division by zero\n");
        return;
    }

    int result = numerator / denominator;

    char out[32];
    int_to_str(result, out);
    print(WHITE, "\nResult: ");
    print(WHITE, out);
    print(WHITE, "\n");
}

