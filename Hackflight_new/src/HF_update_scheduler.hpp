/*
   Timer task for serial comms

   Copyright (c) 2020 Simon D. Levy

   This file is part of Hackflight.

   Hackflight is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Hackflight is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with Hackflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "HF_loggingfunctions.hpp"
#include <vector>
#include <limits.h>
#include "HF_sensor.hpp"
#include "HF_pidtask.hpp"
#include "HF_state.hpp"
#include "HF_board.hpp"
#include "HF_receiver.hpp"
#include "float.h"

namespace hf
{
    class UpdateScheduler
    {
        friend class Hackflight;

        // criticality = 0 - undefined, 1 - LOW, 2 - HIGH

        struct task_info {
            unsigned int time_task_started = 0;
            unsigned int inter_arrival_time = 0;
            unsigned int next_release_time = 0;
            unsigned int criticality = 0;
        };
        unsigned int update_time_required;
        unsigned int number_of_tasks = 0;
        unsigned int number_of_sensors = 0;
        #define receiver_task_wcet 122
        #define receiver_task_frequency 300f

        // reactive control states
        // divide original frequencies
        float _reactive_control_state = 1.0;
        #define reactive_control_inertial_threshold_1 1.0f
        #define reactive_control_inertial_threshold_2 16.0f

        //#define reactive_control_angular_threshold_1 1
        //#define reactive_control_angular_threshold_2 1.5

        #define reactive_control_high_freq 1.0f
        #define reactive_control_med_freq 1.5f
        #define reactive_control_low_freq 3.0f

        bool reactive_control_running = false;

        bool update_scheduled = true;

        PidTask *_pidTask;
        Receiver *_receiver_task;
        Board *_board = NULL;

        float *_state;

       public:

        std::vector<task_info> task_infos;
        std::vector<Sensor *> sensors;
        std::vector<float> sensor_default_frequencies;

        // task id 0 receiver task
        #define receiver_task_id 0
        // task id 1 PID task
        #define pid_task_id 1
        // task ids 2+ sensor tasks

        void init(PidTask* pid_task, Receiver* receiver_task, float* state, Board* board){
            number_of_tasks = 2;
            for (unsigned int i = 0; i < number_of_tasks; i++) {
                task_infos.push_back(task_info());
            }
            _pidTask = pid_task;
            _receiver_task = receiver_task;
            _state = state;
            _board = board;
            // use this for reactive control training
            //reactive_control_change_state(1);
        }

        bool no_update(void){
            return update_scheduled;
        }

        void add_sensor(Sensor *sensor){
            sensors.push_back(sensor);
            sensor_default_frequencies.push_back(sensor->current_frequency());
            number_of_sensors++;
            task_infos.push_back(task_info());
            number_of_tasks++;
        }

        void set_task_inter_arrival_time(unsigned int task_id, unsigned int inter_arrival_time)
        {
            task_infos[task_id].inter_arrival_time = inter_arrival_time;
        }

        void set_task_criticality(int task_id, unsigned int criticality)
        {
            task_infos[task_id].criticality = criticality;
        }

        unsigned int task_started_start_time;
        unsigned int task_started_end_time;
        unsigned int task_ended_start_time;
        unsigned int task_ended_end_time;

        void task_started(unsigned int task_id)
        {
            task_started_start_time =  _board->getTimeUS();
            update_next_release_time(task_id, true);
            task_started_end_time = _board->getTimeUS();
            //print_string("ITU,%d\n", task_started_end_time - task_started_start_time);
        }
        
        void task_ended(unsigned int task_id){
            //unsigned int current_time = _board->getTimeUS();
            //print_string("Nmn,%d\n", normal_min_invocation_time()-current_time);
            if (!update_scheduled) {
                update_scheduled = when_schedule_update();
            }
            else execute_low_criticality_tasks(_board->getTimeUS());

            // Used for measuring overhead of switching reactive control state
            /* static unsigned int reactive = 0;
            switch (reactive)
            {
            case 0:
                reactive_control_change_state(reactive_control_high_freq);
                break;
            case 1:
                reactive_control_change_state(reactive_control_med_freq);
                break;
            case 2:
                reactive_control_change_state(reactive_control_low_freq);
                break;
            default:
                break;
            }
            reactive++;
            if(reactive > 2) reactive = 0; */
        }

        void initialize_scheduling(unsigned int input_update_time_required){
            update_time_required = input_update_time_required;
            update_scheduled = false;
        }

        bool when_schedule_update()
        {
            if(reactive_control_running) reactive_control_loop();

            unsigned int min_value = normal_min_invocation_time();
            unsigned int current_time = _board->getTimeUS();
            if (min_value > current_time && min_value - current_time > update_time_required)
            {
                schedule_update(current_time, false);
                unsigned int time_after_update = _board->getTimeUS();
                print_string("MITU,%d\n", time_after_update - current_time);
                return true;
            }
            
            min_value = high_criticality_min_invocation_time();
            current_time = _board->getTimeUS();
            if (min_value > current_time && min_value - current_time > update_time_required) {
                schedule_update(current_time, true);
                unsigned int time_after_update = _board->getTimeUS();
                print_string("MITUM,%d\n", time_after_update - current_time);
                return true;
            }

            //reactive_control_running = true;

            // Update scheduling failed\n
            print_string("Usf\n");
            return false;
        }

        void schedule_update(unsigned int current_time, bool mixed_criticality){
            // perform update here
            //delayMicroseconds(update_time_required);
            unsigned int time_after_update = _board->getTimeUS();
            update_scheduled = true;
            reactive_control_running = false;
            reactive_control_change_state(reactive_control_high_freq);

            // Mitigation system for released low-criticality tasks when update scheduled only with 
            // high-criticality tasks
            if (mixed_criticality && time_after_update >= task_infos[receiver_task_id].next_release_time) {
                _receiver_task->enabled = false;
            }

            // Update of size,%d,performed at time,%d\n
            print_string("Us,%d,p,%d\n", update_time_required, time_after_update);
        }

        void execute_low_criticality_tasks(unsigned int current_time){
            // Mitigation system for released low-criticality tasks
            // Executes and reenables low-criticality tasks only if idle time is sufficient
            //unsigned int time_1;
            if (!_receiver_task->enabled) {
                //unsigned int time_0 = _board->getTimeUS();
                unsigned int min_value = high_criticality_min_invocation_time();
                if (min_value > current_time && min_value-current_time > receiver_task_wcet){
                    //time_1 = _board->getTimeUS();
                    // mitigation time task enabled
                    //print_string("Mte,%d\n", time_1 - time_0);
                    task_started(receiver_task_id);
                    _receiver_task->update();
                    _receiver_task->enabled = true;
                    task_ended(receiver_task_id);
                    return;
                }
                _receiver_task->enabled = false;
                //time_1 = _board->getTimeUS();
                // mitigation time task not enabled
                //print_string("Mtne,%d\n", time_1-time_0);
            }
        }

        unsigned int normal_min_invocation_time(){
            unsigned int min_value = UINT_MAX;
            for (unsigned int i = 0; i < number_of_tasks; i++) {
                // Task index,%d,Next invocation time,%d\n
                //print_string("Ti,%d,Nit,%d\n", i, task_infos[i].next_release_time);
                if (task_infos[i].next_release_time < min_value) {
                    min_value = task_infos[i].next_release_time;
                }
            }
            return min_value;
        }

        unsigned int high_criticality_min_invocation_time(){
            unsigned int min_value = UINT_MAX;
            for (unsigned int i = 0; i < number_of_tasks; i++) {
                if(task_infos[i].criticality == 2){
                    // Task index,%d,Next invocation time,%d\n
                    //print_string("Ti,%d,Nit,%d\n", i, task_infos[i].next_release_time);
                    if (task_infos[i].next_release_time < min_value) {
                        min_value = task_infos[i].next_release_time;
                    }
                }
            }
            return min_value;
        }

        void update_next_release_time(unsigned int task_id, bool change_time_task_started){
            if(change_time_task_started) task_infos[task_id].time_task_started = _board->getTimeUS();
            task_infos[task_id].next_release_time = task_infos[task_id].inter_arrival_time + task_infos[task_id].time_task_started;
        }

        void reactive_control_loop(){
            //unsigned int time_1 = _board->getTimeUS();
            float inertial_euclidean_velocity = sqrtf(_state[State::DX] * _state[State::DX] + _state[State::DY] * _state[State::DY] + _state[State::DZ] * _state[State::DZ]);
            //float angular_euclidean_velocity = sqrtf(_state[State::DPHI] * _state[State::DPHI] + _state[State::DTHETA] * _state[State::DTHETA] + _state[State::DPSI] * _state[State::DPSI]);

            //float inertial_euclidean_velocity = 0.0;

            // if reactive control state is high frequency, we want it to change to medium not low for safety.
            if (_reactive_control_state != reactive_control_high_freq && (inertial_euclidean_velocity < reactive_control_inertial_threshold_1)){
                reactive_control_change_state(reactive_control_low_freq);
            }
            else if (inertial_euclidean_velocity < reactive_control_inertial_threshold_2){
                reactive_control_change_state(reactive_control_med_freq);
            }
            else reactive_control_change_state(reactive_control_high_freq);
            //unsigned int time_2 = _board->getTimeUS();
            //print_string("RC,%d\n", time_2-time_1);
        }

        bool reactive_control_change_state(float new_reactive_control_state){
            if (new_reactive_control_state == _reactive_control_state) return false;

            //unsigned int time_0 = _board->getTimeUS();

            float old_frequency_pid = _pidTask->current_frequency();
            float new_frequency_pid = old_frequency_pid * _reactive_control_state / new_reactive_control_state;
            _pidTask->change_frequency(new_frequency_pid);
            task_infos[pid_task_id].inter_arrival_time = 1000000 / new_frequency_pid;
            update_next_release_time(pid_task_id, false);

            for (unsigned int i = 0; i < number_of_sensors; i++) {
                float old_frequency_sensor = sensors[i]->current_frequency();
                float new_frequency_sensor;

                if(new_reactive_control_state < _reactive_control_state){
                    new_frequency_sensor = std::min(sensor_default_frequencies[i], new_frequency_pid);
                }
                else new_frequency_sensor = std::min(old_frequency_sensor, new_frequency_pid);
                sensors[i]->change_frequency(new_frequency_sensor);
                task_infos[i + 2].inter_arrival_time = 1000000 / new_frequency_sensor;
                update_next_release_time(i + 2, false);
            }

            _reactive_control_state = new_reactive_control_state;
            //print_string("RCc,%d\n", _board->getTimeUS()-time_0);

            return true;
        }
    }; 


}  // namespace hf
