#
# Makefile for Hackflight Copilot socket version
#
# Copyright (c) 2021 on D. Levy
#
# MIT License

HSRC = Main.hs \
	   Safety.hs \
	   Serial.hs \
       ../Receiver.hs \
       ../Hackflight.hs \
       ../Demands.hs \
       ../State.hs \
       ../Sensor.hs \
       ../Time.hs \
       ../Utils.hs \
       ../Mixer.hs \
       ../PidController.hs \
       ../pidcontrollers/RatePid.hs \
       ../pidcontrollers/YawPid.hs \
       ../pidcontrollers/AltHoldPid.hs \
       ../pidcontrollers/LevelPid.hs \
       ../pidcontrollers/PosHoldPid.hs \
       ../sensors/Gyrometer.hs \
       ../sensors/Altimeter.hs \
       ../sensors/Quaternion.hs \
       ../sensors/OpticalFlow.hs

RUNH = runhaskell -i.. -i../sensors -i../pidcontrollers Main.hs

DST = $(MYHOME)/Documents/Unreal\ Projects/MulticopterSim/Source/CopilotModule

all: copilot.cpp copilot.h
	cp copilot.c $(DST)/copilot.cpp
	sed -i 's/extern/EXTERN/g' copilot.h
	cat ../cfiles/header.h copilot.h > $(DST)/copilot.h

copilot.c: $(HSRC) Main.hs
	$(RUNH)

copilot.h: $(HSRC) Main.hs
	$(RUNH)

clean:
	rm -f copilot.* tmp.c *~

commit:
	git commit -a -m="More Haskell Copilot work"

copilot.cpp: copilot.c
	tail -n +8 copilot.c > tmp.c
	cat ../cfiles/header.c tmp.c > copilot.c
	rm tmp.c
