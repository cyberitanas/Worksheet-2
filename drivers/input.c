#include "input.h"

#define INPUT_BUF_SIZE 256

static volatile char buffer[INPUT_BUF_SIZE];
static volatile int head = 0;
static volatile int tail = 0;

static int next_index(int idx)
{
    ++idx;
    if (idx >= INPUT_BUF_SIZE) {
        idx = 0;
    }
    return idx;
}

void input_reset(void)
{
    head = 0;
    tail = 0;
}

void input_putc(char c)
{
    int next = next_index(head);
    if (next == tail) {
        tail = next_index(tail);
    }

    buffer[head] = c;
    head = next;
}

void input_backspace(void)
{
    if (head == tail) {
        return;
    }

    if (head == 0) {
        head = INPUT_BUF_SIZE - 1;
    } else {
        --head;
    }
}

char getc(void)
{
    if (head == tail) {
        return 0;
    }

    char c = buffer[tail];
    tail = next_index(tail);
    return c;
}

void readline(char *buf, int max_len)
{
    int len = 0;

    while (1) {
        char c = getc();

        if (!c) {
            continue;
        }

        if (c == '\n') {
            break;
        }

        if (c == '\b') {
            if (len > 0) {
                --len;
            }
            continue;
        }

        if (len < max_len - 1) {
            buf[len++] = c;
        }
    }

    buf[len] = '\0';
}
