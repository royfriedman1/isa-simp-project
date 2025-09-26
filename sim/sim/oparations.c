/**
 * @file execution.c
 * @brief Contains functions for executing decoded instructions.
 *
 * This file includes implementations for executing arithmetic, comparison,
 * memory, and I/O operations as part of the instruction execution process.
 *
 * Functions:
 * - arithmetic_operation: Handles arithmetic operations (e.g., ADD, SUB).
 * - comparison_operation: Handles comparison and branch operations.
 * - load_store_operation: Executes memory-related operations (LW, SW).
 */
#include "simulator_functions.h"


 // Performs arithmetic operations like add, sub, etc., based on the opcode
void arithmetic_operation(instruction_decode* decoded_instruction, int register_array[REG_NUM], int* PC)
{
    (*PC)++; // Update PC
    int rs = register_array[decoded_instruction->rs];
    int rt = register_array[decoded_instruction->rt];
    int rm = register_array[decoded_instruction->rm];
    int result;

    switch (decoded_instruction->opcode)
    {
    case ADD: // Add
        result = rs + rt + rm;
        break;
    case SUB: // Subtract
        result = rs - rt - rm;

       
        break;
    case MAC: // Multiply and Accumulate (MAC)
        result = rs * rt + rm;
        break;
    case AND: // AND
        result = rs & rt & rm;
        break;
    case OR: // OR
        result = rs | rt | rm;
        break;
    case XOR: // XOR
        result = rs ^ rt ^ rm;
        break;
    case SLL: // Shift Left Logical (SLL)
        result = rs << rt;

        
        break;
    case SRA:              // Shift Right Arithmetic (SRA)
        result = rs >> rt; // Assuming arithmetic shift
        break;
    case SRL: // Shift Right Logical (SRL)
        result = (unsigned int)rs >> rt;
        break;
    default:
        printf("Error: Unsupported opcode %d.\n", decoded_instruction->opcode);
        return;
    }

    // Store the result in the destination register

    register_array[decoded_instruction->rd] = result;
    
}

// Function to handle comparison and branch instructions
void comparison_operation(instruction_decode* decoded_instruction, int register_array[REG_NUM], int* PC)
{
    /*
        Handles comparison and branch operations based on the decoded instruction.
        INPUT:
        - decoded_instruction: Pointer to the decoded instruction structure.
        - register_array: Array of CPU registers.
        - PC: Pointer to the Program Counter.
    */

    // Validate the opcode
    if (decoded_instruction->opcode < BEQ || decoded_instruction->opcode > JAL)
    {
        printf("Error: Invalid opcode %d\n", decoded_instruction->opcode);
        return;
    }

    // Validate that the register indices are within range
    if (decoded_instruction->rs < 0 || decoded_instruction->rs >= REG_NUM ||
        decoded_instruction->rt < 0 || decoded_instruction->rt >= REG_NUM ||
        decoded_instruction->rm < 0 || decoded_instruction->rm >= REG_NUM)
    {
        printf("Error: Invalid register indices in instruction\n");
        return;
    }

    // Perform the operation based on the opcode
    switch (decoded_instruction->opcode)
    {
    case BEQ: // Branch if equal
        *PC = (register_array[decoded_instruction->rs] == register_array[decoded_instruction->rt])
            ? (register_array[decoded_instruction->rm] & MASK_12_BIT)
            : (*PC + 1);
        break;

    case BNE: // Branch if not equal
        *PC = (register_array[decoded_instruction->rs] != register_array[decoded_instruction->rt])
            ? (register_array[decoded_instruction->rm] & MASK_12_BIT)
            : (*PC + 1);
        break;

    case BLT: // Branch if less than
        *PC = (register_array[decoded_instruction->rs] < register_array[decoded_instruction->rt])
            ? (register_array[decoded_instruction->rm] & MASK_12_BIT)
            : (*PC + 1);
        break;

    case BGT: // Branch if greater than
        *PC = (register_array[decoded_instruction->rs] > register_array[decoded_instruction->rt])
            ? (register_array[decoded_instruction->rm] & MASK_12_BIT)
            : (*PC + 1);
        break;

    case BLE: // Branch if less or equal
        *PC = (register_array[decoded_instruction->rs] <= register_array[decoded_instruction->rt])
            ? (register_array[decoded_instruction->rm] & MASK_12_BIT)
            : (*PC + 1);
        break;

    case BGE: // Branch if greater or equal
        *PC = (register_array[decoded_instruction->rs] >= register_array[decoded_instruction->rt])
            ? (register_array[decoded_instruction->rm] & MASK_12_BIT)
            : (*PC + 1);
        break;

    case JAL: // Jump and link
        // Save the return address (address of the next instruction)
        register_array[decoded_instruction->rd] = *PC + 1;
        // Jump to the target address masked to 12 bits
        *PC = register_array[decoded_instruction->rm] & MASK_12_BIT;


        break;

    default:
        // This should never happen due to the initial validation
        printf("Unexpected opcode encountered: %d\n", decoded_instruction->opcode);
        break;
    }

}

// Executes memory-related operations like lw (load word) and sw (store word)
void load_store_operation(instruction_decode* decoded_instruction, int data_memory[MEM_SIZE], int register_array[REG_NUM], int* PC)
{
    /*
        INPUT:
        - decoded_instruction: Decoded instruction containing opcode and operands.
        - data_memory: Data memory array (32-bit words).
        - register_array: CPU register array.
        - PC: Pointer to the Program Counter.

        FUNCTIONALITY:
        - lw (Load Word): Loads a word from data memory into a register.
        - sw (Store Word): Stores a word from a register into data memory.
    */

    // Compute memory address using base register (rs) and rt
    unsigned int address = register_array[decoded_instruction->rs] + register_array[decoded_instruction->rt];


    switch (decoded_instruction->opcode)
    {
    case LW: // lw (Load Word)
        if (address >= MEM_SIZE)
        {
            fprintf(stderr, "Error: Memory access out of bounds during 'lw'. Address: %u\n", address);
            return;
        }
        register_array[decoded_instruction->rd] = data_memory[address] + register_array[decoded_instruction->rm];
        
        break;

    case SW: // sw (Store Word)
        if (address >= MEM_SIZE)
        {
            fprintf(stderr, "Error: Memory access out of bounds during 'sw'. Address: %u\n", address);
            return;
        }
        data_memory[address] = register_array[decoded_instruction->rd] + register_array[decoded_instruction->rm];

        
        break;

    default:
        fprintf(stderr, "Error: Unsupported memory operation opcode: %d\n", decoded_instruction->opcode);
        return;
    }

    // Increment PC after successful memory operation
    (*PC)++;
}

