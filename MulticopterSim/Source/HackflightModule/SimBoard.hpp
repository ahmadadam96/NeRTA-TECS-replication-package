/*
   Hackflight Board class implementation for MulticopterSim

   Implements the core Board functionality getTime()

   Copyright(C) 2019 Simon D.Levy

   MIT License
*/

#pragma once

#include <HF_board.hpp>

class SimBoard : public hf::Board {

    private:
   
        float _currentTime = 0; // must be float for Hackflight

    protected:

        float getTime(void)
        {
            return _currentTime;
        }
        
        unsigned int getTimeUS(void){
            return _currentTime*1000000;
        }

    public:

        SimBoard() 
        {
        }

        virtual ~SimBoard() 
		{ 
		}

        void set(const float time)
        {
            _currentTime = time;
        }

}; // class Simboard

