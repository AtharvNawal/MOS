#include <stdio.h>
#include <stdbool.h> 
#include <string.h>
#include <stdlib.h>

FILE* output_file ;
//**********************Decleration**********************//
char main_memory[100][4] ;          // Decleration of main memory 

char buffer_memory[40] ;            // Declerartion of buffer memory

char instruction_register[4] ;      // Decleration of instruction register

char general_porpose_register[4] ;  // Decleration of general porpose register

int program_counter = 0 ;           // Decleration of program counter

bool togle_register = false ;       // Decleration of togle register

int serial_interrupt = 0 ;          // Decleration of serial interrupt

static bool data_card = false;      // Decleration of data card variable to check for data card is present in the job or not , declered globally for debugging porpose 
//**********************Initialisation Function**********************//
void init_FR(){
    togle_register = false ; 
}
void init_IC(){
    program_counter = 0 ; 
}
void init_R(){
    for( int i = 0 ; i < 4 ; i++ ){
        general_porpose_register[i] = '*';
    }
}
void init_IR(){
    for( int i = 0 ; i < 4 ; i++ ){
        instruction_register[i] = '*' ; 
    }
}
void init_buffer_memory(){
    for( int i = 0 ; i < 40 ; i++ ){
        buffer_memory[i] = '*' ;
    }
}
void init_main_memory(){
    for( int i = 0 ; i < 100 ; i++ ){
        for( int j = 0 ; j < 4 ; j++ ){
            main_memory[i][j] = '*';
        }
    }
}
void init_data_card(){
    data_card = false ;
}
void init(){
    init_data_card();
    init_FR();
    init_IC();
    init_R();
    init_IR();
    init_main_memory();
    init_buffer_memory();
}

//**********************Display Functions**********************//
void display_main_memory(){
    for( int i = 0 ; i < 70 ; i++ ){
        printf("      _  _  _  _");
        printf("\n");
        for( int j = 0 ; j < 5 ; j++ ){
            if( j == 0 ){
                if( i < 10){
                    printf("0%d ->", i);
                }
                else{
                    printf("%d ->", i);
                }
            }
            else{
                printf("|%c|",main_memory[i][j-1]);
            }
        }
        printf("\n");
        printf("      ^  ^  ^  ^\n");
        printf("      0  1  2  3\n");
    }
}
void display_buffer_memory(){
    for( int i = 0 ; i < 40 ; i++ ){
        if( i%4 == 0 ){
            printf("\n");
        }
        printf("|%c|",buffer_memory[i]);
    }
    printf("\n");
}
void display_CPU(){
    printf("Togle register: |%d|\t",togle_register);
    printf("program counter: |%d|",program_counter);
    printf("\n");
    printf("General porpose register: \n");
    for( int i = 0 ; i < 4 ; i++ ){
        printf("|%c| ",general_porpose_register[i]);
    }
    printf("\n");
    printf("instruction register: \n");
    for( int i = 0 ; i < 4 ; i++ ){
        printf("|%c| ",instruction_register[i]);
    }
    printf("\n");
}

//**********************File Functions**********************//

FILE* open_file(){
    char filename[100];
    char txt_extension[] = ".txt" ;
    char A_filename[104] ; 

     FILE *file ;

    printf("Enter the name of the file to open: " ) ;
    scanf("%s", filename ) ;

    strcat( A_filename , filename ) ;
    strcat( A_filename , txt_extension ) ;

    file = fopen( A_filename , "r" ) ;

    if (file == NULL) {
        printf("Error: Could not open file %s\n", filename);
    }

    output_file = fopen("Output.txt", "w");
    if (output_file == NULL) {
        printf("Error: Could not open output file output.txt\n");
        fclose(file);
        return NULL;
    }

    printf("File Opened successfully\n");
    return file;
}


int close_file(FILE *file) {
    fclose(output_file);
    if (file != NULL) {
        printf("\nFile closed\n");
        return fclose(file);
    } else {
        printf("Error: File is not open.\n");
        return EOF;
    }
}

//**********************Job Functions**********************//
int Job_counter(FILE* file){
    char line[100] ;
    int job_count = 0 ; 
    while(fgets(line,sizeof(line),file)){
        if(strstr(line,"$AMJ")){
            job_count++ ;
        }
    }
    rewind(file);
    return job_count ;
}
int check_for_$AMJ(FILE *file) {
    char buffer[5];
    if (fread(buffer, sizeof(char), 4, file) != 4) {
        printf("Error: Could not read the first four characters from the file.\n");
        return 0; 
    }

    buffer[4] = '\0';

    if (strcmp(buffer, "$AMJ") == 0) {
        return 1; 
    } else {
        return 0; 
    }
}
int check_for_DTA( FILE* file ){
    char str[100];
    while(fgets(str,sizeof(str),file)){
        if(strstr(str,"$DTA") !=NULL){
            return 0; 
        }
    }
    return 1 ; 
}

//**********************Load Functions**********************//

void load_program_card(FILE* file) {
    char instruction_line[40];
    bool program_card = false;
    int buffer_index = 0;

    while (fgets(instruction_line, sizeof(instruction_line), file)) {
        if (strstr(instruction_line, "GD") != NULL || strstr(instruction_line, "PD") != NULL || 
            strstr(instruction_line, "LR") != NULL || strstr(instruction_line, "SR") != NULL || 
            strstr(instruction_line, "CR") != NULL || strstr(instruction_line, "BT") != NULL || 
            strstr(instruction_line, "H") != NULL) {
            program_card = true;
        }
        if (strstr(instruction_line, "$DTA") != NULL) {
            program_card = false;
            fseek(file , -6 , SEEK_CUR);
            break;
        }

        if (program_card) {
            for (int i = 0; instruction_line[i] != '\0'; i++) {
                if (instruction_line[i] == '\n') {
                    continue;
                }
                if( instruction_line[i] == 'H'){
                    buffer_memory[buffer_index] = 'H' ;
                    buffer_memory[buffer_index + 1] = '*' ;
                    buffer_memory[buffer_index + 2] = '*' ;
                    buffer_memory[buffer_index + 3] = '*' ;
                    buffer_index += 4 ;
                }
                else{
                    buffer_memory[buffer_index++] = instruction_line[i];
                }

                if (buffer_index == sizeof(buffer_memory)) {
                    for (int i = program_counter, j = 0; i < program_counter + 10; i++) {
                        for (int k = 0; k < 4; k++, j++) {
                            if (buffer_memory[j] == '\n') {
                                continue;
                            }
                            main_memory[i][k] = buffer_memory[j];
                        }
                    }
                    init_buffer_memory();
                    program_counter += 10;  
                    buffer_index = 0;  
                }
            }
        }
    }

    if (buffer_index > 0) {
        for (int i = program_counter, j = 0; j < buffer_index && i < 100; i++) {
            for (int k = 0; k < 4 && j < buffer_index; k++, j++) {
                main_memory[i][k] = buffer_memory[j];
            }
        }
        program_counter += (buffer_index + 3) / 4; 
        init_buffer_memory();
    }
    init_IC();
    //printf("%s\n",instruction_line);
}

void load_instruction_register( char instruction_register[] , int program_counter ){
    strncpy(instruction_register , main_memory[program_counter],4);
}

//**********************Execution Functions**********************//

int MOS( int serial_interrupt , char instruction_register[] , FILE* file ){
     if (serial_interrupt == 1) {
        char data_line[40]; 
 
        //printf("%d\n",data_card);
        if (!data_card) {
            while (fgets(data_line, sizeof(data_line), file) != NULL) {
                //printf("%s",data_line);
                if (strstr(data_line, "$DTA") != NULL) {
                    data_card = true;
                    //printf("%d\n",data_card);
                    break;
                }
            }
        }

        if (data_card) {
            if (fgets(data_line, sizeof(data_line), file) != NULL) {
                //printf("%s\n",data_line);
                if (strstr(data_line, "$END") != NULL) {
                    data_card = false;
                    //printf("%d\n",data_card);
                    return 0;   
                }
                //printf("%s",instruction_register);
                char last_two_digits[3];
                last_two_digits[0] = instruction_register[2];
                last_two_digits[1] = instruction_register[3];
                last_two_digits[2] = '\0';  

                int location_to = atoi(last_two_digits);
                int data_length = strlen(data_line);
                int column = 0;
                int row = location_to; 

                for (int i = 0; i < data_length ; i++) {
                    if (column >= 4) {
                        column = 0;  
                        row++;    
                        if (row >= 100) {  
                            break; 
                        }
                    }
                    if (data_line[i] == '\n') {
                        break;
                    }
                    main_memory[row][column] = data_line[i];
                    //printf("%c",main_memory[row][column]);
                    column++;
                }
            }
        }
        return 0;
    } 
    else if (serial_interrupt == 2) {
        char last_two_digits[3];
        last_two_digits[0] = instruction_register[2];
        last_two_digits[1] = instruction_register[3];
        last_two_digits[2] = '\0';

        int location_from = atoi(last_two_digits);
        // Print output to terminal and write to output file
        for (int i = location_from; i < location_from + 10; i++) {
            for (int j = 0; j < 4; j++) {
                if (main_memory[i][j] == '*') {
                    printf(" ");
                    if (output_file != NULL) {
                        fprintf(output_file, " ");
                    }
                } else {
                    printf("%c", main_memory[i][j]);
                    if (output_file != NULL) {
                        fprintf(output_file, "%c", main_memory[i][j]);
                    }
                }
            }
        }
        printf("\n");
        if (output_file != NULL) {
            fprintf(output_file, "\n");
        }

        return 0;
    }
    else{
        //printf("\nEnd");
        printf("\n\n");
        fprintf(output_file,"\n\n");
        return 1 ; 
    }
}

//-----------------------------Decode Functions---------------------------//
int decode_instructions( FILE* file , char instruction_register[] ){
    if( strncmp(instruction_register , "GD" , 2) == 0 ){
            serial_interrupt = 1 ; 
            if( MOS(serial_interrupt , instruction_register , file ) ){
                return 0 ;
            }
        }
        else if( !strncmp(instruction_register, "PD" , 2) ){
            serial_interrupt = 2 ; 
            if( MOS(serial_interrupt , instruction_register , file ) ){
                return 0 ; 
            }
        }
        else if( !strncmp(instruction_register, "H" , 1) ){
            serial_interrupt = 3 ; 
            if( MOS(serial_interrupt , instruction_register , file ) ){
                return 1 ; 
            }
        }
        else if( !strncmp(instruction_register, "LR" , 2) ){
            //printf("%s",instruction_register);
            char last_two_digits[3];
            last_two_digits[0] = instruction_register[2];
            last_two_digits[1] = instruction_register[3];
            last_two_digits[2] = '\0';  

            int location_from = atoi(last_two_digits);
            //printf("%d",location_from);
            for( int i = 0 ; i < 4 ; i++ ){
                general_porpose_register[i] = main_memory[location_from][i] ; 
            }
            //printf("%s",general_porpose_register);
        }
        else if( !strncmp(instruction_register, "SR", 2) ){
            char last_two_digits[3];
            last_two_digits[0] = instruction_register[2];
            last_two_digits[1] = instruction_register[3];
            last_two_digits[2] = '\0';  

            int location_to = atoi(last_two_digits);
            //printf("%d ", location_to);
            for( int i = 0 ; i < 4 ; i++ ){
                main_memory[location_to][i] = general_porpose_register[i] ; 
            }
        }
        else if( !strncmp(instruction_register, "CR", 2) ){
            char last_two_digits[3];
            last_two_digits[0] = instruction_register[2];
            last_two_digits[1] = instruction_register[3];
            last_two_digits[2] = '\0';  

            int location_from = atoi(last_two_digits);
            if( strcmp( general_porpose_register , main_memory[location_from]) != 0 ){
                togle_register = true ; 
            }
            else{
                togle_register = false ; 
            }
        }
        else if( !strncmp(instruction_register, "BT", 2 ) ){
            char last_two_digits[3];
            last_two_digits[0] = instruction_register[2];
            last_two_digits[1] = instruction_register[3];
            last_two_digits[2] = '\0';  

            int jump_to = atoi(last_two_digits);
            if( togle_register ){
                program_counter = jump_to -1  ; 
            }
        }
        program_counter++ ; 
    return 0 ; 
}


void execute_program_card( FILE* file ){
    //rewind( file );
    //printf("%.4s\n",instruction_register);
    //fseek(file , 2 , SEEK_CUR);
    while( strncmp(main_memory[program_counter] , "*" , 1) ){

        load_instruction_register( instruction_register , program_counter );
        if( decode_instructions( file , instruction_register ) ){
            break ; 
        }
     }
}