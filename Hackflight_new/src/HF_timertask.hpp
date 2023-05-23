/*
   Abstract class for timer tasks

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "HF_board.hpp"

namespace hf {

    class TimerTask {

        private:

            float _period = 0;
            float _time = 0;

        protected:

            TimerTask(float freq)
            {
                _period = 1 / freq;
                _time = 0;
            }

        public:
            bool enabled = true;

            bool ready(Board * board)
            {
                float time = board->getTime();

                if (enabled && (time - _time) > _period)
                {
                    _time = time;
                    return true;
                }

                return false;
             }

             virtual void change_frequency(float freq)
             {
                 _period = 1 / freq;
             }

             virtual float current_frequency()
             {
                 return 1 / _period;
             }

    };  // TimerTask

} // namespace hf
