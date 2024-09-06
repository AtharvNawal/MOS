#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char Memory[100][4];
char R[4], IR[4];
int IC = 0, SI = 3;
int input_ptr = 0, output_ptr = 0;
bool C = false;
char buffer[41] = "";

void print_mem() {
    printf("\n\nMEMORY\n");
    for (int i = 0; i < 100; i++) {
        for (int k = 0; k < 4; k++) {
            printf("%c ", Memory[i][k]);
        }
        printf("\n");
    }
}

void clearContents() {
    // Memory Initialized by '_'
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 4; j++) {
            Memory[i][j] = '_';
        }
    }
    buffer[0] = '\0';
    memset(R, 0, sizeof(R));
    IC = 0;
    C = false;
}

void lineReader(const char *temp) {
    FILE *fout = fopen("output.txt", "a");
    if (fout == NULL) {
        printf("Error opening file.\n");
        return;
    }
    fprintf(fout, "%s\n", temp);
    fclose(fout);
}

void READ(int block) {   
    printf("\nGD\n");
    
    // Obtaining data from input file
    FILE *fin = fopen("job_phase1.txt", "r");
    if (fin == NULL) {
        printf("Error opening file.\n");
        return;
    }

    char line[41];
    for (int i = 0; i < input_ptr; i++) {
        fgets(line, sizeof(line), fin);
    }
    // WE SKIP LINES
    fgets(line, sizeof(line), fin);  // THIS LINE IS PROGRAM CARD
    input_ptr++;
    fclose(fin);
    
    strncpy(buffer, line, 40);
    buffer[40] = '\0';
    for (int i = 0; i < strlen(buffer) && i < 40; i++) {
        int a = i / 4 + block;     
        int b = i % 4;
        Memory[a][b] = buffer[i];
    }
}

void WRITE(int block) {
    printf("\nPD\n");
    char temp[41] = "";
    int i = 0;
    while (i < 40) {
        int a = i / 4 + block;     
        int b = i % 4;
        if (Memory[a][b] == '_') {
            temp[i] = ' ';
        } else {
            temp[i] = Memory[a][b];
        }
        i++;
    }
    temp[40] = '\0';
    
    lineReader(temp);
}

int TERMINATE(int si) {
    if (si == 3)
        return 1;
    else 
        return 0;
}

void executeUserProgram() {
    while (1) {
        // Bringing instr to IR
        for (int i = 0; i < 4; i++) {
            IR[i] = Memory[IC][i];
        }

        // Halt
        if (IR[0] == 'H') {
            TERMINATE(SI);
            break;
        }

        // Address Extraction
        printf("IR: %c %c %c %c\n", IR[0], IR[1], IR[2], IR[3]);
        int t1 = ((int)IR[2]) - ((int)'0');
        int t2 = ((int)IR[3]) - ((int)'0');
        int block = t1 * 10 + t2;
        printf("Start Block: %d\n\n", block);

        // Checking Instruction
        if (IR[0] == 'G' && IR[1] == 'D') {   
            SI = 1;
            READ(block);
        } else if (IR[0] == 'P' && IR[1] == 'D') {   
            SI = 2;
            WRITE(block);
        } else if (IR[0] == 'L' && IR[1] == 'R') {
            printf("\n");
            for (int i = 0; i < 4; i++) {
                R[i] = Memory[block][i];
            }
        } else if (IR[0] == 'C' && IR[1] == 'R') {
            bool f = false;
            for (int i = 0; i < 4; i++) {
                if (R[i] != Memory[block][i]) {
                    f = true;
                    break;
                }
            }
            C = !f;
        } else if (IR[0] == 'S' && IR[1] == 'R') {
            for (int j = 0; j < 4; j++) {
                Memory[block][j] = R[j];
            }
        } else if (IR[0] == 'B' && IR[1] == 'T') {   
            if (C == true) {
                IC = block - 1;   
            }
        }

        // Incrementing program counter
        IC++;
    }

    printf("\n\n #############   Program DONE   ########### \n\n");
}

void startExecution() {
    IC = 0;
    executeUserProgram();
    
    // Leaving two lines for each job
    FILE *fout = fopen("output.txt", "a");
    if (fout == NULL) {
        printf("Error opening file.\n");
        return;
    }
    fprintf(fout, "\n\n");
    fclose(fout);
}

void LOAD() {
    FILE *fin;
    char line[41];
    
    while (1) {   
        // Start file handling        
        fin = fopen("job_phase1.txt", "r");
        if (fin == NULL) {
            printf("Error opening file.\n");
            return;
        }

        for (int j = 0; j < input_ptr; j++) {
            fgets(line, sizeof(line), fin);
        }
        fgets(line, sizeof(line), fin);
        input_ptr++;
        fclose(fin);

        // For last job in input file
        if (strncmp(line, "$END", 4) == 0)
            break;

        // For next job in input file
        if (strncmp(line, "$AMJ", 4) == 0) {   
            // Clear Contents for a new JOB
            clearContents();

            // Job started loading
            char prog[41];
            fin = fopen("job_phase1.txt", "r");
            if (fin == NULL) {
                printf("Error opening file.\n");
                return;
            }

            for (int j = 0; j < input_ptr; j++) {
                fgets(prog, sizeof(prog), fin);
            }
            fgets(prog, sizeof(prog), fin);
            input_ptr++;
            printf("Program Card: %s\n", prog);

            int k = 0;
            while (strncmp(prog, "$DTA", 4) != 0) {
                // Loading inst to memory!                 
                for (int j = 0; j < strlen(prog); j++) {   
                    if (prog[j] == 'H') {
                        Memory[k / 4][0] = 'H';
                        k += 3;
                    } else {
                        int a = k / 4;
                        int b = k % 4;
                        Memory[a][b] = prog[j];
                    }
                    k++;
                }
                fgets(prog, sizeof(prog), fin);
                input_ptr++;
            }
            fclose(fin);
            
            startExecution();
            // Printing memory after every job
            print_mem();    
        }

        // Read Input till end of program
        fin = fopen("job_phase1.txt", "r");
        if (fin == NULL) {
            printf("Error opening file.\n");
            return;
        }
        for (int j = 0; j < input_ptr; j++) {
            fgets(line, sizeof(line), fin);
        }
        fgets(line, sizeof(line), fin);
        input_ptr++;

        while (strncmp(line, "$END", 4) != 0) {
            fgets(line, sizeof(line), fin);
            input_ptr++;
        }
        fgets(line, sizeof(line), fin);
        fclose(fin);
    }
}

int main() {
    LOAD();  
    return 0;
}