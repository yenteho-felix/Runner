#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 *  Loading Screen Module Implementation
 *  Handle initialization and display of loading screens during gameplay
 */
class FLoadingScreenModule : public IModuleInterface
{
public:
    /** Called when module is first loaded and ended */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /** Identifies this as a game module, not an editor module */
    virtual bool IsGameModule() const override { return true; };

    /** Shows the loading screen during the level transitions */
    virtual void StartLoadingScreen(const FString& MapName);

private:
    /** Store the background texture to prevent it from being garbage collected */
    UTexture2D* BackgroundTexture; 
};
