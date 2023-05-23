/*
   Abstract class for sensors

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "HF_state.hpp"
#include "HF_board.hpp"

namespace hf {

    class Sensor {

        protected:
            float FREQ;

            float _time = 0;

            float _period;

        public:

            virtual void modifyState(State * state, float time) = 0;

            virtual void begin(void) { }

            virtual void change_frequency(float new_frequency)
            {
                FREQ = new_frequency;
                _period = 1 / new_frequency;
            }

            virtual float current_frequency()
            {
                return FREQ;
            }
            
            virtual bool ready(Board* board)
            {
                float time = board->getTime();

                if ((time - _time) > _period) {
                    _time = time;
                    return true;
                }

                return false;
            }

    };  // class Sensor

} // namespace hf
