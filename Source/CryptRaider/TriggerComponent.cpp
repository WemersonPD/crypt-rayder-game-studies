// Fill out your copyright notice in the Description page of Project Settings.

#include "TriggerComponent.h"

UTriggerComponent::UTriggerComponent()
{
    // Allow us to use the tick function
    // This is useful for checking if the player is within the trigger box
    // and then triggering an event
    PrimaryComponentTick.bCanEverTick = true;
}

void UTriggerComponent::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Display, TEXT("Trigger Component Initiated"));
}

void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor *KeyActor = GetAcceptableActor();
    if (KeyActor == nullptr)
    {
        // UE_LOG(LogTemp, Display, TEXT("Acceptable Actor Not Found"));
        // No acceptable actor found

        // Return the Mover to the original position
        Mover->SetShouldMove(false);
        return;
    }

    // Get the Key Component to disable physics
    UPrimitiveComponent *KeyComponent = Cast<UPrimitiveComponent>(KeyActor->GetRootComponent());
    if (KeyComponent != nullptr)
    {
        KeyComponent->SetSimulatePhysics(false);
    }

    // Attach the Key Actor to the Trigger Box (A door for example)
    KeyActor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);

    // Start the movement.
    Mover->SetShouldMove(true);
}

AActor *UTriggerComponent::GetAcceptableActor() const
{

    TArray<AActor *> OverlappingActors;
    GetOverlappingActors(OverlappingActors);

    if (OverlappingActors.Num() == 0)
    {
        return nullptr;
    }

    for (AActor *Actor : OverlappingActors)
    {

        bool HasKeyTag = Actor->ActorHasTag(KeyTag);
        bool IsGrabbing = Actor->ActorHasTag("Grabbed");

        if (HasKeyTag && !IsGrabbing)
        {
            return Actor;
        }
    }

    return nullptr;
}

void UTriggerComponent::SetMover(UMover *NewMover)
{
    Mover = NewMover;
}