#include "load_jobs.h"

void main(){
    FILE* file = open_file();
    int total_jobs = Job_counter(file);
    for( int i =  0 ; i < total_jobs ; i++ ){
        init();
        load_program_card(file);
        execute_program_card(file);
    }
    close_file( file );
}

/*
FILE* file = open_file();
    init();
    load_program_card(file);
    execute_program_card(file);
    init();
    load_program_card(file);
    execute_program_card(file);
    init();
    load_program_card(file);
    execute_program_card(file);
    init();
    load_program_card(file);
    execute_program_card(file);
    init();
    load_program_card(file);
    execute_program_card(file);
    init();
    load_program_card(file);
    execute_program_card(file);
    init();
    load_program_card(file);
    execute_program_card(file);
    init();
    load_program_card(file);
    execute_program_card(file);
*/