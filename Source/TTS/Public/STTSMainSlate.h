// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "http.h"
#include "Components/VerticalBox.h"
#include "Misc/FileHelper.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Input/SVolumeControl.h"
#include "Widgets/Text/STextBlock.h"


struct SoundConfig
{
	FString Sex = "zh-CN-XiaoxiaoNeural";
	FString style = "customerservice";
	FString styledegree = "1";
	FString rate = "1";
	FString volume = "100";
};

class TTS_API STTSMainSlate : public SCompoundWidget
{
public:
SLATE_BEGIN_ARGS(STTSMainSlate)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	const FString TOKEN_URI = "https://southeastasia.api.cognitive.microsoft.com/sts/v1.0/issuetoken";
	const FString SUB_KEY = "input your SUB_KEY";
	const FString URL = "https://southeastasia.tts.speech.microsoft.com/cognitiveservices/v1";
	const FString RESOURCE_NAME = "XiaoXiaoServices";
	FString SaveDir = *FPaths::ProjectSavedDir() + FString("audio/");
	FString token;

	TSharedPtr<STextBlock> STokenText;
	TSharedPtr<SHyperlink> SFolderText;
	TSharedPtr<SVerticalBox> SBaseRawContainer;
	TSharedPtr<SButton> SSpawnSpeech;
	TSharedPtr<SVolumeControl> SSpeechVolume;

	void FetchToken();
	void TokenCallBack(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	FReply CreateNewRaw();
	FReply SpawnSpeech();
	FReply ChangeFolder();
	FText UpdateFolderText() const;
	FText UpdateSelectedText() const;
	SoundConfig GetSoundConfig();
	void OpenFolder();
	SoundConfig localconfig;

private:
	void UpdateSpeechStyleList();
	TArray<TSharedPtr<FString>> speechStyleList;
};
