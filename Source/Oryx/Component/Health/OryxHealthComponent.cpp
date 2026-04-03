#include "OryxHealthComponent.h"

UOryxHealthComponent::UOryxHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOryxHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

void UOryxHealthComponent::ApplyDamage(float DamageAmount)
{
	if (DamageAmount <= 0.f || bIsDead) return;

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);

	UE_LOG(LogTemp, Log,
		TEXT("%s took %.1f damage. Health: %.1f / %.1f"),
		*GetOwner()->GetName(), DamageAmount, CurrentHealth, MaxHealth);

	OnDamaged.Broadcast(CurrentHealth, DamageAmount);

	if (CurrentHealth <= 0.f)
	{
		bIsDead = true;
		OnDeath.Broadcast(GetOwner());
	}
}

void UOryxHealthComponent::ApplyHealing(float HealAmount)
{
	if (HealAmount <= 0.f || bIsDead) return;

	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.f, MaxHealth);

	UE_LOG(LogTemp, Log,
		TEXT("%s healed %.1f. Health: %.1f / %.1f"),
		*GetOwner()->GetName(), HealAmount, CurrentHealth, MaxHealth);

	OnDamaged.Broadcast(CurrentHealth, -HealAmount);
}