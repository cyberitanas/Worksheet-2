#ifndef INPUT_H
#define INPUT_H

void input_reset(void);
void input_putc(char c);
void input_backspace(void);

char getc(void);
void readline(char *buf, int max_len);

#endif
