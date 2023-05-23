/*
   Timer task for closed-loop controllers

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "HF_timertask.hpp"
#include "HF_state.hpp"
#include "HF_receiver.hpp"
#include "HF_mixer.hpp"
#include "HF_pidcontroller.hpp"

namespace hf {

    class PidTask : public TimerTask {

        private:

            // PID controllers
            PidController * _controllers[256] = {};
            uint8_t _controller_count = 0;

        public:
            // For now, we keep all PID timer tasks the same.  At some point it might be useful to 
            // investigate, e.g., faster updates for Rate PID than for Level PID.
            float FREQ = 300;
            static constexpr unsigned int task_id = 1;
            static constexpr unsigned int criticality = 2;

            // For now, we keep all tasks the same.  At some point it might be
            // useful to investigate, e.g., faster updates for Rate PID than
            // for Level PID.
            PidTask(void)
                : TimerTask(FREQ)
            {
                _controller_count = 0;
                change_frequency(FREQ);
            }

            void addController(PidController* controller, uint8_t auxState)
            {
                controller->auxState = auxState;

                _controllers[_controller_count++] = controller;
            }

            bool update(Board * board, Receiver * receiver, Mixer * mixer, State * state)
            {
                // Start with demands from open-loop controller
                float demands[Receiver::MAX_DEMANDS] = {};
                receiver->getDemands(demands);

                uint8_t auxState = receiver->getModeIndex();

                // Apply PID controllers to demands
                for (uint8_t k=0; k<_controller_count; ++k) {
                    if (_controllers[k]->auxState <= auxState) {
                    _controllers[k]->modifyDemands(state, demands);
                    }
                }

                // Use updated demands to run motors, allowing
                // mixer to choose whether it cares about
                // open-loop controller being inactive (e.g.,
                // throttle down)
                if (!state->failsafe) {
                    mixer->run(demands, state->armed && !receiver->inactive());
                }

                return true;

             } // doTask

    };  // PidTask

} // namespace hf
