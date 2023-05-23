/*
   Hackflight class supporting safety checks and serial communication

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "hf_boards/realboard.hpp"
#include "HF_pure.hpp"
#include "HF_serialtask.hpp"

namespace hf {

    class HackflightFull : public HackflightPure {

        private:

            // Serial tasks
            SerialTask * _serial_tasks[10] = {};
            uint8_t _serial_task_count = 0;

            void startSensors(void) 
            {
                for (uint8_t k=0; k<_sensor_count; ++k) {
                    _sensors[k]->begin();
                }
            }

        public:

            HackflightFull(RealBoard * board, Receiver * receiver, Mixer * mixer)
                : HackflightPure(board, receiver, mixer)
            {
                _serial_task_count = 0;
            }

            void begin(void)
            {  
                _state.armed = false;

                // Start the board
                _board->begin();

                // Initialize the sensors
                startSensors();

                // Initialize the open-loop controller
                _receiver->begin();

                // Start the mixer
                _mixer->begin();

            } // begin

            void update(void)
            {
                HackflightPure::update();

                //printSpeeds(&_state);
                print_string("");
            }

            void addSerialTask(SerialTask * task)
            {
                _serial_tasks[_serial_task_count++] = task;

                _update_scheduler.set_task_inter_arrival_time(task->task_id, 1000000 / task->FREQ);
                _update_scheduler.set_task_criticality(task->task_id, task->criticality);

                task->init(_receiver, _mixer, &_state);
            }

    }; // class HackflightFull

} // namespace
