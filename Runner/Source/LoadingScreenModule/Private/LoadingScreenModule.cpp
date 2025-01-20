#include "LoadingScreenModule.h"
#include "SLoadingScreen.h"
#include "MoviePlayer.h"

#define LOCTEXT_NAMESPACE "FLoadingScreenModuleModule"

void FLoadingScreenModule::StartupModule()
{
    UE_LOG(LogTemp, Display, TEXT("FLoadingScreenModuleModule::StartupModule"));

    // Load the background texture, which will be used for the loading screen
    BackgroundTexture = LoadObject<UTexture2D>(NULL, TEXT("/Script/Engine.Texture2D'/Game/Runner/Textures/UI/BG2.BG2'"));
}

bool FLoadingScreenModule::IsGameModule() const
{
	return true;
}

void FLoadingScreenModule::StartLoadingScreen()
{
    UE_LOG(LogTemp, Display, TEXT("FLoadingScreenModuleModule::StartLoadingScreen"));

    // Create a struct to hold all our loading screen settings
    FLoadingScreenAttributes LoadingScreen;
    LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
    LoadingScreen.MinimumLoadingScreenDisplayTime = 5.0f;
    LoadingScreen.WidgetLoadingScreen = SNew(SLoadingScreen).BackgroundTexture(BackgroundTexture);
    GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void FLoadingScreenModule::ShutdownModule()
{
    UE_LOG(LogTemp, Display, TEXT("FLoadingScreenModuleModule::ShutdownModule"));
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FLoadingScreenModule, LoadingScreenModule)