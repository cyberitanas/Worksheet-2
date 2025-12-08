ENTRY(loader)                   /* Entry point is 'loader' */

SECTIONS {
    . = 0x00100000;             /* Load kernel at 1MB */
    
    .text ALIGN (0x1000) : {    /* Code section, 4KB aligned */
        *(.text)                 /* All .text sections from all files */
    }
    
    .rodata ALIGN (0x1000) : {  /* Read-only data, 4KB aligned */
        *(.rodata*)
    }
    
    .data ALIGN (0x1000) : {    /* Initialized data, 4KB aligned */
        *(.data)
    }
    
    .bss ALIGN (0x1000) : {     /* Uninitialized data, 4KB aligned */
        *(COMMON)
        *(.bss)
    }
}