
#pragma once

#include <HF_filters.hpp>
#include <HF_sensor.hpp>
#include <HF_state.hpp>

namespace hf{
    class TestbedSensor : public hf::Sensor {
        friend class Hackflight;

        protected:
        
        unsigned int _port;

        virtual void modifyState(hf::State *state, float time) override
        {
            (void)time;

            hf::State *hfstate = (hf::State *)state;

            // Replace these with data obtained through communication with testbed
            // hfstate->x[hf::State::PHI] = _dynamics->x(hf::State::PHI);
            // hfstate->x[hf::State::THETA] = _dynamics->x(hf::State::THETA);
            // hfstate->x[hf::State::PSI] = _dynamics->x(hf::State::PSI);

            // hfstate->x[hf::State::DPHI] = _dynamics->x(hf::State::DPHI);
            // hfstate->x[hf::State::DTHETA] = _dynamics->x(hf::State::DTHETA);
            // hfstate->x[hf::State::DPSI] = _dynamics->x(hf::State::DPSI);

            // hfstate->x[hf::State::Z] = -_dynamics->x(hf::State::Z);
            // hfstate->x[hf::State::DZ] = -_dynamics->x(hf::State::DZ);

            // hfstate->x[hf::State::DX] = _dynamics->x(hf::State::DX);
            // hfstate->x[hf::State::DY] = _dynamics->x(hf::State::DY);
        }

    public:
        TestbedSensor(unsigned int port = 0)
        {
            _port = port;
        }

    }; 
}