/*
 * Header-only code for dynamics of vehicles with fixed rotor pitch
 * (quadcopter, hexacopter, ocotocopter, ...)
 *
 * Copyright (C) 2021 Simon D. Levy
 *
 * MIT License
 */

#pragma once

#include "../Dynamics.hpp"


class FixedPitchDynamics : public Dynamics {

    public:

        /**
         *  Vehicle parameters
         */
        typedef struct {

            double b;  // thrust coefficient [F=b*w^2]
            double l;  // arm length [m]

        } fixed_pitch_params_t; 

    private:

        fixed_pitch_params_t _fparams;

        double start_time;

    protected:

        FixedPitchDynamics(uint8_t nmotors, Dynamics::vehicle_params_t &vparams, fixed_pitch_params_t &fparams)
            : Dynamics(nmotors, vparams)
        {
            memcpy(&_fparams, &fparams, sizeof(fixed_pitch_params_t));

            /**

            IConsoleManager::Get().RegisterConsoleVariable(
                TEXT("ForceApplied"),
                0,
                TEXT("Well this is just a test debug flag.\n")
                    TEXT("<=0: off \n") TEXT(">=1: enable debug something\n"),
                ECVF_Cheat);
                */

            start_time = FPlatformTime::Seconds();
        }


        virtual double getThrustCoefficient(double * actuators) override
        {
            // Thrust coefficient is constant for fixed-pitch rotors

            (void)actuators;
            
            return _fparams.b;
        }

        virtual void computeRollAndPitch(double * actuators, double * omegas2, double & roll, double & pitch) override
        {
            // We've already used actuators to compute omegas2
            (void)actuators;

            roll = 0;
            pitch = 0;

            static double last_update_time = FPlatformTime::Seconds();

            for (uint8_t i=0; i<_rotorCount; ++i) {
                roll += _fparams.l * _fparams.b * omegas2[i] * getRotorRollContribution(i);
                pitch += _fparams.l * _fparams.b * omegas2[i] * getRotorPitchContribution(i);
            }

            double current_time = FPlatformTime::Seconds();

            const double force_added = 20.0;

            //test a 1 N force applied in the roll direction clockwise.
            // if (current_time - start_time > 20.0 && current_time - start_time < 21.0) roll += _fparams.l * force_added;

            /* if (FPlatformTime::Seconds() - last_update_time > 1.0 / 250.0) {
                UE_LOG(LogTemp, Display, TEXT("roll force added ,%.6lf\n"), force_added);
                last_update_time = FPlatformTime::Seconds();
            } */
        }

        virtual int8_t getRotorRollContribution(uint8_t i)  = 0;
        virtual int8_t getRotorPitchContribution(uint8_t i)  = 0;
 
}; // class FixedPitchDynamics
