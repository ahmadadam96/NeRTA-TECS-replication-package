As this
[sketch](https://github.com/simondlevy/Hackflight/blob/master/examples/Ladybug/DSMX/DSMX.ino#L46-L62)
shows, it's simple to set the Rate PID and Level PID parameters for your a
model (airframe) like the [3D Fly](https://www.thingiverse.com/thing:2519301).
We've found [this video](https://www.youtube.com/watch?v=30Au6sEv6-o) helpful
for understanding the basics of PID tuning.

As as alternative or addition to using the trim tabs on our transmitters, we
find it useful to be able to trim-out the vehicle with a set of
parameters passed to the constructor for the 
[Receiver](https://github.com/simondlevy/Hackflight/blob/master/src/receiver.hpp)
class, which you can also
set to override their default of zero.

<hr>

<b>Next</b>: [Flashing the firmware](https://github.com/simondlevy/Hackflight/wiki/L.04-Flashing-the-firmware)
