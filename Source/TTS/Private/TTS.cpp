// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TTS.h"


#include "LevelEditor.h"
#include "STTSMainSlate.h"
#include "TTSCommands.h"
#include "TTSStyle.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/SCanvas.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

static const FName TTSTabName("TTS");

#define LOCTEXT_NAMESPACE "FTTSModule"

class SBaseLine : SCompoundWidget
{
public:
SLATE_BEGIN_ARGS(SBaseLine)
		{
		}

	SLATE_END_ARGS()

public:
	TSharedPtr<SEditableText> SSpeechName;
	TSharedPtr<SEditableText> SSpeechText;
public:
	void Construct(const FArguments& InArgs)
	{
		ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SAssignNew(SSpeechName, SEditableText)
				.HintText(FText::FromString("Here input your SpeechName"))
			]
			+ SVerticalBox::Slot()
			[
				SAssignNew(SSpeechText, SEditableText)
				.HintText(FText::FromString("Here input your SpeechText"))
			]
		];
	}
};


void FTTSModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FTTSStyle::Initialize();
	FTTSStyle::ReloadTextures();

	FTTSCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FTTSCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FTTSModule::PluginButtonClicked),
		FCanExecuteAction());

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands,
		                               FMenuExtensionDelegate::CreateRaw(this, &FTTSModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}

	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands,
		                                     FToolBarExtensionDelegate::CreateRaw(
			                                     this, &FTTSModule::AddToolbarExtension));

		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		                        TTSTabName, FOnSpawnTab::CreateRaw(this, &FTTSModule::OnSpawnPluginTab))
	                        .SetDisplayName(LOCTEXT("FTTSTabTitle", "TTS"))
	                        .SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FTTSModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FTTSStyle::Shutdown();

	FTTSCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TTSTabName);
}

TSharedRef<SDockTab> FTTSModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(STTSMainSlate)
				// + SCanvas::Slot().HAlign(HAlign_Fill).VAlign(VAlign_Fill)
				//                              .Position(FVector2D::ZeroVector)
				//                              .Size(FVector2D(1280, 720))
				//             [
				//                 SNew(SMainSlot)
				//             ]
			]
		];
}

void FTTSModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(TTSTabName);
}

void FTTSModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FTTSCommands::Get().OpenPluginWindow);
}

void FTTSModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FTTSCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTTSModule, TTS)
