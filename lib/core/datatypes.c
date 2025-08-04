static void int_to_str(int value, char* buffer, int buffer_size) {
    if (buffer_size <= 0) return;

    int i = buffer_size - 1;
    buffer[i] = '\0';
    i--;

    int is_negative = 0;
    unsigned int uvalue;

    if (value < 0) {
        is_negative = 1;
        uvalue = (unsigned int)(-value);
    } else {
        uvalue = (unsigned int)value;
    }

    if (uvalue == 0) {
        if (i >= 0) {
            buffer[i] = '0';
            i--;
        }
    } else {
        while (uvalue > 0 && i >= 0) {
            buffer[i] = '0' + (uvalue % 10);
            uvalue /= 10;
            i--;
        }
    }

    if (is_negative && i >= 0) {
        buffer[i] = '-';
        i--;
    }

    // Shift string to start of buffer
    int start = i + 1;
    int j = 0;
    while (buffer[start] != '\0' && j < buffer_size) {
        buffer[j] = buffer[start];
        j++; start++;
    }
    buffer[j] = '\0';
}


void int_to_strmath(int num, char *str) {
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