int sum_of_three(int a, int b, int c)
{
    return a + b + c;
}

int subtract(int a, int b)
{
    return a - b;
}

int command(int operation, int a, int b, int c)
{
    if (operation == 0) {
        return sum_of_three(a, b, c);
    } else if (operation == 1) {
        return subtract(a, b);
    }
    return 0;
}

void kmain()
{
    // Test: 5 + 10 + 15 = 30 (0x1E in hex) Write result to eax register so we can see it in logQ.txt
    int result = sum_of_three(5, 10, 15);
    
    // If EAX = 30 (0000000x1E), the test passed
    asm volatile("mov %0, %%eax" : : "r"(result));
}