/**
 * @file utils.c
 * @brief Provides utility functions for the simulator.
 *
 * This file includes general-purpose helper functions, such as conversions,
 * validations, and other reusable functionality.
 *
 * Functions:
 * - str_hex_2_bin: Converts a portion of a hex string to an integer.
 */

#include "simulator_functions.h"

 // Converts a portion of a hex string into its decimal equivalent
int str_hex_2_bin(const char* string, int start_index, int slice_width)
{
    /*
        Converts a slice of a hexadecimal string into an integer value.
        INPUT: string (hex string), start_index (start of the slice), slice_width (length of the slice)
        OUTPUT: Returns the integer value of the sliced hex string, or -1 if the input is invalid.
    */

    // Validate inputs
    if (string == NULL || start_index < 0 || slice_width <= 0 || start_index + slice_width > strlen(string))
    {
        return -1; // Return error code for invalid input
    }

    // Use a static buffer to avoid dynamic memory allocation
    char slice[16]; // Assuming the slice will not exceed 15 characters
    strncpy(slice, string + start_index, slice_width);
    slice[slice_width] = '\0'; // Null-terminate the string

    // Convert the slice to an integer
    char* end_ptr;
    int result = strtol(slice, &end_ptr, 16);

    // Check for conversion errors
    if (*end_ptr != '\0')
    {
        return -1; // Invalid character in the hex string
    }

    return result;
}

