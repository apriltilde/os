#ifndef RW_H
#define RW_H

void readsec(void);
void writesec(void);
void wipesec(void);
void execsec(void);
void var_extract(const char* var_name, char* output, int max_len);
#endif

