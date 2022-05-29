// Copyright PL

#include "AsyncFrameGrabber.h"

#include "Modules/ModuleManager.h"
#include "FrameGrabber.h"
#include "Widgets/SViewport.h"
#include "Engine/GameEngine.h"
#include "Framework/Application/SlateApplication.h"

#if WITH_EDITOR
#include "IAssetViewport.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Slate/SceneViewport.h"
#endif

#include "AsyncCPUFramebuffer.h"

PRAGMA_DISABLE_OPTIMIZATION

void FindSceneViewport(TWeakPtr<FSceneViewport>& OutSceneViewport)
{
    if (!GIsEditor)
    {
        UGameEngine* GameEngine = Cast<UGameEngine>(GEngine);
        OutSceneViewport = GameEngine->SceneViewport.ToSharedRef();
    }
#if WITH_EDITOR
    else
    {
        for (const FWorldContext& Context : GEngine->GetWorldContexts())
        {
            if (Context.WorldType == EWorldType::PIE)
            {
                FSlatePlayInEditorInfo* SlatePlayInEditorSession = GEditor->SlatePlayInEditorMap.Find(Context.ContextHandle);
                if (SlatePlayInEditorSession)
                {
                    if (SlatePlayInEditorSession->DestinationSlateViewport.IsValid())
                    {
                        TSharedPtr<IAssetViewport> DestinationLevelViewport = SlatePlayInEditorSession->DestinationSlateViewport.Pin();
                        OutSceneViewport = DestinationLevelViewport->GetSharedActiveViewport();
                    }
                    else if (SlatePlayInEditorSession->SlatePlayInEditorWindowViewport.IsValid())
                    {
                        OutSceneViewport = SlatePlayInEditorSession->SlatePlayInEditorWindowViewport;
                    }
                }
            }
        }
    }
#endif
}

bool UAsyncFrameGrabber::GetNextFrame(TArray<FColor>& OutFrame)
{
    bool bResult = ReadyFrames.Dequeue(OutFrame);
    if (bResult)
    {
        NumCapturedFrames.Decrement();
    }
    return bResult;
}

FIntPoint UAsyncFrameGrabber::GetFrameResolution() const
{
    if (TSharedPtr<FSceneViewport> SceneViewportPinner = SceneViewport.Pin())
    {
        return SceneViewportPinner->GetSize();
    }

    return FIntPoint{0, 0};
}

UAsyncFrameGrabber* UAsyncFrameGrabber::GetFrameGrabberInstance()
{
    FAsyncCPUFramebufferModule& Module = FModuleManager::GetModuleChecked<FAsyncCPUFramebufferModule>(TEXT("AsyncCPUFramebuffer"));

    return Module.GetFrameGrabber();
}

void UAsyncFrameGrabber::StartCapturing()
{
    CreateGrabber();
}

void UAsyncFrameGrabber::StopCapturing()
{
    ReleaseGrabber();
    ReadyFrames.Empty();
}

void UAsyncFrameGrabber::Tick(float DeltaTime)
{
    if (GrabberInstance.IsValid())
    {
        if (bViewportResized)
        {
            ReleaseGrabber();
            CreateGrabber();

            bViewportResized = false;
        }
    }

    // double check to make sure we still have valid grabber after handling "viewport resized" event
    if (GrabberInstance.IsValid())
    {
        GrabberInstance->CaptureThisFrame(FFramePayloadPtr());

        TArray<FCapturedFrameData> CapturedFrames(GrabberInstance->GetCapturedFrames());

        for (FCapturedFrameData& FrameData : CapturedFrames)
        {
            ReadyFrames.Enqueue(MoveTemp(FrameData.ColorBuffer));
            NumCapturedFrames.Increment();
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

void UAsyncFrameGrabber::OnViewportResized(FVector2D NewSize)
{
    bViewportResized = true;
}

void UAsyncFrameGrabber::CreateGrabber()
{
    FindSceneViewport(SceneViewport);

    if (TSharedPtr<FSceneViewport> SceneViewportPinned = SceneViewport.Pin())
    {
        // set the listener for the window resize event
        SceneViewportPinned->SetOnSceneViewportResizeDel(FOnSceneViewportResize::CreateUObject(this, &UAsyncFrameGrabber::OnViewportResized));

        GrabberInstance = MakeShared<FFrameGrabber, ESPMode::ThreadSafe>(SceneViewport.Pin().ToSharedRef(), SceneViewport.Pin()->GetSize());
        GrabberInstance->StartCapturingFrames();
    }
}

void UAsyncFrameGrabber::ReleaseGrabber()
{
    if (GrabberInstance.IsValid())
    {
        GrabberInstance->Shutdown();
        GrabberInstance = nullptr;
    }
}

PRAGMA_ENABLE_OPTIMIZATION
