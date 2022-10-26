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
	
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UPhysicsHandleComponent* PhysicsHandle =  GetPhysicsHandle();
	

	if(PhysicsHandle && PhysicsHandle-> GetGrabbedComponent() != nullptr)
	{
		FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
		PhysicsHandle-> SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
	}
	
	
	
	// ...
}

void UGrabber::Release(){
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	
	if(PhysicsHandle && PhysicsHandle-> GetGrabbedComponent() != nullptr)
	{
		AActor* Actor = PhysicsHandle->GetGrabbedComponent()-> GetOwner();
		Actor->Tags.Remove("Grabbed");
		PhysicsHandle->GetGrabbedComponent()->WakeAllRigidBodies();
		PhysicsHandle->ReleaseComponent();
	}
}

void UGrabber::Grab(){
	
	UPhysicsHandleComponent* PhysicsHandle =  GetPhysicsHandle();
	if(PhysicsHandle == nullptr){
		return;
	}

	
	FHitResult HitResult;
	bool HasHit = GetGrabbableInReach(HitResult);
		
		if(HasHit){
			UPrimitiveComponent* HitComponent = HitResult.GetComponent();
			HitComponent->SetSimulatePhysics(true);
			HitComponent -> WakeAllRigidBodies();
			AActor* HitActor = HitResult.GetActor();
			HitActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			HitResult.GetActor()-> Tags.Add("Grabbed");

			PhysicsHandle->GrabComponentAtLocationWithRotation(
				HitComponent,
				NAME_None,
				HitResult.ImpactPoint,
				GetComponentRotation()
			);

		}
		else{
			UE_LOG(LogTemp, Display, TEXT("No Actor Hit"));
		}
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const{

	return GetOwner() -> FindComponentByClass<UPhysicsHandleComponent>();
}

bool UGrabber::GetGrabbableInReach(FHitResult& OutHitResult) const{
	FVector Start = GetOwner()-> GetActorLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDistance;
	DrawDebugLine(GetWorld(), Start, End, FColor::Red);
	DrawDebugSphere(GetWorld(), End, 10, 10, FColor::Blue, false, 5);
	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
	return GetWorld() -> SweepSingleByChannel(
		OutHitResult,
		Start, 
		End, 
		FQuat::Identity, 
		ECC_GameTraceChannel2,
		Sphere
		);
}