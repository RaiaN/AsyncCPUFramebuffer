// Copyright PL

#include "AsyncCPUFramebuffer.h"
#include "AsyncFrameGrabber.h"

void FAsyncCPUFramebufferModule::StartupModule()
{}

void FAsyncCPUFramebufferModule::ShutdownModule()
{
	FrameGrabberInstance = nullptr;
}

UAsyncFrameGrabber* FAsyncCPUFramebufferModule::GetFrameGrabber()
{
	if (!FrameGrabberInstance.IsValid())
	{
		FrameGrabberInstance.Reset(NewObject<UAsyncFrameGrabber>(GetTransientPackage()));
	}

    return FrameGrabberInstance.Get();
}

	
IMPLEMENT_MODULE(FAsyncCPUFramebufferModule, AsyncCPUFramebuffer)
