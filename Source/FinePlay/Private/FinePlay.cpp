// Copyright Epic Games, Inc. All Rights Reserved.

#include "FinePlay.h"
#include "FinePlayLog.h"

#define LOCTEXT_NAMESPACE "FFinePlayModule"

void FFinePlayModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FFinePlayModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFinePlayModule, FinePlay)
DEFINE_LOG_CATEGORY(LogFinePlay);