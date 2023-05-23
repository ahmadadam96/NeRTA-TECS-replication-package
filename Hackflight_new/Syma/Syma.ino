/*
   Hackflight sketch for Ladybug Flight Controller with Spektrum DSMX receiver

   Additional libraries needed:

       https://github.com/simondlevy/EM7180
       https://github.com/simondlevy/CrossPlatformDataBus
       https://github.com/simondlevy/SpektrumDSM 

   Hardware support for Ladybug flight controller:

       https://github.com/simondlevy/grumpyoldpizza

   Copyright (C) 2021 Simon D. Levy

   MIT License

 */

#include "HF_full.hpp"
#include "hf_boards/realboards/arduino_serial/arduino/ladybugfc.hpp"
#include "hf_receivers/arduino/cppm.hpp"
#include "hf_mixers/quad/xmw.hpp"
#include "hf_pidcontrollers/rate.hpp"
#include "hf_pidcontrollers/yaw.hpp"
#include "hf_pidcontrollers/level.hpp"
#include "hf_pidcontrollers/althold.hpp"
#include "hf_sensors/usfs.hpp"

#include "hf_motors/arduino/brushed.hpp"

// Receiver ===================================================================

static constexpr uint8_t CHANNEL_MAP[6] = {2, 3, 1, 0, 4, 5};
static constexpr float DEMAND_SCALE = 4.0f;
static constexpr float SOFTWARE_TRIM[3] = {0, 0.05, 0.035};

static hf::CPPM_Receiver receiver = 
    hf::CPPM_Receiver(0, CHANNEL_MAP, DEMAND_SCALE);  

// Board =======================================================================

// Bluetooth comms over Serial2
static hf::LadybugFC board = hf::LadybugFC();  

// Motors  =====================================================================

static hf::ArduinoBrushedMotor motor1 = hf::ArduinoBrushedMotor(13);
static hf::ArduinoBrushedMotor motor2 = hf::ArduinoBrushedMotor(A2);
static hf::ArduinoBrushedMotor motor3 = hf::ArduinoBrushedMotor(3);
static hf::ArduinoBrushedMotor motor4 = hf::ArduinoBrushedMotor(11);

// Mixer =======================================================================

static hf::MixerQuadXMW mixer = hf::MixerQuadXMW(&motor1, &motor2, &motor3, &motor4);

// PID controllers =============================================================

static hf::RatePid ratePid = hf::RatePid(0.225, 0.001875, 0.375);
static hf::YawPid yawPid = hf::YawPid(1.0625, 0.005625f);
static hf::LevelPid levelPid = hf::LevelPid(0.20f);

// Sensors =====================================================================

static hf::USFS imu;

// Hackflight object ===========================================================

static hf::HackflightFull h(&board, &receiver, &mixer);

// Setup =======================================================================

void setup(void)
{
    // Add sensors
    h.addSensor(&imu, 330.0);

    // Add PID controllers
    h.addPidController(&levelPid);
    h.addPidController(&ratePid);
    h.addPidController(&yawPid);

    // Start Hackflight firmware
    h.begin();
}

// Loop ======================================================================

void loop(void)
{
    // Update Hackflight firmware
    h.update();
}
