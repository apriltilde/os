#ifndef STRING_H
#define STRING_H

int str_eq(const char* a, const char* b);// Helper: copy string with max length
void str_copy(char* dest, const char* src, int max_len);
int str_len(const char* s);

#endif // STRING_H
