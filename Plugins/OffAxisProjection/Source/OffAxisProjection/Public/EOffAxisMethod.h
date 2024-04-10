// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "EOffAxisMethod.generated.h"

/**
 * 
 */
UENUM()
enum EOffAxisMethod
{
	Slow		UMETA(DisplayName = "Slow"),
	Test		UMETA(DisplayName = "Test"),
};
