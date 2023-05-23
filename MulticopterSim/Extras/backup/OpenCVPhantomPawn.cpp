/*
* Class implementation for Phantom pawn in MulticopterSim
*
* Copyright (C) 2018 Simon D. Levy
*
* MIT License
*/

#include "HackflightModule/pawns/OpenCVPhantomPawn.h"

AOpenCVPhantomPawn::AOpenCVPhantomPawn()
{
    _phantom.build(this);

    _phantom.addCamera(&_camera);
}

void AOpenCVPhantomPawn::PostInitializeComponents()
{
    _phantom.PostInitializeComponents();

    Super::PostInitializeComponents();
}

// Called when the game starts or when spawned
void AOpenCVPhantomPawn::BeginPlay()
{
    _flightManager = new FHackflightFlightManager(
           /* this,
            &_mixer.mixer,
            _mixer.motors,*/
            &_phantom.dynamics);

    _phantom.BeginPlay(_flightManager);

    Super::BeginPlay();
}

void AOpenCVPhantomPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    _phantom.EndPlay();

    Super::EndPlay(EndPlayReason);
}

// Called automatically on main thread
void AOpenCVPhantomPawn::Tick(float DeltaSeconds)
{
    _phantom.Tick(DeltaSeconds);

    _flightManager->tick();

    Super::Tick(DeltaSeconds);
}


