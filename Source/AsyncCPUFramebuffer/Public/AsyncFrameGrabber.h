// Copyright PL

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "FrameGrabber.h"
#include "AsyncFrameGrabber.generated.h"

UCLASS()
class UAsyncFrameGrabber : public UObject, public FTickableGameObject
{
    GENERATED_BODY()

public:
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

private:
    TSharedPtr<class FFrameGrabber, ESPMode::ThreadSafe> GrabberInstance;

    TQueue<TArray<FColor>, EQueueMode::Mpsc> ReadyFrames;
};