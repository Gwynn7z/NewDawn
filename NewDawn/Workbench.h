#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums.h"
#include "Workbench.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class NEWDAWN_API AWorkbench : public AActor
{
	GENERATED_BODY()
	
public:	
	AWorkbench();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Components") UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category = "Components") UBoxComponent* CollisionBox;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION() void Overlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void EndOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
