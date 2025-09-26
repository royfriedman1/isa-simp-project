#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LABEL 50
#define MAX_LINE 500
#define CMD_MEM_LINES_SIZE 4096

#define HEX_INSTRUCTION_LENGTH 12
#define HEX_DATA_LENGTH 8
#define OP_LENGTH 2
#define REG_LENGTH 1
#define IMM_LENGTH 3


typedef struct Command{
    int opcode;
    int rd;
    int rs;
    int rt;
    int rm;
    int immediate1;
    int immediate2;
    int is_pseudo;
}Command;

typedef struct Label{
    char name[MAX_LABEL+1]; //we add +1 for the null terminator
    int adress;
    struct Label* next;
}Label_Node;


/*Add label to the chain*/
Label_Node* add_label(char* name, int address){
    Label_Node* L = (Label_Node*)malloc(sizeof(Label_Node));
    memset(L->name, '\0', (MAX_LABEL+1)*sizeof(char)); //initialize the name argument to null chars for easy handling
    strcpy(L->name, name);
    L->name[strlen(L->name)-1] = '\0'; //removing the ':' char at the end
    L->adress = address;
    L->next = NULL;
    return L;
}


/*Search label node in the chain*/
Label_Node* find_label(Label_Node* head, char* name){
    Label_Node* found_label = head;
    while (found_label != NULL)
    {
        if (strcmp(found_label->name, name)==0)
        {
            return found_label;
        }
        found_label = found_label->next;
    }
    
}


/*Recieves a string line and checks if it is a label*/
int is_label(char* str)
{
    int len = strlen(str);

    //Removing any trailing spaces after the ':' character
	for (int i = 0; i < strlen(str); i++)
    {
        if (str[i-1] == ':') //if we found the end of the label name and are now after the ':' char
        {
            for (int j = i; j < strlen(str); j++)
            {
                if (str[j] != '\n')
                {
                    str[j] = '\0'; //remove everything after it
                }
            }  
        }
    }

    len = strlen(str); //updating to the new length
    if (len<=MAX_LABEL+1 && isalpha(str[0]) && str[len-1] == ':')
    {
        return 1;
    }
    return 0;
}


/*Remove comments from line*/
void remove_comments(char* line) {
	for (int i = 0; i < strlen(line); i++)
    {
        if (line[i] == '#') //if we found the begginning of the comment
        {
            for (int j = i; j < strlen(line); j++)
            {
                line[j] = '\0'; //remove everything after it
            }  
        }
    }
}

/*Function to recieve a line and return a copy of it without spaces and tabs*/
char* remove_spaces(char* line) {
	char new_line[MAX_LINE] = { 0 };
	char* new_line_p = new_line;
    remove_comments(new_line_p);
	char* head_new_line = new_line;
	while (*line != '\0') //iterate until we reach the comment or the end of the line
	{
		if (((*line != ' ') && (*line != '\t'))) //if the current char isnt a space
        {
			*new_line_p = *line; //copy the char to the new line
			new_line_p++; //advance the new line char pointer
		}
		line++;
	}
	*new_line_p = '\0'; //end the line with null terminator
	return head_new_line;
}


/*Function to reset an array*/
void reset_array(char* str)
{
    memset(str, '\0', strlen(str) * sizeof(char));
}


/*Function that recieves a SIMP string of a register and returns it's int value*/
int find_opcode(char* op_str)
{
    int opcode;

    if (strcmp(op_str, "add") == 0) opcode = 0;
    else if (strcmp(op_str, "sub") == 0) opcode = 1;
    else if (strcmp(op_str, "mac") == 0) opcode = 2;
    else if (strcmp(op_str, "and") == 0) opcode = 3;
    else if (strcmp(op_str, "or") == 0) opcode = 4;
    else if (strcmp(op_str, "xor") == 0) opcode = 5;
    else if (strcmp(op_str, "sll") == 0) opcode = 6;
    else if (strcmp(op_str, "sra") == 0) opcode = 7;
    else if (strcmp(op_str, "srl") == 0) opcode = 8;
    else if (strcmp(op_str, "beq") == 0) opcode = 9;
    else if (strcmp(op_str, "bne") == 0) opcode = 10;
    else if (strcmp(op_str, "blt") == 0) opcode = 11;
    else if (strcmp(op_str, "bgt") == 0) opcode = 12;
    else if (strcmp(op_str, "ble") == 0) opcode = 13;
    else if (strcmp(op_str, "bge") == 0) opcode = 14;
    else if (strcmp(op_str, "jal") == 0) opcode = 15;
    else if (strcmp(op_str, "lw") == 0) opcode = 16;
    else if (strcmp(op_str, "sw") == 0) opcode = 17;
    else if (strcmp(op_str, "reti") == 0) opcode = 18;
    else if (strcmp(op_str, "in") == 0) opcode = 19;
    else if (strcmp(op_str, "out") == 0) opcode = 20;
    else if (strcmp(op_str, "halt") == 0) opcode = 21;
    else opcode = -1; // If no match is found, return -1 (or some error value)

    return opcode;
}


/*Function that recieves a SIMP string of a register and returns it's int value*/
int find_register(char* reg_str)
{
    int reg;

    if (strcmp(reg_str, "$zero") == 0) reg = 0;
    else if (strcmp(reg_str, "$imm1") == 0) reg = 1;
    else if (strcmp(reg_str, "$imm2") == 0) reg = 2;
    else if (strcmp(reg_str, "$v0") == 0) reg = 3;
    else if (strcmp(reg_str, "$a0") == 0) reg = 4;
    else if (strcmp(reg_str, "$a1") == 0) reg = 5;
    else if (strcmp(reg_str, "$a2") == 0) reg = 6;
    else if (strcmp(reg_str, "$t0") == 0) reg = 7;
    else if (strcmp(reg_str, "$t1") == 0) reg = 8;
    else if (strcmp(reg_str, "$t2") == 0) reg = 9;
    else if (strcmp(reg_str, "$s0") == 0) reg = 10;
    else if (strcmp(reg_str, "$s1") == 0) reg = 11;
    else if (strcmp(reg_str, "$s2") == 0) reg = 12;
    else if (strcmp(reg_str, "$gp") == 0) reg = 13;
    else if (strcmp(reg_str, "$sp") == 0) reg = 14;
    else if (strcmp(reg_str, "$ra") == 0) reg = 15;
    else reg = -1; // Return -1 or any error code if no match is found

    return reg;
}


/*Function that recieves a SIMP string of the immediate field and tells if it's a decimal number, hexadecimal number, or a label*/
int find_immediate_type(char* imm_str)
{
    int imm_type;

    //checking if imm_str is in hexadecimal representation
    if (imm_str[0] == '0' && imm_str[1] == 'x')
    {
        imm_type = 16;
    }
    else if ((imm_str[0]>=65 && imm_str[0]<=90) || (imm_str[0]>=97 && imm_str[0]<=122)) //if imm_str starts with a letter then it's a label
    {
        imm_type = -1;
    }
    else
    {
        imm_type = 10; //if the 2 other tests failed then it's in decimal representation
    }

    return imm_type;
}


/*Function that recieves a string of the immediate field and it's base representation(10, 16, or label) and returns it's int value*/
int* string_to_int(char *str, int base, Label_Node* head) {
    int result = 0;
    int* res_ptr;
    int digit_value;
    int sign = 1;
    
    //Loop through each character of the string
    int i = 0;
    
    //handling a label and returning it's address
    if (base == -1)
    {
        Label_Node* label = find_label(head, str);
        if (label != NULL)
        {
            return &label->adress;
        }
        else
        {
            return NULL;
        }
    }
    

    //if the string is hexadecimal then we will skip the "0x" initial two chars.
    if (base == 16)
    {
        i = 2;
    }
    
    char current;
    while (str[i] != '\0')
    {
        current = str[i];

        //check if negative decimal
        if (i == 0 && current == '-')
        {
            sign = -1;
            i++;
            continue; //continue to next char
        }
        
        //Convert the character to uppercase to handle both cases uniformly (relevant only if the base is hexadecimal)
        current = toupper(current);

        //Convert the character to its corresponding integer value
        if (isdigit(current))
        {
            digit_value = current - '0';  // For '0' to '9'
        }
        else if (isalpha(current))
        {
            digit_value = current - 'A' + 10;  // For 'A' to 'F'
        }
        else
        {
            //Invalid character for the given base
            return NULL;
        }

        //Check if the digit value is valid in the specified base
        if (digit_value >= base) {
            return NULL;  // Invalid digit for the base
        }

        //Accumulate the result, multiplying by the base for each digit
        result = result*base + digit_value;
        i++;
    }
    result = result * sign;
    res_ptr = &result;
    return res_ptr;
}


/*Recieve an int and create string of it's hex representation*/
char* int_to_hex(int num) {
    // Allocate memory for the string that will hold the hexadecimal value.
    // We use 4 characters: 3 hexadecimal digits + null-terminator
    char* hexStr = (char*)malloc(4 * sizeof(char));

    if (hexStr == NULL) {
        // Memory allocation failed
        return NULL;
    }

    // Convert integer to hexadecimal string
    snprintf(hexStr, 4, "%X", num & 0xFFF);

    return hexStr;
}


/*Concatenate the hex representation of an argument to the final hex command string*/
void concat_hex_str(char* hexStr, int parameter, int parameter_bit_len)
{
    char* parameter_hex_str = int_to_hex(parameter); //get hex representation of the argument
    int string_len = strlen(parameter_hex_str); //length of the hex representation
    int hexStr_end = strlen(hexStr); //where we finished writing the previous argument
    int i = hexStr_end;

    while (i < hexStr_end + (parameter_bit_len - string_len))
    {
        hexStr[i] = '0'; //pad with zeros the unnecessary bits in the final hex string
        i++;
    }
    
    strcat(hexStr, parameter_hex_str);
    free(parameter_hex_str);
}


/*Recieve a command struct and return a hex string that represents the command*/
char* cmd_to_hex_line(Command* cmd)
{
    // Allocate memory for the string that will hold the hexadecimal value.
    // We use 9 characters: "0x" + 8 hexadecimal digits + null-terminator
    char* hexStr = (char*)malloc((HEX_INSTRUCTION_LENGTH+2)* sizeof(char));
    memset(hexStr, '\0', (HEX_INSTRUCTION_LENGTH+2)* sizeof(char));

    if (hexStr == NULL) {
        return NULL;
    }
    reset_array(hexStr);
    
    //opcode
    concat_hex_str(hexStr, cmd->opcode, OP_LENGTH);

    //rd
    concat_hex_str(hexStr, cmd->rd, REG_LENGTH);

    //rs
    concat_hex_str(hexStr, cmd->rs, REG_LENGTH);

    //rt
    concat_hex_str(hexStr, cmd->rt, REG_LENGTH);

    //rm
    concat_hex_str(hexStr, cmd->rm, REG_LENGTH);

    //immediates
    concat_hex_str(hexStr, cmd->immediate1, IMM_LENGTH);
    concat_hex_str(hexStr, cmd->immediate2, IMM_LENGTH);

    return hexStr;
}


/*Handling a pseudo instruction*/
void** get_pseudo_instruction(char* raw_command_line)
{
    char* adjusted_line = raw_command_line;
    remove_comments(adjusted_line); //removing the comments from the command

    void** address_and_value = (void**)malloc(2*sizeof(void*));
    if (!address_and_value)
    {
        printf("Memory allocation failed");
        return NULL;
    }
    
    address_and_value[0] = (int*)malloc(sizeof(int)); //address will be in index 0
    if (!address_and_value[0])
    {
        free(address_and_value);
        printf("Memory allocation failed");
        return NULL;
    }

    address_and_value[1] = (char*)malloc((HEX_DATA_LENGTH+2)* sizeof(char)); //value will be in index 0
    if (!address_and_value[1])
    {
        free(address_and_value[0]);
        free(address_and_value);
        printf("Memory allocation failed");
        return NULL;
    }
    memset(address_and_value[1],'\0',(HEX_DATA_LENGTH+1)* sizeof(char));
    
    int base;

    char delim[] = " ,\t";
    char* token = strtok(adjusted_line, delim); //The first token will be the ".word"

    token = strtok(NULL, delim); //second token is address
    base = find_immediate_type(token);
    *(int*)address_and_value[0] = *string_to_int(token, base, NULL);
    
    token = strtok(NULL, delim); //third token is value
    base = find_immediate_type(token);
    if (base == 16) //if the value is already in hex representation
    {
        strcpy(address_and_value[1], token+2); //+2 to ignore the "0x"
    }
    else
    {
        concat_hex_str(address_and_value[1], *string_to_int(token, base, NULL), HEX_DATA_LENGTH);
    }
    if (*(int*)address_and_value[0] != 4095)
    {
        strcat((char*)address_and_value[1], "\n");
    }
    
    token = strtok(NULL, delim); //fourth token is NULL

    return address_and_value;
}


/*Initialize dmem_arr to zeros*/
void dmem_initialize(char dmem_arr[][10]) {
    for (int i = 0; i < CMD_MEM_LINES_SIZE; i++) {
        strcpy(dmem_arr[i], "00000000");
        if (i != CMD_MEM_LINES_SIZE - 1) {
            strcat(dmem_arr[i], "\n");
        }
    }
}

/*Function to print the entire 2D array to a file*/
void print_2D_array_to_file(char array[][10], FILE* file, int last_line) {
    if (array == NULL) {
        printf("Array is NULL, nothing to print\n");
        return;
    }

    //Loop through all lines and write to the file
    for (int i = 0; i < last_line+1; i++) {
		if (i != last_line)
		{
			fprintf(file, "%s", &array[i]);
		}
		else
		{
			int j = 0;
			while (array[i][j] != '\n')
			{
				fprintf(file, "%c", array[i][j]);
				j++;
			}
		}
    }

    fclose(file);
}


/*Recieve a string of a command line in SIMP format and all the labels, and return a command struct*/
Command* buildCommand(char* raw_command_line, Label_Node* labels_head)
{
    char* adjusted_line = raw_command_line;
    remove_comments(adjusted_line); //removing the comments from the command

    Command* cmd = (Command*)malloc(sizeof(Command)); //intialization

    char delim[] = " ,\t";
    char* token = strtok(adjusted_line, delim); //The first token will be the opcode connected to the first register without spaces

    //discovering the opcode
    int opcode;
    opcode = find_opcode(token);
    if (opcode == -1)
    {
        return NULL;
    }
    
    //discovering the registers, using an array to loop the operation 4 times
    int reg_arr[4] = {0};
    for (int i = 0; i < sizeof(reg_arr)/4; i++)
    {
        token = strtok(NULL, delim);
        reg_arr[i] = find_register(token);
        if (reg_arr[i] == -1) {
            return NULL;
        }
    }
    
    //discovering the immediates, using an array to loop the operation 2 times
    int imm_arr[2] = {0};
    for (int i = 0; i < sizeof(imm_arr)/4; i++)
    {
        token = strtok(NULL, delim); //Next token is immediate1
        int* imm_ptr =  string_to_int(token, find_immediate_type(token), labels_head);
        if (imm_ptr != NULL)
        {
            imm_arr[i] = *imm_ptr;
        }
        else return NULL;
    }


    token = strtok(NULL, delim); //Next token is NULL
    
    //constructing the struct
    cmd->opcode = opcode;
    cmd->rd = reg_arr[0];
    cmd->rs = reg_arr[1];
    cmd->rt = reg_arr[2];
    cmd->rm = reg_arr[3];
    cmd->immediate1 = imm_arr[0];
    cmd->immediate2 = imm_arr[1];

    return cmd;
}

int main(int argc, char* argv[]){

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input.asm> <output.imemin> <output.dmemin>\n", argv[0]);
        return 1;
    }
    

    FILE* program_ptr = fopen(argv[1], "r");
    FILE* imemin_ptr = fopen(argv[2], "w");
    FILE* dmemin_ptr = fopen(argv[3], "w");

    if (program_ptr == NULL) {
        printf("Error openning file program.asm");
        return 1;
    }
    if (imemin_ptr == NULL) {
        printf("Error openning file imemin.txt");
        return 1;
    }
    if (dmemin_ptr == NULL) {
        printf("Error openning file dmemin.txt");
        return 1;
    }

    /*initialize dmemin to 4096 lines of 8 digits of 0*/
    char dmem_arr[CMD_MEM_LINES_SIZE][10];
    dmem_initialize(dmem_arr);
    
    char str_cmd[MAX_LINE] = {0};
    int found_label = 0;
    Label_Node* head = NULL;
    Label_Node* last = NULL;

    /*FIRST READING, FINDING ALL LABELS AND REMEMBERING THEIR ADDRESSES*/
    int PC = 0;
    int last_line = -1;
    int scan_element = fscanf(program_ptr, "%[^\n]", str_cmd);
    do
    {
        if (scan_element == 0)
        {
            fgetc(program_ptr); //flush the \n
            scan_element = fscanf(program_ptr, "%[^\n]", str_cmd);
            continue;
        }
        
        fgetc(program_ptr); //flush the \n
        if (is_label(str_cmd))
        {
            found_label = 1;
            if (head == NULL) //if it is the first label
            {
                 head = add_label(str_cmd, PC);
                 last = head;
            }
            else
            {
                last->next = add_label(str_cmd, PC);
                last = last->next;
            }
        }
        else
        {
            PC++;
            if (!strstr(str_cmd, ".word"))
            {
                last_line++;
            }
            if (found_label == 1)
            {
                found_label = 0;
            }
        }
        scan_element = fscanf(program_ptr, "%[^\n]", str_cmd);
    } while (scan_element != EOF);

    

    /*SECOND READING, ANALYZING SIMP COMMANDS*/
    rewind(program_ptr);
    Command* cmd;
    PC = 0;
	int data_last_line = 0; //The last relevant line in dmemin.txt. Will be used so we can ignore the remaining irrelevant lines after that.
    scan_element = fscanf(program_ptr, "%[^\n]", str_cmd);
    char* line_no_space;
    do
    {
        line_no_space = remove_spaces(str_cmd);
        remove_comments(line_no_space);
        if (scan_element == 0 || is_label(str_cmd) || strlen(line_no_space)==0)
        {
            fgetc(program_ptr); //flush the \n
            scan_element = fscanf(program_ptr, "%[^\n]", str_cmd);
            continue;
        }
        else
        {
            fgetc(program_ptr); //flush the \n
            if (strstr(str_cmd, ".word"))
            {
                void** pseudo = get_pseudo_instruction(str_cmd);
                strcpy(dmem_arr[*(int*)pseudo[0]], (char*)pseudo[1]);
				if (data_last_line < *(int*)pseudo[0])
				{
					data_last_line = *(int*)pseudo[0]; //updating last relevant line in dmemin.
				}
                free(pseudo[0]);
                free(pseudo[1]);
                free(pseudo);
            }
            else
            {
                cmd = buildCommand(str_cmd, head);
                if (cmd == NULL)
                {
                    printf("Error in assembly code\n");
                    return 1;
                }
                else
                {
                    fprintf(imemin_ptr, cmd_to_hex_line(cmd));
                    if (PC != last_line)
                    {
                        fprintf(imemin_ptr, "\n");
                    }
                    free(cmd);
                }
            }
            PC++;
        }
        scan_element = fscanf(program_ptr, "%[^\n]", str_cmd);
    } while (scan_element != EOF);

    print_2D_array_to_file(dmem_arr, dmemin_ptr, data_last_line);
    
    fclose(program_ptr);
    fclose(imemin_ptr);
    fclose(dmemin_ptr);

    return 1;
}