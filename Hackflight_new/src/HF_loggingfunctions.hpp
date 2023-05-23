#pragma once

#include "HF_debugger.hpp"
#include <String>
#include "HF_board.hpp"

namespace hf {
    #ifdef ARDUINO_BUILD
    char character_buf[30000];
    int character_buffer_index = 0;
    va_list ap;
    unsigned int last_time_printed = 5000000;
    #endif

    static void print_string(const char * fmt, ...){
        #ifdef ARDUINO_BUILD
        va_start(ap, fmt);

        if(character_buffer_index>=29800) character_buffer_index = 0;

        int size = vsnprintf(&character_buf[character_buffer_index], 200, fmt, ap);

        unsigned int current_time = micros();

        character_buffer_index += size;

        if (Serial && (character_buffer_index >= 29800 || current_time > last_time_printed + 15000000)){
            printf("Ws,%d\n", micros());
            Serial.write(character_buf, character_buffer_index);
            // Writing to serial completed at,%d\n
            printf("Wc,%d\n", micros());
            character_buffer_index = 0;
            last_time_printed = micros();
        }
        
        va_end(ap); 
        #endif
    }

    static void printTaskTime(int task_id, bool task_start)
    {
        #ifdef ARDUINO_BUILD
        unsigned int current_time = micros();
        //if(current_time > 5000000 && current_time < 6000000){
            // Task,%d,started at time,%ld\n
            if (task_start) {
                //print_string("T,%d,s,%ld\n", task_id, current_time);
            }
            // Task,%d,terminated at time,%ld\n
            else{
                //print_string("T,%d,t,%ld\n", task_id, current_time);
            }
        //} 
        #endif
    }

    static void printSpeeds(State* state)
    {
        /* unsigned int current_time = micros();
        if (current_time > 10000000 && current_time < 15000000) {
            print_string("Dx,%f,Dy,%f,Dz%f,DPH,%f,DT,%f,DPS,%f\n", state->x[State::DX], state->x[State::DY], state->x[State::DZ], state->x[State::DPHI], state->x[State::DTHETA], state->x[State::DPSI]);
        } */
    }
}

