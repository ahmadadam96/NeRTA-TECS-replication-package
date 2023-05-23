/*
   Core Hackflight class

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "HF_sensor.hpp"
#include "HF_board.hpp"
#include "HF_pidtask.hpp"
#include "HF_pidcontroller.hpp"
#include "HF_receiver.hpp"
#include "HF_mixer.hpp"
#include "HF_state.hpp"
#include "HF_update_scheduler.hpp"

//#define SCRIPT_BUILD

namespace hf {

    class HackflightPure {

        private:

            void checkSensors(State * state)
            {
                // Some sensors may need to know the current time
                float time = _board->getTime();

                for (uint8_t k=0; k<_sensor_count; ++k) {
                    if(_sensors[k]->ready(_board)){
                        unsigned int task_id = k + 2;
                        _update_scheduler.task_started(task_id);
                        printTaskTime(task_id, true);
                        _sensors[k]->modifyState((State *)state, time);
                        printTaskTime(task_id, false);
                        _update_scheduler.task_ended(task_id);
                    }
                }
            }

        protected:

            // Essentials
            Board * _board = NULL;
            Receiver * _receiver = NULL;
            Mixer * _mixer = NULL;
            State _state = {};

            // Sensors 
            Sensor * _sensors[256] = {};
            uint8_t _sensor_count = 0;

            // Update scheduler with the sensor count
            UpdateScheduler _update_scheduler;

        public:

            // Timer task for PID controllers
            PidTask _pidTask;

            HackflightPure(Board * board, Receiver * receiver, Mixer * mixer)
            {
                _board = board;
                _receiver = receiver;
                _mixer = mixer;

                _update_scheduler.init(&_pidTask, _receiver, _state.x, _board);

                _update_scheduler.set_task_inter_arrival_time(_pidTask.task_id, 1000000 / _pidTask.FREQ);
                _update_scheduler.set_task_criticality(_pidTask.task_id, _pidTask.criticality);
                _update_scheduler.set_task_inter_arrival_time(_receiver->task_id, 1000000 / _receiver->FREQ);
                _update_scheduler.set_task_criticality(_receiver->task_id, _receiver->criticality);

                _receiver->init(board, &_state, mixer);

                _sensor_count = 0;
            }

            void begin(void)
            {  
                _state.armed = true;

            } // begin

            void update(void)
            {
                static unsigned int count = 1;

                // For determining biggest update that can be scheduled
                //if (_update_scheduler.no_update() && count == 1 && _board->getTimeUS() > 5000000){
                    //_update_scheduler.initialize_scheduling(2800);
                //    count++;
                //}
                
                // For evaluating overhead.
                /*
                static unsigned int dice = 0;

                if (_update_scheduler.no_update() && _board->getTimeUS() > 5000000 && dice > 15){
                    _update_scheduler.initialize_scheduling(4000);
                    dice = 0;
                }
                */

                if(_receiver->ready(_board)){
                    _update_scheduler.task_started(_receiver->task_id);
                    printTaskTime(_receiver->task_id, true);
                    #ifndef SCRIPT_BUILD
                    if(_receiver->gotNewFrame()) {
                        _receiver->update();
                    }
                    #endif
                    printTaskTime(_receiver->task_id, false);
                    _update_scheduler.task_ended(_receiver->task_id);
                }

                // Update PID controllers task
                if (_pidTask.ready(_board)) {
                    _update_scheduler.task_started(_pidTask.task_id);
                    printTaskTime(_pidTask.task_id, true);
                    _pidTask.update(_board, _receiver, _mixer, &_state);
                    printTaskTime(_pidTask.task_id, false);
                    _update_scheduler.task_ended(_pidTask.task_id);
                }
                // Check sensors
                checkSensors(&_state);

                //if (_update_scheduler.no_update()) dice++;
            }

            void addSensor(Sensor * sensor, float sensor_frequency) 
            {
                _sensors[_sensor_count++] = sensor;
                sensor->change_frequency(sensor_frequency);
                _update_scheduler.add_sensor(sensor);
                _update_scheduler.set_task_inter_arrival_time(1 + _sensor_count, 1000000 / sensor_frequency);
                _update_scheduler.set_task_criticality(1 + _sensor_count, 2);
            }

            void addPidController(PidController *controller, uint8_t auxState = 0)
            {
                _pidTask.addController(controller, auxState);
            }

    }; // class HackflightPure

} // namespace hf
