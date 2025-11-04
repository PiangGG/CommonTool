// Fill out your copyright notice in the Description page of Project Settings.


#include "TagToolLibrary.h"


void UTagToolLibrary::ConvertStringToGameplayTag(FString TagString,FGameplayTag& GameplayTag)
{
	GameplayTag = FGameplayTag::RequestGameplayTag(FName(*TagString), false);
}

void UTagToolLibrary::ConvertGameplayTaTogString(const FGameplayTag& GameplayTag, FString& TagString)
{
	TagString = GameplayTag.ToString();
}
