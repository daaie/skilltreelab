// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZombieHpBarWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API UZombieHpBarWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HP")
	float Percent;
};
