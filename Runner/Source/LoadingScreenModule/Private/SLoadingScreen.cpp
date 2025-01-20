// Fill out your copyright notice in the Description page of Project Settings.


#include "SLoadingScreen.h"
#include "SlateOptMacros.h"
#include "SlateExtras.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SLoadingScreen::Construct(const FArguments& InArgs)
{
	BackgroundTexture = InArgs._BackgroundTexture;
	BackgroundBrush = MakeShareable(new FSlateBrush());
	BackgroundBrush->SetResourceObject(BackgroundTexture);

	// Begin constructing the widget hierarchy
	// ChildSlot is the root slot of our compound widget=
	ChildSlot
	[
		// Create an overlay widget which allows stacking multiple elements
		SNew(SOverlay)
			// Add first slot to the overlay for the background image
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				// Create an image widget using our background brush
				SNew(SImage)
					.Image(BackgroundTexture ? BackgroundBrush.Get() : nullptr)
					.ColorAndOpacity(FLinearColor::White)
			]
			// Add second slot to the overlay for the loading indicator
			+ SOverlay::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Center)
			.Padding(10.0f)
			[
				// Create a throbber widget
				SNew(SThrobber)
					.Visibility(EVisibility::HitTestInvisible)
					.NumPieces(20)
			]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
