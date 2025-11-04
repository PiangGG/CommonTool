// Fill out your copyright notice in the Description page of Project Settings.


#include "ComDatasmithActor.h"
#include "PrintToolLibrary.h"

AComDatasmithActor::AComDatasmithActor()
{
	
}

void AComDatasmithActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}

void AComDatasmithActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AComDatasmithActor::OnOpenDelta()
{
	Super::OnOpenDelta();
	UPrintToolLibrary::Debug(FString("CommonDatasmithRuntimeActor::OnOpenDelta"));
	
}

void AComDatasmithActor::OnImportEnd()
{
	Super::OnImportEnd();
	UPrintToolLibrary::Debug(FString("CommonDatasmithRuntimeActor::OnImportEnd"));
	
}