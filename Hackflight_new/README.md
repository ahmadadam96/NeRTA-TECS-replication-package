## Intro

Hackflight is a simple, platform-independent, header-only C++ toolkit for building multirotor flight controllers.  It is geared toward people like me who want to tinker with flight-control firmware, and use it to teach students about ideas like inertial measurement and PID tuning.  <b>If you are in the 99% percent of users who just want to get your vehicle flying without getting into firmware hacking, I recommend [Betaflight](http://betaflight.com/)</b> (great for getting started when you're on a budget) <b>or the [Ardupilot](http://copter.ardupilot.org) system</b> (for sophisticated mission planning with waypoint navigation and the like).  In addition to big user communities and loads of great features, these platforms have safety mechanisms that Hackflight lacks, which will help avoid injury to you and damage to your vehicle.

## Supported platforms

Hackflight is currently working on the following platforms:

* [Ladybug Flight Controller](https://www.tindie.com/products/TleraCorp/ladybug-flight-controller/) from Tlera Corp

* [MulticopterSim](https://github.com/simondlevy/MulticopterSim) flight simulator based on UnrealEngine4

## Standard units

By supporting floating-point operations, these platforms allow us to write simpler code based on standard units:

* Distances in meters
* Time in seconds
* Quaternions in the interval [-1,+1]
* Euler angles in radians
* Accelerometer values in Gs
* Barometric pressure in Pascals
* Stick demands in the interval [-1,+1]
* Motor demands in [0,1]

## Design principles

Thanks to some help from [Sytelus](https://github.com/sytelus), the core Hackflight [firmware](https://github.com/simondlevy/hackflight/tree/master/src) adheres to standard practices for C++, notably, short, simple methods and minimal use of compiler macros like <b>#ifdef</b> that can make it difficult to follow what the code is doing.  

A typical Arduino sketch for hackflight is written as follows:

1. Construct a ```Hackflight``` objecting using a ```Board```, ```Receiver```, and ```Mixer``` object.

2. Add sensors (```Gyrometer```, ```Quaternion```)

3. Add PID controllers (```Rate```, ```Level```)

4. In the ```loop()``` function, call ```Hackflight::update()```

## Core C++ Classes
* The <a href="https://github.com/simondlevy/Hackflight/blob/master/src/HF_board.hpp">Board</a> class specifies an abstract (pure virtual) <tt>getTime()</tt> method that you must implement for a particular microcontroller or simulator.  

* The <a href="https://github.com/simondlevy/Hackflight/blob/master/src/HF_receiver.hpp">Receiver</a> class performs basic functions associated with R/C receivers, and specifies a set of abstract methods that you implement for a particular receiver (DSMX, SBUS, etc.).

* The <a href="https://github.com/simondlevy/Hackflight/blob/master/src/HF_mixer.hpp">Mixer</a> class is an abstract class that can be subclassed for various kinds of mixers; for example, a quadcopter mixer using the MultiWii motor layout.

* The <a href="https://github.com/simondlevy/Hackflight/blob/master/src/HF_pidcontroller.hpp">PidController</a> class specifies an abstract method <tt>modifyDemands()</tt> that inputs the vehicles's current state and outputs an array of floating-point values representing how that controller affects the demands. (For example, an altitude-hold controller for a quadcopter would use the 'copter's altitude and vertical velocity to adjust the throttle demand.)  If you're mathematically-minded, you can think of a PID controller as a function from a (<i>State</i>, <i>Demands</i>) pair to <i>Demands</i>: <b><i>PidController</i>: <i>State</i> &times; <i>Demands</i> &rarr; <i>Demands</i></b>

* The <a href="https://github.com/simondlevy/Hackflight/blob/master/src/HF_sensor.hpp">Sensor</a> class specifies abstract methods <tt>ready()</tt> for checking whether the sensor
has new data avaiable, and  <tt>modifyState()</tt> for modifying the vehicle's state based on that data.  If you're mathematically-minded, you can think of a sensor as a function from states to states: <b><i>Sensor</i>: <i>State</i> &rarr; <i>State</i></b>

Together, these classes interact as shown in the following diagram:

<p align="center"> 
<img src="extras/media/dataflow2.png" width=700>
</p>

## Ground Control Station

Because it is useful to get some visual feedback on things like vehicle orientation and RC receiver channel values,  we also provide a very simple &ldquo;Ground Control Station&rdquo; (GCS) program that allows you to connect to the board and see what's going on. Windows users can run this program directly: just clone the [HackflightGCS](https://github.com/simondlevy/HackflightGCS) repository and double-click on <b>hackflight.exe</b>.  Others can run the <b >hackflight.py</b> Python script in the <b>extras/gcs/python</b> folder.  To run the Python script you'll need to install [MSPPG](https://github.com/simondlevy/Hackflight/tree/master/extras/parser), a parser generator for the Multiwii Serial Protocol (MSP) messages used by the firmware. Follow the directions in that folder to install MSPPG for Python.

## PID Controllers

<b>A PID controller is not the same as a [flight mode](https://oscarliang.com/rate-acro-horizon-flight-mode-level/).</b> For example, so-called [Acro mode](http://ardupilot.org/copter/docs/acro-mode.html#acro-mode) requires a PID controller based on angular velocity (a.k.a. rate, computed from the gyrometer) for each of the three angles (roll, pitch yaw). So-called [Stabilize](http://ardupilot.org/copter/docs/stabilize-mode.html#stabilize-mode) mode requires these three angular-velocity controllers, plus a PID controller based on angle (computed from the quaternion) for the roll and pitch axes. To support this arrangement in Hackflight, PID controllers for aux state 0 will also run in aux states 1 and 2, and PID controllers for aux state 1 will also run in aux state 2.

<p align="center"> <img src="extras/media/pidcontrollers.png" width=600> </p>
