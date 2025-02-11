﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZombieTargetPoint.h"
#include "ZombieCharacter.generated.h"

UENUM(BlueprintType)
enum class EZombieState : uint8
{
	NORMAL	UMETA(Display = "Normal"),
	BATTLE	UMETA(Display = "Battle"),
	DEAD	UMETA(Display = "Dead"),
	CHASE	UMETA(Display = "Chase"),
	HIT		UMETA(Display = "Hit"),
};

UENUM(BlueprintType)
enum class EZombieAnimState : uint8
{
	Idle	UMETA(Display = "Idle"),
	Walk	UMETA(Display = "Walk"),
	Attack01	UMETA(Display = "Attack01"),
	Attack02	UMETA(Display = "Attack02"),
	Death	UMETA(Display = "Death"),
	Scream	UMETA(Display = "Scream"),
	Run	UMETA(Display = "Run"),
};

UCLASS()
class PROJECTBD_API AZombieCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZombieCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
		float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
		float MaxHP = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
		float RunSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
		float WalkSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		EZombieState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		EZombieState TempState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		EZombieAnimState CurrentAnimState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		TArray<AZombieTargetPoint*> Targets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float AttackRange = 200.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
		class UWidgetComponent* Widget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
		class UParticleSystem* BulletEffect;

	UFUNCTION()
		void OnSeePawn(APawn * Pawn);

	UFUNCTION()
		void OnHearNoise(APawn * Pawn, const FVector & Location, float Volume);

	UFUNCTION()
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void OnAttack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Attack = 50.0f;
};
