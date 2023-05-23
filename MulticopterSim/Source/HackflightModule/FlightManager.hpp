/*
   MulticopterSim FlightManager class implementation using UDP sockets 

   Acts as a client for a server program running on another address

   Sends joystick demands and vehicle state to server; receives motor
   values

   Copyright(C) 2019 Simon D.Levy

   MIT License
*/

#include <algorithm>

#include "../MainModule/FlightManager.hpp"
#include "../MainModule/Dynamics.hpp"

#include <HF_pure.hpp>

// PID controllers
#include <hf_pidcontrollers/level.hpp>
#include <hf_pidcontrollers/rate.hpp>
#include <hf_pidcontrollers/yaw.hpp>
#include <hf_pidcontrollers/althold.hpp>
#include <hf_pidcontrollers/poshold.hpp>

#include "SimReceiver.hpp"
#include "SimBoard.hpp"
#include "SimMotor.hpp"

#include "sensors/SimGyrometer.hpp"
#include "sensors/SimQuaternion.hpp"
#include "sensors/SimAltimeter.hpp"
#include "sensors/SimOpticalFlow.hpp"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h" 

//#define SCRIPT_BUILD

class FHackflightFlightManager : public FFlightManager {

    private:

        // PID controllers
		hf::RatePid _ratePid = hf::RatePid(0.225, 0.001875, 0.375);
		hf::YawPid _yawPid = hf::YawPid(1.0625, 0.005625);
        hf::LevelPid _levelPid = hf::LevelPid(0.20);
        hf::AltitudeHoldPid _altHoldPid;
        hf::PositionHoldPid _posHoldPid;

        // Mixer
        hf::Mixer * _mixer = NULL;

        // "Board": implements getTime()
        SimBoard _board;

        // "Receiver": joystick/gamepad
        SimReceiver * _receiver = NULL;

        // "Sensors": directly from ground-truth
        SimGyrometer * _gyrometer = NULL;
        SimQuaternion * _quaternion = NULL;
        SimAltimeter * _altimeter = NULL;
        SimOpticalFlow * _opticflow = NULL;

        // "Motors": passed to mixer so it can modify them
        SimMotor * _motors[100] = {};

        // Main firmware
        hf::HackflightPure * _hackflight = NULL;
        
        // Guards socket comms
        bool _ready = false;

    public:

        double start_time;

        unsigned int stage;

        APawn * _pawn;


        FHackflightFlightManager(
                APawn * pawn,
                hf::Mixer * mixer,
                SimMotor ** motors,
                Dynamics * dynamics)
            : FFlightManager(dynamics)
        {
            // Store mixer, motors for later
            _mixer = mixer;
            for (uint8_t k=0; k<_actuatorCount; ++k) {
                _motors[k] = motors[k];
            }

            start_time = FPlatformTime::Seconds();

            stage = 0;

            _pawn = pawn;

            // Pass PlayerController to receiver constructor in case we have no
            // joystick / game-controller
            _receiver = new SimReceiver(pawn);

            // Create Hackflight object
            _hackflight = new hf::HackflightPure(&_board, _receiver, _mixer);

            // Create simulated sensors
            _gyrometer = new SimGyrometer(_dynamics);
            _quaternion = new SimQuaternion(_dynamics);
            _altimeter = new SimAltimeter(_dynamics);
            _opticflow = new SimOpticalFlow(_dynamics);

            // Add simulated sensors
            _hackflight->addSensor(_gyrometer, 330);
            _hackflight->addSensor(_quaternion, 330);
            _hackflight->addSensor(_altimeter, 100);
            _hackflight->addSensor(_opticflow, 50);
 
            // Add PID controllers for all aux switch positions.
            // Position hold goes first, so it can have access to roll and yaw
            // stick demands before other PID controllers modify them.
            _hackflight->addPidController(&_posHoldPid);
            _hackflight->addPidController(&_ratePid);
            _hackflight->addPidController(&_yawPid);
            _hackflight->addPidController(&_levelPid);
            _hackflight->addPidController(&_altHoldPid);
            stage = 0;

            _receiver->write(0.0, hf::DEMANDS_THROTTLE);
            _receiver->write(0.0, hf::DEMANDS_ROLL);
            _receiver->write(0.0, hf::DEMANDS_YAW);
            _receiver->write(0.0, hf::DEMANDS_PITCH);

            // Start Hackflight firmware, indicating already armed
            _hackflight->begin();

            _ready = true;
        }

        ~FHackflightFlightManager()
        {
        }
        virtual void getActuators(const double time, double * values) override
        {
            static float drone_speed_array[1250];

            // Avoid null-pointer exceptions at startup, freeze after control
            // program halts
            if (!_ready) {
                return;
            }

            // Poll the "receiver" (joystick or game controller)
            #ifndef SCRIPT_BUILD
            _receiver->poll();
            #endif

            // Update the Hackflight firmware, causing Hackflight's actuator
            // to set the values of the simulated motors
            _hackflight->update();

            static double previous_print = 0.0;

            static int drone_speed_array_index = 0;
            
            float demands[hf::Receiver::MAX_DEMANDS] = {};
            
            #ifdef SCRIPT_BUILD
            if (FPlatformTime::Seconds() - previous_print > 1.0 / 250.0) {
                //UE_LOG(LogTemp, Display, TEXT("pid frequency ,%f\n"), _hackflight->_pidTask.current_frequency());
                float inertial_euclidean_velocity = sqrtf(_dynamics->x(hf::State::DX) * _dynamics->x(hf::State::DX) + _dynamics->x(hf::State::DY) * _dynamics->x(hf::State::DY) + _dynamics->x(hf::State::DZ) * _dynamics->x(hf::State::DZ));
                float angular_euclidean_velocity = sqrtf(_dynamics->x(hf::State::DPHI) * _dynamics->x(hf::State::DPHI) + _dynamics->x(hf::State::DTHETA) * _dynamics->x(hf::State::DTHETA) + _dynamics->x(hf::State::DPSI) * _dynamics->x(hf::State::DPSI));

                UE_LOG(LogTemp, Display, TEXT("inertial euclidean velocity ,%f\n"), inertial_euclidean_velocity);

                drone_speed_array[drone_speed_array_index] = inertial_euclidean_velocity;
                drone_speed_array_index++;

                if (drone_speed_array_index == 1250) drone_speed_array_index = 0;

                //UE_LOG(LogTemp, Display, TEXT("inertial euclidean velocity ,%f\n"), inertial_euclidean_velocity);
                //UE_LOG(LogTemp, Display, TEXT("angular euclidean velocity ,%f\n"), angular_euclidean_velocity);
                
                if(-_dynamics->x(hf::State::Z) < 20 && stage==0) {
                    _receiver->write(1.0, hf::DEMANDS_THROTTLE);
                    _receiver->write(0.0, hf::DEMANDS_ROLL);
                    _receiver->write(0.0, hf::DEMANDS_YAW);
                    _receiver->write(0.0, hf::DEMANDS_PITCH);
                }
                else if (stage == 0) {
                    _receiver->write(0.0, hf::DEMANDS_THROTTLE);
                    _receiver->write(0.0, hf::DEMANDS_ROLL);
                    _receiver->write(0.0, hf::DEMANDS_YAW);
                    _receiver->write(0.0, hf::DEMANDS_PITCH);
                    stage = 1;
                    UE_LOG(LogTemp, Display, TEXT("time ,%.6lf\n"), FPlatformTime::Seconds() - start_time);
                    UE_LOG(LogTemp, Display, TEXT("stage ,%d\n"), stage);
                    UE_LOG(LogTemp, Display, TEXT("altitude ,%f\n"), -_dynamics->x(hf::State::Z));
                    UE_LOG(LogTemp, Display, TEXT("pid frequency ,%f\n"), _hackflight->_pidTask.current_frequency());
                }
                if (inertial_euclidean_velocity > 0.5 && stage == 1) {
                    _receiver->write(0.0, hf::DEMANDS_THROTTLE);
                    _receiver->write(0.0, hf::DEMANDS_YAW);
                    _receiver->write(0.0, hf::DEMANDS_PITCH);
                    _receiver->write(0.0, hf::DEMANDS_ROLL);
                }
                else if (stage == 1){
                    stage = 2;
                    UE_LOG(LogTemp, Display, TEXT("time ,%.6lf\n"), FPlatformTime::Seconds() - start_time);
                    UE_LOG(LogTemp, Display, TEXT("stage ,%d\n"), stage);
                    UE_LOG(LogTemp, Display, TEXT("inertial euclidean velocity ,%f\n"), inertial_euclidean_velocity);
                    UE_LOG(LogTemp, Display, TEXT("pid frequency ,%f\n"), _hackflight->_pidTask.current_frequency());
                }
                if (inertial_euclidean_velocity < 16.0 && stage == 2) {
                    _receiver->write(0.45, hf::DEMANDS_ROLL);
                    _receiver->write(0.0, hf::DEMANDS_THROTTLE);
                    _receiver->write(0.0, hf::DEMANDS_YAW);
                    _receiver->write(0.0, hf::DEMANDS_PITCH);
                }
                else if (stage == 2) {
                    _receiver->write(0.0, hf::DEMANDS_ROLL);
                    _receiver->write(0.0, hf::DEMANDS_THROTTLE);
                    _receiver->write(0.0, hf::DEMANDS_YAW);
                    _receiver->write(0.0, hf::DEMANDS_PITCH);
                    stage = 3;
                    UE_LOG(LogTemp, Display, TEXT("time ,%.6lf\n"), FPlatformTime::Seconds() - start_time);
                    UE_LOG(LogTemp, Display, TEXT("stage ,%d\n"), stage);
                    UE_LOG(LogTemp, Display, TEXT("inertial euclidean velocity ,%f\n"), inertial_euclidean_velocity);
                    UE_LOG(LogTemp, Display, TEXT("pid frequency ,%f\n"), _hackflight->_pidTask.current_frequency());
                }
                int n = sizeof(drone_speed_array) / sizeof(drone_speed_array[0]);
                if (stage == 3 && *std::max_element(drone_speed_array, drone_speed_array+n) > 0.5) {
                    _receiver->write(0.0, hf::DEMANDS_THROTTLE);
                    _receiver->write(0.0, hf::DEMANDS_YAW);
                    _receiver->write(0.0, hf::DEMANDS_PITCH);
                    _receiver->write(0.0, hf::DEMANDS_ROLL);
                } else if (stage == 3) {
                    stage = 4;
                    UE_LOG(LogTemp, Display, TEXT("time ,%.6lf\n"), FPlatformTime::Seconds() - start_time - 5.0);
                    UE_LOG(LogTemp, Display, TEXT("stage ,%d\n"), stage);
                    UE_LOG(LogTemp, Display, TEXT("inertial euclidean velocity ,%f\n"), inertial_euclidean_velocity);
                    UE_LOG(LogTemp, Display, TEXT("pid frequency ,%f\n"), _hackflight->_pidTask.current_frequency());

                    //UGameplayStatics::OpenLevel(_pawn, FName(*_pawn->GetWorld()->GetName()), true);

                    //FString RestartLevel = "RestartLevel";
                    //_pawn->GetWorld()->Exec(_pawn->GetWorld(), *RestartLevel);
                    
                    //GetOwningPlayer()->ConsoleCommand("RestartLevel");
                    UE_LOG(LogTemp, Display, TEXT("restart\n"));
                    UGameplayStatics::GetPlayerController(_pawn, 0)->ConsoleCommand("RestartLevel");

                    //_pawn->Reset();
                    //_pawn->Restart();
                }
                
                // UE_LOG(LogTemp, Display, TEXT("demand roll ,%f\n"), _receiver->get_demand(hf::DEMANDS_ROLL));
                // UE_LOG(LogTemp, Display, TEXT("time ,%.6lf\n"), FPlatformTime::Seconds() - start_time);

                // UE_LOG(LogTemp, Display, TEXT("altitude ,%f\n"), -_dynamics->x(hf::State::Z));
                /*
                for(int i = 0; i<_sensor_count; i++){
                    UE_LOG(LogTemp, Display, TEXT("sensor frequencies ,%f\n"), _sensors[i]->current_frequency());
                }*/

                previous_print = FPlatformTime::Seconds();
            }
            #endif

            // Set the time in the simulated board, so it can be retrieved by
            // Hackflight
            _board.set(time);

            //  Get the new motor values
            for (uint8_t i=0; i < _actuatorCount; ++i) {
                values[i] = _motors[i]->getValue();
            }
            //debugline("m1: %3.3f  m2: %3.3f  m3: %3.3f  m4: %3.3f",   
            //        values[0], values[1], values[2], values[3]);
        }

        void tick(void)
        {
        }

}; // FHackflightFlightManager
