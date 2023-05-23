#pragma once

#include <hf_motors/rotary.hpp>

namespace hf
{
    class TestbedMotor
    {
    protected:
        float _value = 0;

    public:
        float getValue(void)
        {
            return _value;
        }

    };  // class SimMotor

    class TestbedRotaryMotor : public TestbedMotor, public hf::RotaryMotor
    {
    protected:
        virtual void write(float value) override
        {
            _value = value;
        }

    public:
        TestbedRotaryMotor(void)
            : RotaryMotor(0)  // dummy number for pin
        {
        }

    };  // class SimRotaryMotor
}