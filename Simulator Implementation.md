## Simulator Implementation

The simulator contains print functions that print key information to the terminal. Here is an example `UE_LOG(LogTemp, Display, TEXT("roll force added ,%.6lf\n"), force_added);`.

Our additions to the simulator are in two key places. First is to the file Source/MainModule/FixedPitch.hpp, which models the dynamics of vehicles with fixed rotor pitch. In this file we added the following code (currently commented out) to add a roll force to the vehicle for one second. We used this in our experiments for the impact of BRC on the dependability.

``````cpp
//test a 1 N force applied in the roll direction clockwise.
if (current_time - start_time > 20.0 && current_time - start_time < 21.0) roll += _fparams.l * force_added;

if (FPlatformTime::Seconds() - last_update_time > 1.0 / 250.0) {
    UE_LOG(LogTemp, Display, TEXT("roll force added ,%.6lf\n"), force_added);
    last_update_time = FPlatformTime::Seconds();
}
``````

The second addition is in Source/HackflightModule/FlightManager.hpp

We implemented a scripted flight sequence in the `getActuators` function. This flight sequence can be enabled by uncommenting the lines `#define SCRIPT_BUILD` in Source/HackflightModule/FlightManager.hpp in the MulticopterSim folder, and in src/HF_pure.hpp in the Hackflight_new folder. The flight sequence enables us to automate the BRC training procedure. The flight sequence is as follows:

1. Take off to a height of 20.
2. Wait until the drone's euclidean velocity is below 0.5 m/s.
3. Apply thrust in the roll direction until the euclidean velocity reaches a specific value.
4. Wait until the last 1250 measurements of euclidean velocity are within 0.5 m/s.
5. Restart from step 1.

This script enables us to collect many measurements automatically. Also, variations in the host computer's performance and current load introduce small variations between different runs.

This script is can also be used for the BRC dependability experiments by replacing step 3 with waiting for the roll force to be applied. Step 4 is followed after the roll force is applied.

NOTE: you have recompile the simulator with Visual Studio after making changes to the code, then reopening Unreal Engine.
