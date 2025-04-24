// Fill out your copyright notice in the Description page of Project Settings.


#include "SLoadingScreen.h"
#include "SlateOptMacros.h"
#include "SlateExtras.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SLoadingScreen::Construct(const FArguments& InArgs)
{
	// Store the texture passed in from the loading module
	BackgroundTexture = InArgs._BackgroundTexture;

	// Initialize the background brush and assign the texture
	BackgroundBrush = MakeShareable(new FSlateBrush());
	BackgroundBrush->SetResourceObject(BackgroundTexture);

	// Log a warning if the background texture is not set
	if (!BackgroundTexture)
	{
		UE_LOG(LogTemp, Warning, TEXT("SLoadingScreen: BackgroundTexture is null. Fallback to black background."));
	}

	// Prepare font to display
	FSlateFontInfo FontInfo = FCoreStyle::Get().GetFontStyle("NormalFont");
	FontInfo.Size = 20;

	// Begin constructing the widget hierarchy
	ChildSlot
	[
		SNew(SOverlay)

		// Slot for the background image
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SImage)
			.Image(TAttribute<const FSlateBrush*>::Create(
				[this]() { return BackgroundTexture ? BackgroundBrush.Get() : nullptr; }
			))
			// Fallback color to black if texture is not available
			.ColorAndOpacity(BackgroundTexture ? FLinearColor::White : FLinearColor::Gray)
		]

		// Slot for the loading spinner
		+ SOverlay::Slot()
		.VAlign(VAlign_Bottom)
		.HAlign(HAlign_Center)
		.Padding(10.0f)
		[
			SNew(SVerticalBox)

			// "Loading" text
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Loading")))
				.Font(FontInfo)
				.Justification(ETextJustify::Center)
				.ColorAndOpacity(FLinearColor::White)
			]

			// Spinner
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0, 5, 0, 0))
			[
				SNew(SThrobber)
				.Visibility(EVisibility::HitTestInvisible)
				.NumPieces(20)
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
