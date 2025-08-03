#ifndef RW_H
#define RW_H

void readsec(void);
void writesec(void);
void wipesec(void);
int str_eq(const char* a, const char* b);
void str_copy(char* dest, const char* src, int max_len);

void extract_value_from_sector(const char* var_name, char* output, int max_len);
#endif

