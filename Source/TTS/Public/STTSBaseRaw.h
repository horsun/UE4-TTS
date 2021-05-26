// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Images/SThrobber.h"
#include "STTSMainSlate.h"
#include "Sound/SoundWave.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class TTS_API STTSBaseRaw : public SCompoundWidget
{
public:
SLATE_BEGIN_ARGS(STTSBaseRaw)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, STTSMainSlate* smain);
private:
	USoundWave* myWave = nullptr;
	bool bNeedCallBack = true;
public:
	TSharedPtr<SEditableTextBox> SSpeechName;
	TSharedPtr<SEditableTextBox> SSpeechText;
	TSharedPtr<SButton> STryListen;
	TSharedPtr<SButton> SSaveGameContent;
	TSharedPtr<SButton> SReSpawn;
	TSharedPtr<SCircularThrobber> SProcessing;
	STTSMainSlate* MainSlatePtr = nullptr;
public:
	FText GetTipText() const;
	FReply DeleteThis();
	FReply TryListen();
	FReply ReSpawn();
	FReply SaveToGameContent();
	void SpawnSound();
	void SoundCallBack(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	USoundWave* LoadSoundByFile(const FString& InFilePath);
};
