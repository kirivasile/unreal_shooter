// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hero_Quinn.h"
#include "Hero_Quinn_Anim.generated.h"

UCLASS()
class UDEMYSHOOTER_API UHero_Quinn_Anim : public UAnimInstance {
    GENERATED_BODY()

protected:
    void NativeBeginPlay() override;

    void NativeUpdateAnimation(float delta) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float _directionCpp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float _pitchCpp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float _speedCpp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool _isADSCpp;

    AHero_Quinn* _owner;
};