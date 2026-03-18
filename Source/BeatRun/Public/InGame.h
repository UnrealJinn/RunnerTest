// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "InGame.generated.h"


/**
 * 
 */
UCLASS()
class BEATRUN_API UInGame : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerScore;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerHealth;
	/*UFUNCTION()
	void OnPlayButtonClicked();
	UFUNCTION()
	void OnSettingsButtonClicked();
	UFUNCTION()
	void OnExitButtonClicked();*/
	virtual void NativeConstruct() override;
};
