// Copyright PL

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "FrameGrabber.h"
#include "AsyncFrameGrabber.generated.h"

/* Primitive async frame grabber that buffers results to queue */
UCLASS()
class UAsyncFrameGrabber : public UObject, public FTickableGameObject
{
    GENERATED_BODY()

public:
    // Grabs next available frame
    UFUNCTION(BlueprintCallable)
    bool GetNextFrame(UPARAM(ref) TArray<FColor>& OutFrame);

    // Grabs current viewport resolution. Returns (0,0) if no valid viewport | frame grabber not initialized
    UFUNCTION(BlueprintCallable)
    FIntPoint GetFrameResolution() const;

    UFUNCTION(BlueprintCallable)
    static UAsyncFrameGrabber* GetFrameGrabberInstance();

    UFUNCTION(BlueprintCallable)
    void StartCapturing();

    UFUNCTION(BlueprintCallable)
    void StopCapturing();

protected:
    // FTickableGameObject
    void Tick(float DeltaTime) override;
    bool IsAllowedToTick() const override;
    TStatId GetStatId() const override;
    // ~FTickableGameObject

    /** Signals that the viewport was resized */
    UFUNCTION()
    void OnViewportResized(FVector2D NewSize);

private:
    void CreateGrabber();
    void ReleaseGrabber();

private:
    TSharedPtr<class FFrameGrabber, ESPMode::ThreadSafe> GrabberInstance = nullptr;
    TWeakPtr<class FSceneViewport> SceneViewport = nullptr;
    FThreadSafeCounter NumCapturedFrames = 0;
    FThreadSafeBool bViewportResized = false;

    TQueue<TArray<FColor>, EQueueMode::Mpsc> ReadyFrames;
};