<img src="images/receivers2.png" width=500>

Through the virtual methods in its
[Receiver](https://github.com/simondlevy/Hackflight/blob/master/src/receiver.hpp#L50-L56)
class, Hackflight allows you to use different kinds of R/C
receivers (and potentially other kinds of wireless controllers).  Currently, we
are supporting the protocols used in three popular micro-copter receivers, for
which you should now clone the appropriate repository into your Arduino
libraries folder:

1. DSMX serial receiver protocol used in the [Lemon RX DSMX Satellite
Receiver](https://www.amazon.com/gp/product/B00SUM3H6O/ref=oh_aui_search_detailpage?ie=UTF8&psc=1):
use [SpektrumDSM](https://github.com/simondlevy/SpektrumDSM)

2. SBUS serial receiver protocol used in the 
[FrSky XM SBUS Micro Receiver](https://www.getfpv.com/frsky-xm-receiver.html?gclid=CjwKCAjwj8bPBRBiEiwASlFLFfyMip17lmdiJT4BskYb8G7f4PpzJ2PTm1pnQ4K3G6fUhPdPV75TuxoCtq4QAvD_BwE):
use Bolderflight's [SBUS](https://github.com/bolderflight/SBUS) library.

Later in this wiki we will show you how to wire up one of these receivers to your LadybugFC.  
The point of cloning the repository now is to enable you to flash the Hackflight firmware in the next step.

<hr>

<b>Next</b>: [Setting PID parameters](https://github.com/simondlevy/Hackflight/wiki/L.03-Setting-PID-parameters)
