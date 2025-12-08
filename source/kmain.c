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
    // Test: 100 - 25 = 75 (which is going to be written as 0x4B)
    //  Write result to eax register so we can verify in logQ.txt
    int result = subtract(100, 25);
    
   
    asm volatile("mov %0, %%eax" : : "r"(result));
}