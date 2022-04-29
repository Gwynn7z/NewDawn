#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs.h"
#include "Electrodes.generated.h"

class UBoxComponent;

UCLASS()
class NEWDAWN_API AElectrodes : public AActor
{
	GENERATED_BODY()
	
	public:	
		AElectrodes();
		virtual void Tick(float DeltaTime) override;
		virtual void BeginPlay() override;

		UPROPERTY(EditAnywhere, Category="Componenti") UBoxComponent *HitBox;
		UPROPERTY(EditAnywhere, Category = "Componenti") UStaticMeshComponent* RightElectrode;
		UPROPERTY(EditAnywhere, Category = "Componenti") UStaticMeshComponent* LeftElectrode;
		UPROPERTY(EditAnywhere, Category = "Componenti") UParticleSystem* Laser;

		void Activate();
		void Launch(FAggressiveStats Damages);
		void Discharge();

	private:
		void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);
		UFUNCTION() void DestroyElectrodes();
		UFUNCTION() void HitArea();
		
		bool IsMoving;
		bool Activated;
		bool ArrivedAtFull;

		float Speed;
		FAggressiveStats Damage;
		FTimerHandle LifeTimer;
		FTimerHandle DamageTimer;
};
