// Copyright Epic Games, Inc. All Rights Reserved.

#include "Spacelinger.h"
#include "Modules/ModuleManager.h"
#include "Utils/SLCheats.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FSpacelingerModule, Spacelinger, "Spacelinger" );

void FSpacelingerModule::StartupModule() {
	RegisterConsoleCheatsNames();
}

void FSpacelingerModule::ShutdownModule() {}