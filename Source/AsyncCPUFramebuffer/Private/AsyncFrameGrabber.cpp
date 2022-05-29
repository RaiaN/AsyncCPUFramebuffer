// Copyright PL

#include "AsyncFrameGrabber.h"

#include "Modules/ModuleManager.h"
#include "FrameGrabber.h"
#include "Widgets/SViewport.h"
#include "Engine/GameEngine.h"
#include "Framework/Application/SlateApplication.h"

#if WITH_EDITOR
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Slate/SceneViewport.h"
#endif

#include "AsyncCPUFramebuffer.h"

void FindSceneViewport(TSharedPtr<FSceneViewport>& OutSceneViewport)
{
    if (!GIsEditor)
    {
        UGameEngine* GameEngine = Cast<UGameEngine>(GEngine);
        OutSceneViewport = GameEngine->SceneViewport;
    }
#if WITH_EDITOR
    else
    {
        UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
        OutSceneViewport = MakeShareable((FSceneViewport*)(EditorEngine->GetPIEViewport()));
    }
#endif
}


UAsyncFrameGrabber* UAsyncFrameGrabber::GetFrameGrabberInstance()
{
    FAsyncCPUFramebufferModule& Module = FModuleManager::GetModuleChecked<FAsyncCPUFramebufferModule>(TEXT("AsyncCPUFramebuffer"));

    return Module.GetFrameGrabber();
}

void UAsyncFrameGrabber::StartCapturing()
{
    TSharedPtr<FSceneViewport> SceneViewport;
    FindSceneViewport(SceneViewport);

    GrabberInstance = MakeShared<FFrameGrabber, ESPMode::ThreadSafe>(SceneViewport.ToSharedRef(), SceneViewport->GetSize());
}

void UAsyncFrameGrabber::StopCapturing()
{
    GrabberInstance = nullptr;
}

void UAsyncFrameGrabber::Tick(float DeltaTime)
{
    if (GrabberInstance.IsValid())
    {
        GrabberInstance->CaptureThisFrame(FFramePayloadPtr());

        TArray<FCapturedFrameData> CapturedFrames = GrabberInstance->GetCapturedFrames();

        for (FCapturedFrameData& FrameData : CapturedFrames)
        {
            ReadyFrames.Enqueue(MoveTemp(FrameData.ColorBuffer));
        }
    }
}

bool UAsyncFrameGrabber::IsAllowedToTick() const
{
    return !IsTemplate();
}

TStatId UAsyncFrameGrabber::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UAsyncFrameGrabber, STATGROUP_Tickables);
}
