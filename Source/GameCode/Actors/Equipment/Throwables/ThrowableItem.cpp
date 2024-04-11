// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableItem.h"
#include "Pawns/Character/GCBaseCharacter.h"
#include "../../Projectiles/GCProjectile.h"

void AThrowableItem::BeginPlay()
{
	Super::BeginPlay();

	SetThrowAmmo(MaxThrowAmmo);
}

void AThrowableItem::Throw()
{	
	
	AGCBaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner))
	{
		return;
	}

	if (!CanThrow())
	{
		return;
	}

	FVector LaunchDirection;
	FVector SpawnLocation;

	if (CharacterOwner->IsPlayerControlled())
	{
		FVector PlayerViewPoint;
		FRotator PlayerViewRotation;

		APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
		Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);

		FTransform PlayerViewTransform(PlayerViewRotation, PlayerViewPoint);

		FVector ViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
		FVector ViewUpVector = PlayerViewRotation.RotateVector(FVector::UpVector);

		LaunchDirection = ViewDirection + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * ViewUpVector;

		FVector ThrowableSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable);
		FVector SocketInViewSpace = PlayerViewTransform.InverseTransformPosition(ThrowableSocketLocation);

		SpawnLocation = PlayerViewPoint + ViewDirection * SocketInViewSpace.X;

	}
	else
	{
		LaunchDirection = CharacterOwner->GetActorForwardVector() + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * CharacterOwner->GetActorUpVector();
		SpawnLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable);
	}


	AGCProjectile* Projectile = GetWorld()->SpawnActor<AGCProjectile>(ProjectileClass, SpawnLocation, FRotator::ZeroRotator);
	if (IsValid(Projectile))
	{
		SetThrowAmmo(--ThrowAmmo);

		Projectile->SetOwner(GetCharacterOwner());
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
		Projectile->SetProjectileActive(true);

	}


}

void AThrowableItem::SetThrowAmmo(int32 NewAmmo)
{
	ThrowAmmo = FMath::Clamp(NewAmmo, 0, MaxThrowAmmo);
	
	if (OnThrowAmmoChanged.IsBound())
	{
		OnThrowAmmoChanged.Broadcast(ThrowAmmo);
	}
}
