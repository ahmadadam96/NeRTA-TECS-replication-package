## NeRTA implementation

The majority of the implementation of NeRTA is included in the src/HF_update_scheduler.hpp file. Other files include function calls to functions from the UpdateScheduler.

NeRTA has been implemented for a three task system. The tasks are as follows:

1. Receiver task (TASK ID 0)
2. PID task (TASK ID 1)
3. IMU Sensor task (TASK ID 2)

The task information is kept in a vector called `task_infos`, which is a vector of the task_info struct. The task_info struct contains the following key variables:

1. The time the task last started.
2. The task's inter-arrival time.
3. The time next release time.
4. The task's criticality (0 for undefined, 1 for LOW, and 2 for HIGH).

The code contains many debug statements that print out key information. These statements are called using the `print_string` function, which prints to a buffer, until the buffer is full after which the contents of the buffer are sent through serial.

The optimizations can be switched on or off by going into `when_schedule_update` function. To switch off the reactive control optimization, comment the line `reactive_control_running = true`. To switch off the mixed criticality optimization, comment out the following block

``````cpp
min_value = high_criticality_min_invocation_time();
current_time = _board->getTimeUS();
if (min_value > current_time && min_value - current_time > update_time_required) {
    schedule_update(current_time, true);
    //unsigned int time_2 = _board->getTimeUS();
    //print_string("MITUM,%d\n", time_2 - time_1);
    return true;
}
``````
