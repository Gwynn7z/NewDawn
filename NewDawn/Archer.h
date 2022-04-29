#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Structs.h"
#include "Enums.h"
#include "Archer.generated.h"

class UArcherAnimInstance;
class UShapeComponent;
class UCameraComponent;
class UCapsuleComponent;
class USpringArmComponent;
class USceneComponent;
class ABow;
class UInventory;
class USoundCue;
class AEnemy;
class AAllyDrone;
class ACameraActor;
class UDefensesBars;
class UMenu;
class UUserWidget;
class UInteractWidget;
class UWeaponWidget;
class UBowInventoryItem;
class UUsableInventoryItem;

UCLASS()
class NEWDAWN_API AArcher : public APawn
{
	GENERATED_BODY()

public:
	AArcher();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//COMPONENTS
	UPROPERTY(EditAnywhere, Category = "Componenti")    UCapsuleComponent* Root;
	UPROPERTY(EditAnywhere, Category = "Componenti")    USkeletalMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category = "Componenti")	ACameraActor* Camera;
	UPROPERTY(EditAnywhere, Category = "Componenti")	ACameraActor* CameraBow;
	UPROPERTY(EditAnywhere, Category = "Componenti")	USpringArmComponent* CameraSpringArm;
	UPROPERTY(EditAnywhere, Category = "Componenti")	USpringArmComponent* CameraSpringArmBow;

	UPROPERTY(EditAnywhere, Category = "Componenti")	USpringArmComponent* DroneSpringArm;
	UPROPERTY(EditAnywhere, Category = "Componenti")    USceneComponent* FloatingDroneLocation;

	//ANIMATIONS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animazioni") UArcherAnimInstance* AnimInstance;

	//BOW FUNCTIONS
	FORCEINLINE UFUNCTION() bool GetIsAiming() { return isAiming; };
	UFUNCTION(BlueprintCallable) void GrabArrow();
	UFUNCTION(BlueprintCallable) void KnockArrow();
	UFUNCTION(BlueprintCallable) void PutBowBack();
	UFUNCTION() bool ArrowOutOfQuiver();
	UFUNCTION() void UpdateEquippedBow(UBowInventoryItem* NewBow);
	UFUNCTION() void UpdateShortcutItem(UUsableInventoryItem* NewItem);

	//MOVE FUNCTIONS
	UFUNCTION(BlueprintCallable) void LeftStepSound();
	UFUNCTION(BlueprintCallable) void RightStepSound();
	UFUNCTION() void SetCapsuleCollision(bool Collision);
	UFUNCTION() AAllyDrone* GetOwningDrone();
	void Retarget();
	

	//HP FUNCTIONS
	bool Heal(float Amount);
	void GetHit(TMap<EDamageTypes, float> IncomingDamage);
	FORCEINLINE UFUNCTION()	ELifeState GetLifeState() { return LifeState; };

	void AddFightingEnemy(AEnemy* NewEnemy);
	void RemoveFightingEnemy(AEnemy* OldEnemy);
	TArray<AEnemy*> GetFightingEnemies();

	void AddLog(FString Text);
	void InventoryDrop(FUnownedInventory NewInventory);

protected:
	virtual void BeginPlay() override;

private:
	//INVENTORY FUNCTIONS
	UFUNCTION() void BowChangeUp();
	UFUNCTION() void BowChangeDown();
	UFUNCTION() void ArrowChangeUp();
	UFUNCTION() void ArrowChangeDown();
	UFUNCTION() void OpenMenu();
	UFUNCTION() void ShortcutItem();
	void AddToInventory(FUnownedInventory* NewInventory);
	void SetBow(ABow* Newbow);
	void SetArrowType(EDamageTypes ArrowType);
	int32 MaxEquippedBows;

	//POINTERS
	AAllyDrone* Drone;
	ABow* CurrentBow;
	TArray<ABow*> EquippedBows;
	TArray<AEnemy*> FightingEnemies;

	//ENUMS AND STRUCTS / STATS
	UPROPERTY() UInventory* Inventory;
	EArcherMovement MoveState;
	EArcherSpeed Speed;
	EArcherStand StandPosition;
	ELifeState LifeState;
	FDefensiveStats DefensiveStats;
	
	//HUD
	UPROPERTY()
		TSubclassOf<UUserWidget> BarsAssetClass;
	UPROPERTY()
		TSubclassOf<UUserWidget> MenuAssetClass;
	UPROPERTY()
		TSubclassOf<UUserWidget> InteractAssetClass;
	UPROPERTY()
		TSubclassOf<UUserWidget> WeaponAssetClass;
	UPROPERTY()
		UDefensesBars* Bars;
	UPROPERTY() 
		UMenu* MenuWidget;
	UPROPERTY()
		UInteractWidget* InteractWidget;
	UPROPERTY()
		UWeaponWidget* WeaponWidget;

	void SetDefaultBarValues();
	void UpdateHPBar();

	//INTERACT VARIABLES
	const float InteractLineLenght = 250.f;
	AActor* CurrentlyInteractingActor;
	FTimerHandle InteractTimer;

	//CHECKING VARIABLES
	bool isAiming;
	bool isDrawing;
	bool isSprinting;
	bool isFlying;
	bool isDodging;
	bool isGoingToCollide;
	bool isBowBack;
	bool isPilotingDrone;

	//BOW VARIABLES
	EDamageTypes CurrentArrowType;
	FTimerHandle BackBowTimer;
	
	//BOW FUNCTIONS
	void Draw(float Value);
	void Release();
	void SetIsDrawing(bool IsDrawing);

	void Aim(float Value); //Smoother with Axis
	void StopAim();
	void SetIsAiming(bool IsAiming);

	void TakeBowBack();
	void GrabBow();
	void SetBowBack(bool BowBack);
	void PutAwayArrow();

	//MOVE FUNCTIONS
	void StartRunning();
	void StopRunning();
	void StartJump();
	void Stop();
	void SimulateWalkGravity();
	void CollideBlock(FVector ForwardDirection);
	void Turn(float Value);
	void LookUp(float Value);
	void MoveForward(float Value);
	void MoveRight(float Value);
	void StandUp();
	void Crouch();
	void ToggleCrouch();
	void Dodge();
	void AdjustCrouchLocations();

	//MOVE VARIABLES
	float YawSpeed;
	float DodgeSpeed;
	float JumpVy;
	float MoveIncreaserVx;
	const float HitboxExtent = 50.f;
	const float DodgeDistance = 500.f;
	const float GravityLineLength = 100.f;
	FVector MovingDirection;
	UAnimMontage* Walk;
	FVector DodgeArrive;

	//VFX/SFX
	USoundCue* ConcreteRightStep;
	USoundCue* ConcreteLeftStep;
	USoundCue* DirtRightStep;
	USoundCue* DirtLeftStep;
	UParticleSystem* DodgeParticle;

	//INTERACT FUNCTIONS
	UFUNCTION() AActor* GetActorInInteractLine();
	UFUNCTION() void Interact();
	UFUNCTION() void InteractCycle();

	//FUNCTIONS
	UFUNCTION() void ToggleLight();
	UFUNCTION() void SwapToDrone();
};
