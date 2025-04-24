#include "LoadingScreenModule.h"
#include "SLoadingScreen.h"
#include "LevelLoadingSettings.h"
#include "MoviePlayer.h"

#define LOCTEXT_NAMESPACE "FLoadingScreenModuleModule"

void FLoadingScreenModule::StartupModule()
{
    UE_LOG(LogTemp, Display, TEXT("FLoadingScreenModuleModule::StartupModule"));

    // Load the background texture, which will be used for the loading screen
    if (ULevelLoadingSettings* LoadingSettings = GetMutableDefault<ULevelLoadingSettings>())
    {
        BackgroundTexture = LoadingSettings->BackgroundImage.LoadSynchronous();
        if (!BackgroundTexture)
        {
            UE_LOG(LogTemp, Warning, TEXT("BackgroundTexture failed to load."));
        }
    }
}

void FLoadingScreenModule::ShutdownModule()
{
    UE_LOG(LogTemp, Display, TEXT("FLoadingScreenModuleModule::ShutdownModule"));
}

void FLoadingScreenModule::StartLoadingScreen(const FString& MapName)
{
    UE_LOG(LogTemp, Display, TEXT("FLoadingScreenModuleModule::StartLoadingScreen"));
    
    // boolean to indicate if this map should show a loading screen
    bool bShouldShowLoadingScreen = false;

    // Loop through all maps that should have loading screens
    ULevelLoadingSettings* LoadingSettings = GetMutableDefault<ULevelLoadingSettings>();
    if (LoadingSettings)
    {
        for (const FSoftObjectPath& MapPath : LoadingSettings->MapsWithLoadingScreens)
        {
            if (MapPath.GetAssetPathString().Contains(MapName))
            {
                bShouldShowLoadingScreen = true;
                break;
            }
        }
    }
    
    if (!bShouldShowLoadingScreen)
    {
        return;
    }

    // Create a struct to hold all our loading screen settings
    FLoadingScreenAttributes LoadingScreen;
    LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
    LoadingScreen.MinimumLoadingScreenDisplayTime = LoadingSettings->MinLoadingScreenDisplayTime;
    LoadingScreen.WidgetLoadingScreen = SNew(SLoadingScreen).BackgroundTexture(BackgroundTexture);
    GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FLoadingScreenModule, LoadingScreenModule)