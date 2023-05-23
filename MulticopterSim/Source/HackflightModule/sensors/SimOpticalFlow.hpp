/*
   Simulate gyrometer using vehicle kinematics

   Copyright(C) 2019 Simon D.Levy

   MIT License
*/

#pragma once

#include "../MainModule/Dynamics.hpp"

#include <HF_state.hpp>

#include <HF_sensor.hpp>
#include <HF_filters.hpp>

class SimOpticalFlow : public hf::Sensor {

    protected:

        // We do all dynamcics => state conversion; subclasses just return
        // sensor values
        Dynamics * _dynamics = NULL;

        virtual void modifyState(hf::State * state, float time) override
        {
            (void)time;

            hf::State * hfstate = (hf::State *)state;

            hfstate->x[hf::State::DX] = _dynamics->x(hf::State::DX); 
            hfstate->x[hf::State::DY] = _dynamics->x(hf::State::DY); 
        }

    public:

        SimOpticalFlow(Dynamics * dynamics)
        {
            _dynamics = dynamics;
        }

}; // class SimOpticalFlow
