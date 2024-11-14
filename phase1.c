#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

char memory[100][4];
char buffer[40];
char ir[4];              // instraction register
int ic = 0, si = 0;      // instraction counter, si interrupt 1= GD, 2= PD, 3= H
bool c = false;          // if true, will jump to.. acts for BT
char r[4];               // (general purpose register = r) load register of 4 bytes
FILE *inputf;            // file pointer to read
FILE *outputf;

void clearBuffer() {
    for (int m = 0; m < 40; m++) {
        buffer[m] = '\0';
    }
}

void read() {   // in memory
    int i = (ir[2] - '0') * 10 + (ir[3] - '0');   // last 2 char of any instrations, eg - GD10 so we will need 10
    int k = 0;
    clearBuffer();

    fgets(buffer, 41, inputf);
    while (k < 40 && buffer[k] != '\0') {   // k = buffer location
        for (int j = 0; j < 4; j++) {
            if (buffer[k] == '\n') {
                k++;
                continue;
            }
            memory[i][j] = buffer[k];
            ++k;
        }
        ++i;
    }
    clearBuffer();  // need lmited instractions
}

void write() {    // in output file
    outputf = fopen("output1.txt", "a");   // a is append, w is clear previous output and write latest instraction output
    for (int i = (ir[2] - '0') * 10; i < ((ir[2] - '0' + 1) * 10); i++) {
        for (int k = 0; k < 4; k++) {
            if (memory[i][k] != '\0') {
                fprintf(outputf, "%c", memory[i][k]);
            }
            else{ 
            fprintf(outputf, " ");
            }
        }
    }
    fprintf(outputf, "%c", '\n');
    fclose(outputf);
}

void terminate() {
    outputf = fopen("output1.txt", "a");
    fprintf(outputf, "\n");
    fprintf(outputf, "\n");
    fclose(outputf);
}

void mos() {
    if (si == 1) {
        read();
    } else if (si == 2) {
        write();
    } else if (si == 3) {
        terminate();
    }
    si = 0;
}

void executeprogram() {
    ic = 0;
    while (ic < 100 && memory[ic][0] != '\0') {  
        for (int i = 0; i < 4; i++) {
            ir[i] = memory[ic][i];
        }
        ++ic;                   // to store 2nd instraction
        if (ir[0] == 'G' && ir[1] == 'D') {
            si = 1;
            mos();
        } else if (ir[0] == 'P' && ir[1] == 'D') {
            si = 2;
            mos();
        } else if (ir[0] == 'H') {
            si = 3;
            mos();
            break;  // H is terminate

        } else if (ir[0] == 'L' && ir[1] == 'R') {
            int i = (ir[2] - '0') * 10 + (ir[3] - '0');
            for (int j = 0; j < 4; j++) {
                r[j] = memory[i][j];
            }
        } else if (ir[0] == 'S' && ir[1] == 'R') {
            int i = (ir[2] - '0') * 10 + (ir[3] - '0');
            for (int j = 0; j < 4; j++) {
                memory[i][j] = r[j];
            }
        } else if (ir[0] == 'C' && ir[1] == 'R') {
            int i = (ir[2] - '0') * 10 + (ir[3] - '0');
            int cnt = 0;
            for (int j = 0; j < 4; j++) {
                if (memory[i][j] == r[j]) {
                    ++cnt;
                }
            }
            if (cnt == 4) {
                c = true;
            } else {
                c = false;
            }
        } else if (ir[0] == 'B' && ir[1] == 'T') {
            int i = (ir[2] - '0') * 10 + (ir[3] - '0');
            if (c == true) {
                ic = i;
            }
        }
    }
}

void strt() {
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 4; j++) {
            memory[i][j] = '\0';
        }
    }
    for (int i = 0; i < 4; i++) { 
        ir[i] = '\0';
        r[i] = '\0';
    }
    for (int i = 0; i < 40; i++) {
        buffer[i] = '\0';
    }
    ic = 0;
    c = false;
    si = 0;
}

void startexecution() {
    ic = 0;
    executeprogram();
}


void load() {
    if (inputf == NULL) {
        printf("File doesn't exist or empty.");
        return;
    }

    while (fgets(buffer, 41, inputf) != NULL) {   // 41 = last would be \0
        if (buffer[0] == '$' && buffer[1] == 'A' && buffer[2] == 'M' && buffer[3] == 'J') {
            strt();
        } else if (buffer[0] == '$' && buffer[1] == 'D' && buffer[2] == 'T' && buffer[3] == 'A') {
            clearBuffer();
            startexecution();
        } else if (buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D') {
            printf("end");
         } else {
             int k = 0;
             while (k < 40 && buffer[k] != '\0' && buffer[k] != '\n') {
                 for (int j = 0; j < 4; j++) {
                    if (buffer[k] == 'H' && buffer[k] != '\n') {
                         memory[ic][j] = 'H';
                           ++k;
                         break;
                    }
                    memory[ic][j] = buffer[k];
                    ++k;
                }
                ++ic;
            }
        }
        clearBuffer();
    }

    for (int p = 0; p < 100; p++) {
        printf("M[%d]", p);
        for (int q = 0; q < 4; q++) {
            printf("%c ", memory[p][q]);
        }
        printf("\n");
    }
    fclose(inputf);
}

int main() {
    inputf = fopen("input1.txt", "r");
    strt();
    load();
    return 0;
}