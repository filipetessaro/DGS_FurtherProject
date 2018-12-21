// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//Texture2D
//#include "Engine/Texture2D.h"
#include "DDSLoader.h"



//Audio
#include "Components/AudioComponent.h"
#include "AudioDecompress.h"
#include "AudioDevice.h"
#include "ActiveSound.h"
#include "Audio.h"
#include "Developer/TargetPlatform/Public/Interfaces/IAudioFormat.h"
#include "VorbisAudioInfo.h"


#include "F_GeneralLibraryFunctions.generated.h"


/**
*
*/


UENUM(BlueprintType)
enum class EOurImageFormats : uint8
{
	JPG		UMETA(DisplayName = "JPG        "),
	PNG		UMETA(DisplayName = "PNG        "),
	BMP		UMETA(DisplayName = "BMP        "),
	ICO		UMETA(DisplayName = "ICO        "),
	EXR		UMETA(DisplayName = "EXR        "),
	ICNS	UMETA(DisplayName = "ICNS        ")
};


UCLASS()
class FURTHERPROJECT_API UF_GeneralLibraryFunctions : public UObject
{
	GENERATED_BODY()

public:

	/* UFUNCTION(BlueprintCallable, Category = "ScreenShot")
	void ShootScreenshot(FString SaveName);

	UFUNCTION(BlueprintCallable, Category = "ScreenShot")
	UTexture2D* ConvertRenderTarget(UTextureRenderTarget2D* RenderTarget);
	*/

	UFUNCTION(BlueprintPure, Category = "Utilities|Array", Meta = (DisplayName = "Sort (String Array) (Copy Ours)"))
		static TArray<FString> SortStringArray(const TArray<FString>& StringArray, const bool Reversed = false);


	UFUNCTION(BlueprintCallable, Category = "BP Library Input")
		static bool GetMousePositionOnViewport(const APlayerController* MyPC, float& PosX, float& PosY);


	UFUNCTION(BlueprintCallable, Category = "BP Library Input")
		static bool SetMousePositionOnViewport(const APlayerController* MyPC, const float& PosX, const float& PosY);


	/** Make sure to include the appropriate image extension in your file path! Recommended: .bmp, .jpg, .png. Contributed by Community Member Kris! */
	UFUNCTION(BlueprintCallable)
		static bool CaptureComponent2D_SaveImage(class USceneCaptureComponent2D* Target, const FString ImagePath, const FLinearColor ClearColour);

	/** Make sure to include the appropriate image extension in your file path! Recommended: .bmp, .jpg, .png. Contributed by Community Member Kris! */
	UFUNCTION(BlueprintCallable, Meta = (DefaultToSelf = "Target"))
		static bool Capture2D_SaveImage(class ASceneCapture2D* Target, const FString ImagePath, const FLinearColor ClearColour);

	/** Make sure your image path has a valid extension!  Contributed by Community Member Kris! */
	UFUNCTION(BlueprintCallable)
		static UTexture2D*  LoadTexture2D_FromFileByExtension(const FString& ImagePath, bool& IsValid, int32& OutWidth, int32& OutHeight);

	/** InstallDir/GameName */
	UFUNCTION(BlueprintPure)
		static FString FilePaths__GameRootDirectory();

	/** Load a Texture2D from a JPG,PNG,BMP,ICO,EXR,ICNS file! IsValid tells you if file path was valid or not */
	UFUNCTION(BlueprintPure, meta = (Keywords = "image png jpg jpeg bmp bitmap ico icon exr icns"))
		static UTexture2D* LoadTexture2D_FromFile(const FString& FullFilePath, EOurImageFormats ImageFormat, bool& IsValid, int32& Width, int32& Height);

	/** Change volume of Sound class of your choosing, sets the volume instantly! Returns false if the sound class was not found and volume was not set. */
	UFUNCTION(BlueprintCallable)
		static bool SoundVolumeChange(USoundClass* SoundClassObject, float NewVolume);

	/** Get Current Sound Volume! Returns -1 if sound class was not found*/
	UFUNCTION(BlueprintPure)
		static float GetSoundVolume(USoundClass* SoundClassObject);

	/** Delete file from path */
	UFUNCTION(BlueprintCallable)
		static void DeleteFileFromPath(FString FilePath);



	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetAllMapNames", Keywords = "GAMNMaps"), Category = "MapNames")
		static FORCEINLINE TArray<FString> GetAllMapNames_p()
	{
		TArray<FString> MapFiles;

		IFileManager::Get().FindFilesRecursive(MapFiles, *FPaths::ProjectContentDir(), TEXT("*.umap"), true, false, false);

		for (int32 i = 0; i < MapFiles.Num(); i++)
		{
			//replace the whole directory string with only the name of the map

			int32 lastSlashIndex = -1;
			if (MapFiles[i].FindLastChar('/', lastSlashIndex))
			{
				FString pureMapName;

				//length - 5 because of the ".umap" suffix
				for (int32 j = lastSlashIndex + 1; j < MapFiles[i].Len() - 5; j++)
				{
					pureMapName.AppendChar(MapFiles[i][j]);
				}

				MapFiles[i] = pureMapName;
			}
		}

		return MapFiles;


	};

};