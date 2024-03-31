// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneTransform.h"

// Sets default values for this component's properties
USceneTransform::USceneTransform()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USceneTransform::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USceneTransform::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void USceneTransform::UpdateHeadPosition() 
{
	


}


/*
* Simple average calculation.
*/
float USceneTransform::CalculateAverage(const TArray<float>& Values)
{
	if (Values.Num() == 0)
	{
		return 0.0f;
	}

	float Sum = 0.0f;
	for (float Val : Values)
	{
		Sum += Val;
	}
	return Sum / Values.Num();
}