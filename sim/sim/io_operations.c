/**
 * @file io_operations.c
 * @brief Handles I/O operations and interrupt management.
 *
 * This file includes functions to execute `in` and `out` instructions, manage
 * pending interrupts, and update hardware register states.
 *
 * Functions Implemented:
 * - IO_operation: Executes I/O instructions (`in`, `out`, `reti`).
 * - handle_interrupts: Manages interrupts and their effects on the program counter.
 */


#include "simulator_functions.h"


 // Function to handle I/O operations
void IO_operation(instruction_decode* instruction, int data_memory[MEM_SIZE], FILE* hwregtrace, FILE* leds, FILE* display7seg, FILE* monitor, int* PC, int register_array[REG_NUM], int IOR[],
    unsigned char screen[MONITOR_SIZE][MONITOR_SIZE], int disk[NUMBER_OF_SECTORS][SECTOR_SIZE], int* disk_timer)
{
    /*
        Handles I/O operations (in, out, reti) for the processor.
        INPUT:
        - instruction: Decoded instruction structure containing opcode and operands.
        - hwregtrace: Log file for I/O operations.
        - leds, display7seg, monitor: Output log files for devices.
        - PC: Pointer to Program Counter (to update when needed).
        - register_array: CPU register array.
        - IOR: Array representing I/O registers.
        - screen: Monitor pixel array.
    */

    // Names of the I/O registers for logging
    char* IOR_str[] = {
        "irq0enable", "irq1enable", "irq2enable", "irq0status", "irq1status", "irq2status",
        "irqhandler", "irqreturn", "clks", "leds", "display7seg", "timerenable",
        "timercurrent", "timermax", "diskcmd", "disksector", "diskbuffer",
        "diskstatus", "reserved", "reserved", "monitoraddr", "monitordata", "monitorcmd" };

    // Identify the register address (for in/out operations)
    int reg_address = register_array[instruction->rs] + register_array[instruction->rt];

    // Validate register address
    if (reg_address < 0 || reg_address > 22)
    {
        fprintf(stderr, "Error: Invalid register address %d\n", reg_address);
        return;
    }

    char hex_string[9];

    // Switch based on opcode
    switch (instruction->opcode)
    {
    case IN: // 'in': Read from I/O Register
        // Read value from I/O register into rd
        register_array[instruction->rd] = IOR[reg_address];

        // Special case: Clear Monitorcmd register after reading
        if (reg_address == 21)
        {
            IOR[21] = 0;
        }

        
        // Convert DATA: Binar intager to Hexadecimal string
        snprintf(hex_string, 9, "%08X", register_array[instruction->rd]);

        // Log the read operation
        
         log_hw_register(hwregtrace, IOR[8], "READ", IOR_str[reg_address], hex_string);
         break;     
        
        
    case OUT: // 'out': Write to I/O Register
        // Write value from rm to I/O register
        IOR[reg_address] = register_array[instruction->rm];

        // Convert DATA: Binar intager to Hexadecimal string
        snprintf(hex_string, 9, "%08X", register_array[instruction->rm]);

        // Log the write operation
        log_hw_register(hwregtrace, IOR[8], "WRITE", IOR_str[reg_address], hex_string);

        // Handle specific hardware
        if (reg_address == 9) // LEDs
        {
            log_led_change(leds, IOR[8], register_array[instruction->rm]);
        }
        else if (reg_address == 10) // 7-segment display
        {
            log_display_change(display7seg, IOR[8], register_array[instruction->rm]);
        }
        else if (reg_address == 22 && IOR[22] == 1) // Monitor update
        {
            update_monitor(IOR[20], IOR[21], screen);
        }

        else if (reg_address == 14 && IOR[17] == 0) // Disk update
        {
            handle_disk(IOR, disk, data_memory, disk_timer);
        }

        break;

    case RETI: // 'reti': Return from interrupt

        *PC = IOR[7]; // Set PC to the irqreturn value

        // Convert DATA: Binar intager to Hexadecimal string
        snprintf(hex_string, 9, "%08X", IOR[7]);

        if (reg_address != 0) {
        log_hw_register(hwregtrace, IOR[8], "WRITE", IOR_str[reg_address], hex_string);
        }
        break;

    default: // Unknown opcode
        fprintf(stderr, "Unknown I/O opcode: %d\n", instruction->opcode);
        return;
    }

    // Increment PC unless 'reti' was executed
    if (instruction->opcode != RETI)
    {
        (*PC)++;
    }
}


// Handles interrupts based on the IRQ status and enable registers
void handle_interrupts(int* PC, int IOR[IOR_NUM], FILE* hwregtrace_file)
{
    /*
        Handles interrupts based on the IRQ status and enable registers.
        INPUT:
        - PC: Pointer to the Program Counter.
        - IOR: Array representing I/O registers.
        - hwregtrace_file: File pointer for logging hardware register changes.

        FUNCTIONALITY:
        - Checks for active interrupts (irq).
        - Saves the current PC to irqreturn.
        - Jumps to irqhandler if an interrupt is pending.
        - Clears the interrupt flags after servicing.
    */
  
    // Compute the irq signal
    int irq = (IOR[0] & IOR[3]) | (IOR[1] & IOR[4]) | (IOR[2] & IOR[5]);

    // If irq is active and not already in ISR
    if (irq && *PC != IOR[6])
    {
        //char hex_string[9];
        // Save the current PC to irqreturn
        IOR[7] = *PC;

        // Jump to the ISR handler
        *PC = IOR[6];

        // Clear the interrupt flags
        if (IOR[0] & IOR[3])
        {
            IOR[3] = 0; // Clear irq0status
        }
        if (IOR[1] & IOR[4])
        {
            IOR[4] = 0; // Clear irq1status
        }
        if (IOR[2] & IOR[5])
        {
            IOR[5] = 0; // Clear irq2status
        }
    }
}

