#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Enums.h"
#include "Structs.h"
#include "AllyDrone.generated.h"

class UCameraComponent;
class UBoxComponent;
class USpringArmComponent;
class USpotLightComponent;
class UPointLightComponent;
class UParticleSystemComponent;
class AArcherProjectGameModeBase;
class ADroneHUD;
class AWeight;
class AItems;
class AArcher;
class AAllyDroneAI;
class UAllyDroneAnimInstance;
class USoundCue;
class AElectrodes;
class UDefensesBars;
class UInventory;
class UMenu;
class UInteractWidget;
class UDroneWeaponsWidget;
class UDroneAbility;
class AEnemy;
class AWorkbench;
class UDroneUpgrade;

UCLASS()
class NEWDAWN_API AAllyDrone : public APawn
{
	GENERATED_BODY()
public:
	AAllyDrone();
	//OVERRIDES
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void BeginPlay() override;

	//COMPONENTS
	//----------------------------------------
	UPROPERTY(EditAnywhere, Category = "Componenti")    USkeletalMeshComponent* Drone;
	UPROPERTY(EditAnywhere, Category = "Componenti")	UCameraComponent* ThirdPersonCamera;
	UPROPERTY(EditAnywhere, Category = "Componenti")	UCameraComponent* FirstPersonCamera;
	UPROPERTY(EditAnywhere, Category = "Componenti")	USpringArmComponent* CameraSpringArm;
	UPROPERTY(EditAnywhere, Category = "Componenti")	USpotLightComponent* Light;
	UPROPERTY(EditAnywhere, Category = "Componenti")    UParticleSystemComponent* SparkComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animazioni") UAllyDroneAnimInstance* AnimInstance;

	//FUNCTIONS
	void SetHUDBarWidget(UDefensesBars* NewBars);
	void SetHUDInteractWidget(UInteractWidget* NewInteract);
	void ShareMenu(UMenu* NewMenu);
	void CheckStartFighting();
	void CheckStopFighting();
	void SetAbility(ESide Side, UDroneAbility* NewAbility);
	void SetCollisionPreset();
	
	UFUNCTION()	void ToggleLight();
	UFUNCTION() void Pilot();
	UFUNCTION() void Shoot();
	UFUNCTION() void SwapToArcher();
	UFUNCTION() void AddFightingEnemy(AEnemy* NewEnemy);
	UFUNCTION() void RemoveFightingEnemy(AEnemy* NewEnemy);

	//HP FUNCTIONS
	void Heal(float Amount);
	void GetHit(TMap<EDamageTypes, float> Damages);
	void Break();
	bool IsFullHP();
	TTuple<float, float> GetHPForBar();
	TMap<EResourceTypes, int32> GetRequiredMaterials();

	void UseEnergy(float Value);
	void GainEnergy(float Value);
	void DrainEnergy(float Value);
	float GetEnergyLeft();

	UFUNCTION() void SetDroneState(EAllyDroneState State);
	FORCEINLINE UFUNCTION()	EAllyDroneState GetLifeState() { return DroneState; };

	void OpenMenu();
	UFUNCTION() void SetArcher(AArcher* NewArcher);
	FORCEINLINE UFUNCTION() AArcher* GetArcher() { return Archer; };
	FRotator CurrentRotation;

	UFUNCTION() UDroneAbility* GetAbility(ESide Side);
	UFUNCTION() TArray<UDroneAbility*> GetUnlockedAbilities();
	UFUNCTION() TArray<UDroneUpgrade*> GetUpgradeSlots();
	UFUNCTION() TArray<UDroneUpgrade*> GetUnlockedUpgrades();
	UFUNCTION() void AddToSlot(UDroneUpgrade* NewUpgrade);
	UFUNCTION() void RemoveFromSlot(UDroneUpgrade* NewUpgrade);

	void SetOverlappingWorkbench(bool isOverlapping);
	bool IsWorkbenchOverlapping();
	bool GetIsDown();

private:
	//SFX/VFX VARIABLES
	UPROPERTY(EditAnywhere, Category = "Componenti") UParticleSystem* ShootSpark;
	UPROPERTY(EditAnywhere, Category = "Componenti") UParticleSystem* Sparks;
	USoundCue* ShootSound;

	//COMPONENTS
	UPROPERTY() AWeight* GrabbedItem;
	UPROPERTY() AActor* CurrentlyInteractingActor;
	UPROPERTY() AActor* CurrentlyGrabbingActor;
	UPROPERTY() AWorkbench* CurrentOverlappingWorkbench;

	UPROPERTY() AArcher* Archer;
	UPROPERTY() AAllyDroneAI* DroneAI;
	UPROPERTY() AElectrodes* CurrentElectrode;

	UPROPERTY() TArray<UDroneAbility*> UnlockedAbilities;
	UPROPERTY() TArray<UDroneUpgrade*> UpgradeSlots;
	UPROPERTY() TArray<UDroneUpgrade*> UnlockedUpgrades;
	UPROPERTY() UDroneAbility* RightAbility;
	UPROPERTY() UDroneAbility* LeftAbility;

	TMap<EResourceTypes, int32> DroneDestroyed;
	TMap<EResourceTypes, int32> DroneStillAlive;
	bool bWorkbenchIsOverlapping;

	//STATS VARIABLES
	UPROPERTY() TArray<AEnemy*> FightingEnemies;
	EAllyDroneState DroneState;
	FDefensiveStats DefensiveStats;
	FAggressiveStats CannonDamage;
	FAggressiveStats ElectrodesDamage; 
	TMap<EDamageTypes, float> CollisionDamage;
	TMap<EDamageTypes, float> DrainDamage;
	float OverrideCharge;
	float OverrideMaxCharge;
	float MaxEnergyValue;
	float EnergyValue;

	//HUD
	UPROPERTY() UDefensesBars* Bars;
	UPROPERTY() UMenu* Menu;
	UPROPERTY() UInteractWidget* InteractWidget;
	UPROPERTY() UDroneWeaponsWidget* WeaponsWidget;
	UPROPERTY() TSubclassOf<UUserWidget> WeaponAssetClass;
	void SetDefaultBarValues();
	void UpdateHPBar();
	void UpdateEnergyBar();
	void UpdateOverrideBar();


	//MOVE VARIABLES
	//----------------------------------------
	//Forward-Backward
	UPROPERTY(EditAnywhere, Category = "Variabili") float ForwardThrustPower;
	UPROPERTY(EditAnywhere, Category = "Variabili") float ForwardThrustResistance;
	UPROPERTY(EditAnywhere, Category = "Variabili") float ForwardOverheat;
	float CurrentForwardThrust;
	float TempOverheat;

	//Right-Left
	UPROPERTY(EditAnywhere, Category = "Variabili") float RightThrustPower;
	UPROPERTY(EditAnywhere, Category = "Variabili") float RightThrustResistance;
	UPROPERTY(EditAnywhere, Category = "Variabili") float RightOverheat;
	float CurrentRightThrust;

	//Up-Down
	UPROPERTY(EditAnywhere, Category = "Variabili") float UpThrustPower;
	UPROPERTY(EditAnywhere, Category = "Variabili") float UpThrustResistance;
	UPROPERTY(EditAnywhere, Category = "Variabili") float UpOverheat;
	float CurrentUpThrust;

	//Yaw-Roll
	UPROPERTY(EditAnywhere, Category = "Variabili") float YawSpeed;
	UPROPERTY(EditAnywhere, Category = "Variabili") float RollSpeed;
	UPROPERTY(EditAnywhere, Category = "Variabili") float PitchSpeed;
	UPROPERTY(EditAnywhere, Category = "Variabili") float MaxPitch;
	UPROPERTY(EditAnywhere, Category = "Variabili") bool InvertAxis;
	//----------------------------------------


	//INTERACT FUNCTIONS
	UFUNCTION() AActor* GetActorInInteractLine();
	UFUNCTION() void InteractCycle();
	UFUNCTION() AActor* GetActorInGrabLine();
	UFUNCTION() void Interact();
	UFUNCTION()	void Grab();
	UFUNCTION()	void Attach();
	UFUNCTION()	void Detach();

	//INTERACT VARIABLES
	FTimerHandle InteractTimer;
	const float InteractLineLenght = 100.f;
	const float GrabLineLenght = 50.f;

	//MOVE FUNCTIONS
	UFUNCTION()	void Turn(float Amount);
	UFUNCTION()	void Pitch(float Amount);
	UFUNCTION()	void ForwardThrust(float Amount);
	UFUNCTION()	void RightThrust(float Amount);
	UFUNCTION()	void UpThrust(float Amount);
	UFUNCTION()	void CameraSwitch();
	UFUNCTION() void SpeedUp(float Amount);
	UFUNCTION() void Stop();

	//SHOOT FUNCTIONS
	UFUNCTION()	void LeftClickAbility();
	UFUNCTION() void LeftReleaseAbility();
	UFUNCTION()	void RightClickAbility();
	UFUNCTION()	void RightReleaseAbility();
	UFUNCTION()	void RightHoldAbility(float Value);
	UFUNCTION()	void LeftHoldAbility(float Value);
	UFUNCTION() void RechargeShot();
	UFUNCTION() void AimElectrodes(float Value);
	UFUNCTION() void ActivateElectrodes();

	//SHOOT VARIABLES
	FTimerHandle ShootTimer;
	bool CanShoot;
	float ElectrodesEnergyCost;
	float ShotEnergyCost;

	//CONSTANTS
	const FVector LightLocation = FVector(10.f, 0.f, 0.f);
	const float LightIntensity = 3000.f;
	const float LightRange = 1000.f;
	const FRotator SpringArmRotation = FRotator(-15.f, 0.f, 0.f);
	const FVector SpringArmLocation = FVector(-200.f, 0.f, 100.f);
	const FRotator CameraRotation = FRotator(15.f, 0.f, 0.f);
	const FVector CameraLocation = FVector(0.f, 0.f, 0.f);
	const float armLenght = 50.f;
};
