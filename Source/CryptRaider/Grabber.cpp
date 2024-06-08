// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	// ...

	UPhysicsHandleComponent *PhysicsHandle = GetPhysicsHandle();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("PhysicsHandle not found on %s"), *GetOwner()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("PhysicsHandle found on %s"), *GetOwner()->GetName());
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UPhysicsHandleComponent *PhysicsHandle = GetPhysicsHandle();
	if (PhysicsHandle == nullptr)
	{
		return;
	}

	if (PhysicsHandle->GrabbedComponent != nullptr)
	{
		FVector ActorLocation = GetComponentLocation();
		FVector NewLocation = ActorLocation + GetForwardVector() * HoldDistance;

		PhysicsHandle->SetTargetLocationAndRotation(NewLocation, GetComponentRotation());
	}
}

void UGrabber::Release()
{
	UPhysicsHandleComponent *PhysicsHandle = GetPhysicsHandle();
	if (PhysicsHandle == nullptr)
	{
		return;
	}

	UPrimitiveComponent *GrabbedComponent = PhysicsHandle->GetGrabbedComponent();
	if (GrabbedComponent == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Nothing to release"));
		return;
	}

	GrabbedComponent->WakeAllRigidBodies();
	PhysicsHandle->ReleaseComponent();
	UE_LOG(LogTemp, Display, TEXT("Released"));
}

void UGrabber::Grab()
{
	FHitResult HitResult;
	bool IsHitting = HasHit(HitResult);

	if (IsHitting)
	{
		UE_LOG(LogTemp, Display, TEXT("Grabbing"));

		GrabComponent(HitResult);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Nothing to grab"));
	}
}

void UGrabber::GrabComponent(const FHitResult &HitResult)
{
	UPhysicsHandleComponent *PhysicsHandle = GetPhysicsHandle();
	if (PhysicsHandle == nullptr)
	{
		return;
	}

	UPrimitiveComponent *HitComponent = HitResult.GetComponent();
	HitComponent->WakeAllRigidBodies();

	PhysicsHandle->GrabComponentAtLocationWithRotation(
		HitComponent,
		NAME_None,
		HitResult.ImpactPoint,
		GetComponentRotation());
}

bool UGrabber::HasHit(FHitResult &HitResult) const
{
	UWorld *World = GetWorld();

	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDistance;

	DrawDebugLine(World, Start, End, FColor::Red, false);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
	return GetWorld()->SweepSingleByChannel(
		HitResult,
		Start, End,
		FQuat::Identity,
		ECC_GameTraceChannel2,
		Sphere);
}

UPhysicsHandleComponent *UGrabber::GetPhysicsHandle() const
{
	UPhysicsHandleComponent *PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("PhysicsHandle not found on %s"), *GetOwner()->GetName());
	}

	return PhysicsHandle;
}