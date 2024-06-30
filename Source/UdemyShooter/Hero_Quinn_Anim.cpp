#include "Hero_Quinn_Anim.h"
#include "Kismet/KismetSystemLibrary.h"
#include "KismetAnimationLibrary.h"

void UHero_Quinn_Anim::NativeBeginPlay() {
    _owner = (AHero_Quinn*)(UAnimInstance::TryGetPawnOwner());
}

void UHero_Quinn_Anim::NativeUpdateAnimation(float delta) {
    if (_owner == nullptr) {
        // UE_LOG(LogCore, Warning, TEXT("Skipping animation update, because pawn owner is null"));
        return;
    }

    auto aimRotation = _owner->GetBaseAimRotation();
    auto velocity = _owner->GetVelocity();

    _isADSCpp = _owner->GetIsAdsCpp();
    _speedCpp = velocity.Size();

    _directionCpp = UKismetAnimationLibrary::CalculateDirection(velocity, aimRotation);

    // Handle replication of the pitch values
    if (aimRotation.Pitch > 90.0f) _pitchCpp = aimRotation.Pitch - 360.0f;
    else _pitchCpp = aimRotation.Pitch;
}