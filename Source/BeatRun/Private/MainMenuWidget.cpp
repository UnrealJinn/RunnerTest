// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

#include "Kismet/GameplayStatics.h"


void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button events
	if (PlayButton)
	{
		PlayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnPlayButtonClicked);
		SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSettingsButtonClicked);
		ExitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnExitButtonClicked);
	}
}

void UMainMenuWidget::OnPlayButtonClicked()
{
	UGameplayStatics::OpenLevel(this, FName("GameMap"));
}

void UMainMenuWidget::OnSettingsButtonClicked()
{
	
}

void UMainMenuWidget::OnExitButtonClicked()
{
	// Get the player controller
	APlayerController* PlayerController = GetOwningPlayer();
    
	if (PlayerController)
	{
		// Quit the game
		UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, false);
	}
}