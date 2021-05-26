// Fill out your copyright notice in the Description page of Project Settings.


#include "STTSBaseRaw.h"


#include "AssetRegistryModule.h"
#include "Widgets/Layout/SBox.h"
#include "SlateOptMacros.h"
#include "Components/AudioComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void STTSBaseRaw::Construct(const FArguments& InArgs, STTSMainSlate* smain)
{
	MainSlatePtr = smain;
	ChildSlot
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot().VAlign(VAlign_Center).AutoWidth().Padding(0.0, 0.0, 3.0, 0.0)
		[
			SNew(SBox).WidthOverride(100.0f).HeightOverride(30.0f)
			[
				SAssignNew(SSpeechName, SEditableTextBox).HintText(FText::FromString("Here input file name"))
				
			]
		]
		+ SHorizontalBox::Slot().VAlign(VAlign_Center).HAlign(HAlign_Fill)
		[
			SNew(SBox).WidthOverride(400.0f).HeightOverride(30.0f)
			[
				SAssignNew(SSpeechText, SEditableTextBox).HintText(FText::FromString("Here input speech string"))
				.ToolTipText_Raw(this,&STTSBaseRaw::GetTipText)
			]
		]
		+ SHorizontalBox::Slot().VAlign(VAlign_Center).HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[
				SAssignNew(STryListen, SButton)
                .Text(FText::FromString("listen"))
                .OnClicked_Raw(this, &STTSBaseRaw::TryListen)
                .IsEnabled(false)
			]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)			[
				SAssignNew(SSaveGameContent, SButton)
                .Text(FText::FromString("SaveContent"))
                .OnClicked_Raw(this, &STTSBaseRaw::SaveToGameContent)
                .IsEnabled(false)
                .Visibility(EVisibility::Collapsed)
			]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[
				SAssignNew(SReSpawn, SButton)
				.Text(FText::FromString("ReSpawn"))
				.OnClicked_Raw(this, &STTSBaseRaw::ReSpawn)
	            .IsEnabled(false)
			]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[
				SNew(SButton).Text(FText::FromString("Remove"))
				             .OnClicked_Raw(this, &STTSBaseRaw::DeleteThis)
			]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[
				SAssignNew(SProcessing, SCircularThrobber)
				.Visibility(EVisibility::Hidden)
			]
		]

	];
}


FText STTSBaseRaw::GetTipText() const
{
	return SSpeechText->GetText();
}

FReply STTSBaseRaw::DeleteThis()
{
	MainSlatePtr->SBaseRawContainer->RemoveSlot(this->AsShared());
	bNeedCallBack = false;
	return FReply::Handled();
}

FReply STTSBaseRaw::TryListen()
{
	if (!myWave)
	{
		return FReply::Handled();
	}

	UGameplayStatics::SpawnSound2D(GEngine->GetWorldContexts()[0].World(), myWave);
	return FReply::Handled();
}

FReply STTSBaseRaw::ReSpawn()
{
	SpawnSound();

	return FReply::Handled();
}

FReply STTSBaseRaw::SaveToGameContent()
{
	if (!myWave)
	{
		return FReply::Handled();
	}
	//暂时禁用这个功能
	return FReply::Handled();

	/* get full path */
	FString InFilePath = MainSlatePtr->SaveDir;
	FString localFileName = SSpeechName->GetText().ToString();
	InFilePath = InFilePath + localFileName + ".wav";

	/* Asset init */
	FString AssetPath = FPaths::ProjectContentDir()+"test/";
	FString PackagePath = FString("/Game/test");
	UPackage* Package = CreatePackage(nullptr, *PackagePath);
	/* sound wave init*/
	USoundWave* sw = NewObject<USoundWave>(Package, USoundWave::StaticClass(), *localFileName,
	                                       EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
	/* config inited wave file */
	TArray<uint8> rawFile;
	FFileHelper::LoadFileToArray(rawFile, InFilePath.GetCharArray().GetData());
	FWaveModInfo WaveInfo;

	if (WaveInfo.ReadWaveInfo(rawFile.GetData(), rawFile.Num()))
	{
		int32 DurationDiv = *WaveInfo.pChannels * *WaveInfo.pBitsPerSample * *WaveInfo.pSamplesPerSec;
		if (DurationDiv)
		{
			sw->SoundGroup = ESoundGroup::SOUNDGROUP_Default;
			sw->NumChannels = *WaveInfo.pChannels;
			sw->Duration = *WaveInfo.pWaveDataSize * 8.0f / DurationDiv;;
			sw->RawPCMDataSize = WaveInfo.SampleDataSize;
			sw->SetSampleRate(*WaveInfo.pSamplesPerSec);


			sw->InvalidateCompressedData();
			sw->RawData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(sw->RawData.Realloc(rawFile.Num()), rawFile.GetData(), rawFile.Num());
			sw->RawData.Unlock();


			sw->RawPCMDataSize = WaveInfo.SampleDataSize;
			sw->RawPCMData = (uint8*)FMemory::Malloc(sw->RawPCMDataSize);
			FMemory::Memmove(sw->RawPCMData, rawFile.GetData(), rawFile.Num());
		}
	}
	FAssetRegistryModule::AssetCreated(sw);

	sw->MarkPackageDirty();

	FString FilePath = FString::Printf(TEXT("%s%s%s"), *AssetPath, *localFileName,
	                                   *FPackageName::GetAssetPackageExtension());

	bool success = UPackage::SavePackage(Package, sw, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *FilePath);

	// FAssetRegistryModule::AssetCreated(myWave);
	//
	// myWave->MarkPackageDirty();
	// FString PackagePath = FString("/Game/test"); //ue4 目录 用来标记package
	// FString windowsPath = FString("C:/bmxdriving/Content/test");
	// FString PackageSavePath = windowsPath + "/" + SSpeechName->GetText().ToString() +
	// 	FPackageName::GetAssetPackageExtension(); //windows目录 用来保存package
	// UPackage* package = CreatePackage(nullptr, *PackagePath);
	// bool bSave = UPackage::SavePackage(package, myWave, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone,
	//                                    *PackageSavePath);
	return FReply::Handled();
}

void STTSBaseRaw::SpawnSound()
{
	if (SSpeechText->GetText().IsEmpty() || MainSlatePtr->token.IsEmpty())
	{
		return;
	}

	SoundConfig config = MainSlatePtr->GetSoundConfig();
	FString text = SSpeechText->GetText().ToString();
	FString body =
		"<speak xmlns=\"http://www.w3.org/2001/10/synthesis\" xmlns:mstts=\"http://www.w3.org/2001/mstts\" version=\"1.0\" xml:lang=\"zh-CN\"><voice name=\""
		+ config.Sex + "\"><mstts:express-as style =\"" + config.style + "\" styledegree=\"" + config.styledegree +
		"\"><prosody rate =\"" +
		config.rate + "\" volume=\"" + config.volume + "\"> " + text + "</prosody></mstts:express-as></voice></speak>";
	UE_LOG(LogTemp, Warning, TEXT("Spawn Sound With parm:character:%s volume:%s rate:%s"), *config.Sex, *config.volume,*config.rate);
	FHttpRequestPtr request = FHttpModule::Get().CreateRequest();
	request->SetHeader("Ocp-Apim-Subscription-Key", MainSlatePtr->SUB_KEY);
	request->SetHeader(TEXT("Content-Type"), TEXT("application/ssml+xml"));
	request->SetHeader(TEXT("Authorization"), "Bearer " + MainSlatePtr->token);
	request->SetHeader(TEXT("Connection"), TEXT("Keep-Alive"));
	request->SetHeader(TEXT("User-Agent"), MainSlatePtr->RESOURCE_NAME);
	request->SetHeader(TEXT("X-Microsoft-OutputFormat"), TEXT("riff-24khz-16bit-mono-pcm"));

	request->SetURL(MainSlatePtr->URL);
	request->SetVerb("POST");
	request->SetContentAsString(body);
	request->OnProcessRequestComplete().BindRaw(this, &STTSBaseRaw::SoundCallBack);
	request->ProcessRequest();
	SProcessing->SetVisibility(EVisibility::Visible);
	SReSpawn->SetEnabled(false);
	STryListen->SetEnabled(false);
	SSaveGameContent->SetEnabled(false);
}

void STTSBaseRaw::SoundCallBack(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()) || !bNeedCallBack)
	{
		return;
	}
	FString FileName;
	if (HttpResponse->GetContentType().Equals("audio/x-wav"))
	{
		FileName = SSpeechName->GetText().ToString() + ".wav";
		FFileHelper::SaveArrayToFile(HttpResponse->GetContent(),
		                             *FString::Printf(TEXT("%s%s"), *MainSlatePtr->SaveDir, *FileName));
	}
	FString Filepath = MainSlatePtr->SaveDir + FileName;
	STryListen->SetEnabled(true);
	SSaveGameContent->SetEnabled(true);
	SReSpawn->SetEnabled(true);
	myWave = LoadSoundByFile(Filepath);
	SProcessing->SetVisibility(EVisibility::Hidden);
}

USoundWave* STTSBaseRaw::LoadSoundByFile(const FString& InFilePath)
{
	USoundWave* sw = NewObject<USoundWave>(USoundWave::StaticClass(),
	                                       *SSpeechName->GetText().ToString(),
	                                       EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
	if (!sw)
		return nullptr;
	TArray<uint8> rawFile;
	FFileHelper::LoadFileToArray(rawFile, InFilePath.GetCharArray().GetData());
	FWaveModInfo WaveInfo;

	if (WaveInfo.ReadWaveInfo(rawFile.GetData(), rawFile.Num()))
	{
		int32 DurationDiv = *WaveInfo.pChannels * *WaveInfo.pBitsPerSample * *WaveInfo.pSamplesPerSec;
		if (DurationDiv)
		{
			sw->SoundGroup = ESoundGroup::SOUNDGROUP_Default;
			sw->NumChannels = *WaveInfo.pChannels;
			sw->Duration = *WaveInfo.pWaveDataSize * 8.0f / DurationDiv;;
			sw->RawPCMDataSize = WaveInfo.SampleDataSize;
			sw->SetSampleRate(*WaveInfo.pSamplesPerSec);


			sw->InvalidateCompressedData();
			sw->RawData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(sw->RawData.Realloc(rawFile.Num()), rawFile.GetData(), rawFile.Num());
			sw->RawData.Unlock();


			sw->RawPCMDataSize = WaveInfo.SampleDataSize;
			sw->RawPCMData = (uint8*)FMemory::Malloc(sw->RawPCMDataSize);
			FMemory::Memmove(sw->RawPCMData, WaveInfo.SampleDataStart, WaveInfo.SampleDataSize);
			return sw;
		}
		else
		{
			sw->Duration = 0.0f;
			return sw;
		}
	}
	else
	{
		return nullptr;
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
