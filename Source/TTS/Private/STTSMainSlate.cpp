// Fill out your copyright notice in the Description page of Project Settings.


#include "STTSMainSlate.h"


#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SHeader.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Views/SHeaderRow.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Layout/SScrollBox.h"
#include "STTSBaseRaw.h"
#include "TTSStyle.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "DesktopPlatform/Public/IDesktopPlatform.h"
#include "Kismet/GameplayStatics.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION


void STTSMainSlate::Construct(const FArguments& InArgs)
{
	FetchToken();
	UpdateSpeechStyleList();
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight().VAlign(VAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[
				SNew(SBox).WidthOverride(300.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().AutoWidth().HAlign(HAlign_Center).VAlign(VAlign_Center)
					[
						SAssignNew(STokenText, STextBlock)
						.Text(FText::FromString(TEXT("Token 不存在")))
					]
					+ SHorizontalBox::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center)
					[
						SNew(SButton)
						.Text(FText::FromString("ReTry"))
						.Visibility_Lambda([this]()
			             {
				             return token.IsEmpty() ? EVisibility::Visible : EVisibility::Hidden;
			             }
			             )
						.OnClicked_Lambda([this]()
			             {
				             this->FetchToken();
				             return FReply::Handled();
			             }
						)
					]
				]


			]
			+ SHorizontalBox::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
				[
					SNew(STextBlock).Text(FText::FromString("SaveDir:"))
				]
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(5, 0, 0, 0)
				[
					SAssignNew(SFolderText, SHyperlink)
					.Text_Raw(this, &STTSMainSlate::UpdateFolderText)
                    .OnNavigate_Raw(this, &STTSMainSlate::OpenFolder)
                    .ToolTip(SNew(SToolTip).Text(FText::FromString(TEXT("打开文件夹"))))
				]
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(5, 0, 0, 0)
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("修改目录")))
					.OnClicked_Raw(this, &STTSMainSlate::ChangeFolder)
				]
				
			]
		]
		+ SVerticalBox::Slot().HAlign(HAlign_Fill).AutoHeight().VAlign(VAlign_Center)
		[
			SNew(SHeader).Content()
			[
				SNew(STextBlock).Text(FText::FromString("Setting"))
			]
		]
		+ SVerticalBox::Slot().HAlign(HAlign_Fill).AutoHeight().VAlign(VAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)//VolumeControl
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
				[
					SNew(STextBlock).Text(FText::FromString("Volume:"))
					.ToolTipText(FText::FromString(TEXT("音量")))
				]
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Fill)
				[
					SNew(SBox).WidthOverride(150.f)
					[
						SAssignNew(SSpeechVolume, SVolumeControl).Volume(1.0)
						.OnVolumeChanged_Lambda(
							[this](float inputVolume)
							{
								this->localconfig.volume = FString::SanitizeFloat( inputVolume);
							}
						)
					]
				]
			]
		+SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
				[
					SNew(STextBlock).Text(FText::FromString("Rate:"))
					.ToolTipText(FText::FromString(TEXT("语速")))
				]
				+SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Fill).Padding(5, 0, 0, 0)
				[
					SNew(SBox).WidthOverride(50.f)
					[
						SNew(SSpinBox<float>)
						.MinValue(0.0f)
						.MaxValue(2.0f)
						.Value(1.0f)
					.OnValueChanged_Lambda(
						[this](float inputValue) 
						{
							this->localconfig.rate = FString::SanitizeFloat(inputValue);
						})
					]
				]
				
			]
		+SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(10,0,0,0)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Style:"))
				]
		+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[
				SNew(SComboBox< TSharedPtr<FString> >)
				.ToolTipText_Lambda([this]()
					{
						/*("customerservice")));
						("general")));
						("assistant")));
						("chat")));
						("newscast")));
						("affectionate")));
						("angry")));
						("calm")));
						("cheerful")));
						("disgruntled")));
						("fearful")));
						("gentle")));
						("lyrical")));
						("sad")));
						("serious")));*/
						FString tipString;
						tipString += TEXT("customerservice 友好热情(default)\n");
						tipString += TEXT("general unknown\n");
						tipString += TEXT("assistant 热情而轻松的语气\n");
						tipString += TEXT("chat 轻松、随意的语气闲聊\n");
						tipString += TEXT("newscast 正式专业的语气\n");
						tipString += TEXT("affectionate 温暖而亲切的语气\n");
						tipString += TEXT("angry 恼怒的语气♥\n");
						tipString += TEXT("calm 沉着冷静\n");
						tipString += TEXT("cheerful 高音调和音量表达欢快♥♥♥\n");
						tipString += TEXT("disgruntled 轻蔑和抱怨\n");
						tipString += TEXT("fearful 恐惧和紧张\n");
						tipString += TEXT("gentle 低音调和音量表达温和和礼貌\n");
						tipString += TEXT("lyrical 优美又带感伤♥\n");
						tipString += TEXT("sad 高音调、低强度、低音量表达悲伤♥\n");
						tipString += TEXT("serious 严肃和命令♥");
						return FText::FromString(tipString);
					})
						.OptionsSource(&speechStyleList)
						.OnGenerateWidget_Lambda([this](TSharedPtr<FString> InItem) {return SNew(STextBlock).Text(FText::FromString(*InItem)); })
						.OnSelectionChanged_Lambda([this](TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo) {this->localconfig.style = *NewSelection; })
						[
							SNew(SBox).WidthOverride(100.f).HAlign(HAlign_Center)
							[
								SNew(STextBlock)
								.Text(this, &STTSMainSlate::UpdateSelectedText)
							]
						]
				]
			]
		]
		+ SVerticalBox::Slot().AutoHeight().VAlign(VAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SButton)
                .Text(FText::FromString("Create New Raw"))
                .OnClicked_Raw(this, &STTSMainSlate::CreateNewRaw)
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SAssignNew(SSpawnSpeech, SButton)
                .Text(FText::FromString("Spawn Speech"))
                .OnClicked_Raw(this, &STTSMainSlate::SpawnSpeech)
                .IsEnabled(false)
			]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(5, 0, 0, 0)
			[
				SNew(SButton)
				.ButtonStyle(&FTTSStyle::Get().GetWidgetStyle<FButtonStyle>("TTS.TTSHelp"))
				.ToolTipText_Lambda(
					[this]() 
					{
						FString ToolTipText;
						ToolTipText += TEXT("可以通过在文本间 添加<break time=\"5000ms\" /> 来实现语音的停顿\n");
						ToolTipText += TEXT("FileName不需要带文件后缀\n");
						return FText::FromString(ToolTipText);
					}
				)
			]
		]
		+ SVerticalBox::Slot().AutoHeight().VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SSplitter)
					+ SSplitter::Slot()
					[
						SNew(SHeaderRow)
						+ SHeaderRow::Column(FName("FileName")).ManualWidth(100.0f)
						.DefaultLabel(FText::FromString("FileName"))

						+ SHeaderRow::Column(FName("SpeechString")).ManualWidth(400.0f)
						.DefaultLabel(FText::FromString("SpeechString"))

						+ SHeaderRow::Column(FName("Buttons"))
						.DefaultLabel(FText::FromString("Modify"))

					]
				]
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SScrollBox)
					+SScrollBox::Slot()
					[
						SAssignNew(SBaseRawContainer, SVerticalBox)
                        + SVerticalBox::Slot().Padding(0,2).AutoHeight()
                        [
                            SNew(STTSBaseRaw, this)
                        ]
					]
					+SScrollBox::Slot()
					[
						SNew(SScrollBar)
					]
					
				]
			]
		]
	];
}

void STTSMainSlate::FetchToken()
{
	FHttpRequestPtr request = FHttpModule::Get().CreateRequest();
	request->SetHeader("Ocp-Apim-Subscription-Key", SUB_KEY);
	request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	request->SetURL(TOKEN_URI);
	request->SetVerb("POST");
	request->SetContentAsString("");
	request->OnProcessRequestComplete().BindRaw(this, &STTSMainSlate::TokenCallBack);
	request->ProcessRequest();
}

void STTSMainSlate::TokenCallBack(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	/** 判断服务器返回状态 */
	if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		/** 如果服务器未返回成功则返回 */
		return;
	}
	FString ResponseCode = HttpResponse->GetContentAsString();
	token = ResponseCode;
	FString TokenText = "Token : " + token.Left(10) + "......";
	STokenText->SetText(FText::FromString(TokenText));
	SSpawnSpeech->SetEnabled(true);
}

FReply STTSMainSlate::CreateNewRaw()
{
	SBaseRawContainer->AddSlot().Padding(0,2)
	[
		SNew(STTSBaseRaw, this)
	];
	return FReply::Handled();
}

FReply STTSMainSlate::SpawnSpeech()
{
	FChildren* Children = SBaseRawContainer->GetChildren();
	for (int i = 0; i < Children->Num(); i++)
	{
		TSharedRef<SWidget> child = Children->GetChildAt(i);
		static_cast<STTSBaseRaw&>(child->AsShared().Get()).SpawnSound();
	}
	return FReply::Handled();
}

FReply STTSMainSlate::ChangeFolder()
{
	FString OutDir = "";
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	FString DefaultDir = FPaths::ProjectSavedDir();
	DesktopPlatform->OpenDirectoryDialog(nullptr, TEXT("选取路径"), DefaultDir, OutDir);
	SaveDir = OutDir.IsEmpty() ? SaveDir : OutDir + "/";
	return FReply::Handled();
}

FText STTSMainSlate::UpdateFolderText() const
{
	return FText::FromString(FPaths::IsRelative(SaveDir) ? FPaths::ConvertRelativePathToFull(SaveDir) : SaveDir);
}

FText STTSMainSlate::UpdateSelectedText() const
{
	return FText::FromString(localconfig.style);
}

SoundConfig STTSMainSlate::GetSoundConfig()
{
	return localconfig;
}

void STTSMainSlate::OpenFolder()
{
	FString FilePath = SaveDir;
	if (FPaths::IsRelative(FilePath))
	{
		FilePath = FPaths::ConvertRelativePathToFull(FilePath);
	}
	FPlatformProcess::ExploreFolder(*FilePath);
}

void STTSMainSlate::UpdateSpeechStyleList()
{
	/* = { "general","assistant","chat","customerservice","newscast","affectionate","angry",
  //"calm", "cheerful","disgruntled","fearful","gentle","lyrical","sad","serious" };*/
	speechStyleList.Add(MakeShareable(new FString("customerservice")));
	speechStyleList.Add(MakeShareable(new FString("general")) );
	speechStyleList.Add(MakeShareable(new FString("assistant")) );
	speechStyleList.Add(MakeShareable(new FString("chat")) );
	speechStyleList.Add(MakeShareable(new FString("newscast")) );
	speechStyleList.Add(MakeShareable(new FString("affectionate")) );
	speechStyleList.Add(MakeShareable(new FString("angry")) );
	speechStyleList.Add(MakeShareable(new FString("calm")) );
	speechStyleList.Add(MakeShareable(new FString("cheerful")) );
	speechStyleList.Add(MakeShareable(new FString("disgruntled")) );
	speechStyleList.Add(MakeShareable(new FString("fearful")) );
	speechStyleList.Add(MakeShareable(new FString("gentle")) );
	speechStyleList.Add(MakeShareable(new FString("lyrical")) );
	speechStyleList.Add(MakeShareable(new FString("sad")) );
	speechStyleList.Add(MakeShareable(new FString("serious")) );
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
