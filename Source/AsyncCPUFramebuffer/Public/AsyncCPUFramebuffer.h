// Copyright PL

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "UObject/StrongObjectPtr.h"

class UAsyncFrameGrabber;

class FAsyncCPUFramebufferModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	UAsyncFrameGrabber* GetFrameGrabber();

private:
	TStrongObjectPtr<UAsyncFrameGrabber> FrameGrabberInstance;
};
