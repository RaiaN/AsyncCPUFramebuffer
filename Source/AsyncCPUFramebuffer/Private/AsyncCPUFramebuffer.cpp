// Copyright PL

#include "AsyncCPUFramebuffer.h"
#include "AsyncFrameGrabber.h"

void FAsyncCPUFramebufferModule::StartupModule()
{
	FrameGrabberInstance.Reset(NewObject<UAsyncFrameGrabber>(GetTransientPackage()));
}

void FAsyncCPUFramebufferModule::ShutdownModule()
{
	FrameGrabberInstance = nullptr;
}
	
IMPLEMENT_MODULE(FAsyncCPUFramebufferModule, AsyncCPUFramebuffer)

UAsyncFrameGrabber* FAsyncCPUFramebufferModule::GetFrameGrabber()
{
	return FrameGrabberInstance.Get();
}
