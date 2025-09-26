/**
 * @file simulation.c
 * @brief Implements the main simulation loop for the SIMP processor.
 *
 * This file contains the Fetch-Decode-Execute loop, which drives the simulation.
 * It coordinates instruction execution, hardware interactions, and memory updates.
 *
 * Functions Implemented:
 * - simulate: Manages the Fetch-Decode-Execute loop.
 * - fetch_instruction: Fetches instructions from memory based on the program counter.
 * - decode_instruction: Decodes the fetched instructions into components.
 * - execute_instruction: Executes a single decoded instruction.
 */


#include "simulator_functions.h"

 // Main simulation function: Fetch - Decode - Execute loop
void simulate(FILE* output_files[], int IOR[23], int registers[REG_NUM], int data_memory[MEM_SIZE], unsigned char screen[MONITOR_SIZE][MONITOR_SIZE], int disk[NUMBER_OF_SECTORS][SECTOR_SIZE], char instruction_memory[MEM_SIZE][CMD_BYTES + 1], unsigned int interupt2_events[MEM_SIZE])
{
    unsigned int pc = 0; // Program Counter initialization
    unsigned int cycle = 0; // clock cycle initialization 
    unsigned int halted = 0; 
    unsigned disk_timer = 0; // Initialize disk timer
    int* intup2_pointer = interupt2_events;
 
    while (!halted)
    {
        IOR[8] = cycle; // Update clock counter

        IOR[5] = 0; //reset irq2 after one clock cycle.

        // Fetch instruction
        const char* instruction = fetch_instruction(instruction_memory, &pc);


        if (!instruction)
        {
            fprintf(stderr, "Error: Failed to fetch instruction at PC=%u\n", pc);
            break;
        }


        // Decode instruction
        instruction_decode decoded_instruction;
        decode_instruction(instruction, &decoded_instruction, registers);


        // Log instruction trace before execution - but woth the instruction to be performed
        log_trace(output_files[2], pc, instruction, registers);


        // Check halt condition: if disk timer is not done eventhough there are no other instructoins -> prosseccor continues
        if (decoded_instruction.opcode == HALT && disk_timer == 0)
        {
            cycle++;
            halted = 1;
            break;
        }
        // Check halt condition: if disk timer is done and there are no other instructoins -> prosseccor stops
        if (decoded_instruction.opcode == HALT && disk_timer != 0) {
            manage_disk_status(IOR, &disk_timer);
            cycle++;
            continue;
        }

        // Execute instruction
        execute_instruction(&decoded_instruction, registers, &pc, data_memory, IOR, screen,
            output_files[3], output_files[5], output_files[6], output_files[8], &disk_timer, disk);

        //Handling interups:

        //IRQ2 status
        if (cycle == *intup2_pointer) {
            IOR[5] = 1;
            intup2_pointer = intup2_pointer + 1;
        }

        //IRQ1 - Manage disk timer
        manage_disk_status(IOR, &disk_timer);

        //IRQ0 - Handle timer interrupt
        handle_timer_status(IOR);

        //Handle pending interrupts
        handle_interrupts(&pc, IOR, output_files[3]);//to check if 3

        cycle++; // increasing clock by 1
    }

    // Final output writing
    write_data_memory(output_files[0], data_memory);
    write_registers(output_files[1], registers);
    write_disk_contents(output_files[7], disk);
    write_monitor_pixels(output_files[8], screen);
    write_monitor_yuv(output_files[9], screen);
    write_cycle_count(output_files[4], cycle);

}

// Fetches an instruction from memory based on the program counter (PC)
const char* fetch_instruction(const char instruction_memory[MEM_SIZE][CMD_BYTES + 1], int* PC)
{
    /*
        Fetches the instruction from instruction memory based on the program counter (PC).
        Returns a pointer to the instruction at the current PC.
    */
    
    return instruction_memory[*PC]; // Fetch the instruction at the PC location
}

// Decodes the fetched instruction and stores the fields in the instruction_decode struct
void decode_instruction(const char* instruction, instruction_decode* decoded_instruction, int registers[REG_NUM])
{
    /*
        Decodes the instruction and stores the values in the decoded_instruction structure.
        INPUT:
            instruction (hexadecimal string representing the instruction)
            decoded_instruction (pointer to the structure to store decoded components)
        OUTPUT:
            Populates the decoded_instruction structure with the decoded components.
            Prints error messages if the instruction format is invalid.
    */

    
    if (instruction == NULL || strlen(instruction) != 12)
    {
        printf("Error: Invalid instruction format. Instruction must be a 12-character string.\n");
        return;
    }

    // Decode each field and validate conversion
    decoded_instruction->opcode = str_hex_2_bin(instruction, 0, 2);    
    if (decoded_instruction->opcode < 0)
    {
        printf("Error: Invalid opcode in instruction.\n");
        return;
    }
    
    

    decoded_instruction->rd = str_hex_2_bin(instruction, 2, 1);
    if (decoded_instruction->rd < 0)
    {
        printf("Error: Invalid rd register in instruction.\n");
        return;
    }
   

    decoded_instruction->rs = str_hex_2_bin(instruction, 3, 1);
    if (decoded_instruction->rs < 0)
    {
        printf("Error: Invalid rs register in instruction.\n");
        return;
    }
    

    decoded_instruction->rt = str_hex_2_bin(instruction, 4, 1);
    if (decoded_instruction->rt < 0)
    {
        printf("Error: Invalid rt register in instruction.\n");
        return;
    }
   

    decoded_instruction->rm = str_hex_2_bin(instruction, 5, 1);

    if (decoded_instruction->rm < 0)
    {
        printf("Error: Invalid rm register in instruction.\n");
        return;
    }
    


    int sign_imm1;
    decoded_instruction->imm1 = str_hex_2_bin(instruction, 6, 3);
    sign_imm1 = (decoded_instruction->imm1 & 0x800) != 0;

    if (sign_imm1 == 1) {
        sign_imm1 = -1;
        decoded_instruction->imm1 = -1 * (((~decoded_instruction->imm1) + 1) & 0xFFF);
    }
     registers[1] = decoded_instruction->imm1;
  
     
    

    int sign_imm2;
    decoded_instruction->imm2 = str_hex_2_bin(instruction, 9, 3);
    sign_imm2 = (decoded_instruction->imm2 & 0x800) != 0;


    if (sign_imm2 == 1 ) {
        decoded_instruction->imm2 = -1*(((~decoded_instruction->imm2) + 1)& 0xFFF);
    }
    registers[2] = decoded_instruction->imm2;
   

    
}

//Executes a decoded instruction.
void execute_instruction(instruction_decode* decoded_instruction, int register_array[REG_NUM], int* PC, int data_memory[MEM_SIZE],
    int IOR[23], unsigned char screen[MONITOR_SIZE][MONITOR_SIZE], FILE* hwregtrace_file, FILE* leds_file, FILE* display7seg_file, FILE* monitor_file, int* disk_timer, int disk[NUMBER_OF_SECTORS][SECTOR_SIZE])
 {
   
    /*
        Executes the instruction and updates the program counter.
        Based on the opcode, it performs arithmetic, comparison, memory, or I/O operations.
    */

    // Validate opcode range
    if (decoded_instruction->opcode < 0 || decoded_instruction->opcode > 21)
    {
        fprintf(stderr, "Error: Unsupported opcode: %d\n", decoded_instruction->opcode);
        return;
    }

    switch (decoded_instruction->opcode)
    {
        // Arithmetic operations (add, sub, mac, and, or, xor, sll, sra, srl)
    case 0: // add
    case 1: // sub
    case 2: // mac
    case 3: // and
    case 4: // or
    case 5: // xor
    case 6: // sll (shift left logical)
    case 7: // sra (shift right arithmetic)
    case 8: // srl (shift right logical)
        arithmetic_operation(decoded_instruction, register_array, PC);
        break;

        // Comparison and branch operations (beq, bne, blt, bgt, ble, bge, jal)
    case 9:  // beq (branch if equal)
    case 10: // bne (branch if not equal)
    case 11: // blt (branch if less than)
    case 12: // bgt (branch if greater than)
    case 13: // ble (branch if less or equal)
    case 14: // bge (branch if greater or equal)
    case 15: // jal (jump and link)
        comparison_operation(decoded_instruction, register_array, PC);
        break;

        // Memory operations (lw, sw)
    case 16: // lw (load word)
    case 17: // sw (store word)
        load_store_operation(decoded_instruction, data_memory, register_array, PC);
        break;

        // I/O operations (in, out, reti)
    case 18: // reti (return from interrupt)
    case 19: // in (read from I/O register)
    case 20: // out (write to I/O register)
        IO_operation(decoded_instruction, data_memory, hwregtrace_file, leds_file, display7seg_file, monitor_file, PC, register_array, IOR, screen, disk, disk_timer);
        break;

        // Halt operation
    case 21: // halt
        // No specific action here; handled in the simulation loop
        break;

        // Default case (should not be reached due to validation above)
    default:
        fprintf(stderr, "Unexpected opcode: %d\n", decoded_instruction->opcode);
        break;
}
}


