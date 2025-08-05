#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdint.h>

void int_to_str(int value, char* buffer, int buffer_size);
void int_to_strmath(int num, char *str);
int str_to_int(const char *str);
#endif // DATATYPES_H
