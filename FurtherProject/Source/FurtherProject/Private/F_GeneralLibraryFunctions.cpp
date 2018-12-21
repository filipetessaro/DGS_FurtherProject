// Fill out your copyright notice in the Description page of Project Settings.


#include "F_GeneralLibraryFunctions.h"
#include "FurtherProject.h"
#include "ImageUtils.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"


/*
void Uot(FString SaveName)
{
FString ScreenshotName = SaveName;
FScreenshotRequest::RequestScreenshot(ScreenshotName, true, false);
}


UTexture2D* rtRenderTarget(UTextureRenderTarget2D * RenderTarget)
{
UTexture2D* Texture = RenderTarget->ConstructTexture2D(this, "Photo", EObjectFlags::RF_NoFlags, CTF_DeferCompression);

return Texture;
}
*/



static TSharedPtr<IImageWrapper> GetImageWrapperByExtention(const FString InImagePath)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	if (InImagePath.EndsWith(".png"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	}
	else if (InImagePath.EndsWith(".jpg") || InImagePath.EndsWith(".jpeg"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	}
	else if (InImagePath.EndsWith(".bmp"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	}
	else if (InImagePath.EndsWith(".ico"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICO);
	}
	else if (InImagePath.EndsWith(".exr"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);
	}
	else if (InImagePath.EndsWith(".icns"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICNS);
	}

	return nullptr;
}



bool UF_GeneralLibraryFunctions::CaptureComponent2D_SaveImage(class USceneCaptureComponent2D* Target, const FString ImagePath, const FLinearColor ClearColour)
{
	// Bad scene capture component!
	if ((Target == nullptr) || (Target->TextureTarget == nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("texture is null"));
		return false;
	}

	FRenderTarget* RenderTarget = Target->TextureTarget->GameThread_GetRenderTargetResource();
	if (RenderTarget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("target is null"));
		return false;
	}

	TArray<FColor> RawPixels;

	// Format not supported - use PF_B8G8R8A8.
	if (Target->TextureTarget->GetFormat() == PF_B8G8R8A8)
	{
		UE_LOG(LogTemp, Warning, TEXT("not compatible format"));
		// TRACEWARN("Format not supported - use PF_B8G8R8A8.");
		return false;
	}

	if (!RenderTarget->ReadPixels(RawPixels))
	{
		UE_LOG(LogTemp, Warning, TEXT("read pixel problem"));
		return false;
	}

	// Convert to FColor.
	FColor ClearFColour = ClearColour.ToFColor(false); // FIXME - want sRGB or not?

	for (auto& Pixel : RawPixels)
	{
		// Switch Red/Blue changes.
		const uint8 PR = Pixel.R;
		const uint8 PB = Pixel.B;
		Pixel.R = PB;
		Pixel.B = PR;

		// Set alpha based on RGB values of ClearColour.
		Pixel.A = ((Pixel.R == ClearFColour.R) && (Pixel.G == ClearFColour.G) && (Pixel.B == ClearFColour.B)) ? 0 : 255;
	}

	TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapperByExtention(ImagePath);

	const int32 Width = Target->TextureTarget->SizeX;
	const int32 Height = Target->TextureTarget->SizeY;

	if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(&RawPixels[0], RawPixels.Num() * sizeof(FColor), Width, Height, ERGBFormat::RGBA, 8))
	{
		FFileHelper::SaveArrayToFile(ImageWrapper->GetCompressed(), *ImagePath);
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("wrapper not valid"));
	return false;
}




bool UF_GeneralLibraryFunctions::Capture2D_SaveImage(class ASceneCapture2D* Target, const FString ImagePath, const FLinearColor ClearColour)
{
	return (Target) ? CaptureComponent2D_SaveImage(Target->GetCaptureComponent2D(), ImagePath, ClearColour) : false;
}



UTexture2D* UF_GeneralLibraryFunctions::LoadTexture2D_FromFileByExtension(const FString& ImagePath, bool& IsValid, int32& OutWidth, int32& OutHeight)
{
	UTexture2D* Texture = nullptr;
	IsValid = false;

	// To avoid log spam, make sure it exists before doing anything else.
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*ImagePath))
	{
		return nullptr;
	}

	TArray<uint8> CompressedData;
	if (!FFileHelper::LoadFileToArray(CompressedData, *ImagePath))
	{
		return nullptr;
	}

	TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapperByExtention(ImagePath);

	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(CompressedData.GetData(), CompressedData.Num()))
	{
		const TArray<uint8>* UncompressedRGBA = nullptr;

		if (ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA))
		{
			Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

			if (Texture != nullptr)
			{
				IsValid = true;

				OutWidth = ImageWrapper->GetWidth();
				OutHeight = ImageWrapper->GetHeight();

				void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedRGBA->GetData(), UncompressedRGBA->Num());
				Texture->PlatformData->Mips[0].BulkData.Unlock();
				Texture->UpdateResource();
			}
		}
	}

	return Texture;
}


// Get the Game Root file Path
FString UF_GeneralLibraryFunctions::FilePaths__GameRootDirectory()
{
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
}





static EImageFormat GetOurImageFormat(EOurImageFormats OurFormat)
{
	/*
	ImageWrapper.h
	namespace EImageFormat
	{

	Enumerates the types of image formats this class can handle

	enum Type
	{
	//Portable Network Graphics
	PNG,

	//Joint Photographic Experts Group
	JPEG,

	//Single channel jpeg
	GrayscaleJPEG,

	//Windows Bitmap
	BMP,

	//Windows Icon resource
	ICO,

	//OpenEXR (HDR) image file format
	EXR,

	//Mac icon
	ICNS
	};
	};

	*/


	switch (OurFormat)
	{
	case EOurImageFormats::JPG: return EImageFormat::JPEG;
	case EOurImageFormats::PNG: return EImageFormat::PNG;
	case EOurImageFormats::BMP: return EImageFormat::BMP;
	case EOurImageFormats::ICO: return EImageFormat::ICO;
	case EOurImageFormats::EXR: return EImageFormat::EXR;
	case EOurImageFormats::ICNS: return EImageFormat::ICNS;
	}
	return EImageFormat::JPEG;
}



static FString GetOurImageExtension(EOurImageFormats OurFormat)
{
	switch (OurFormat)
	{
	case EOurImageFormats::JPG: return ".jpg";
	case EOurImageFormats::PNG: return ".png";
	case EOurImageFormats::BMP: return ".bmp";
	case EOurImageFormats::ICO: return ".ico";
	case EOurImageFormats::EXR: return ".exr";
	case EOurImageFormats::ICNS: return ".icns";
	}
	return ".png";
}


UTexture2D* UF_GeneralLibraryFunctions::LoadTexture2D_FromFile(const FString& FullFilePath, EOurImageFormats ImageFormat, bool& IsValid, int32& Width, int32& Height)
{


	IsValid = false;
	UTexture2D* LoadedT2D = NULL;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(GetOurImageFormat(ImageFormat));

	//Load From File
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *FullFilePath)) return NULL;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//Create T2D!
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		const TArray<uint8>* UncompressedBGRA = NULL;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
		{
			LoadedT2D = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

			//Valid?
			if (!LoadedT2D) return NULL;
			//~~~~~~~~~~~~~~

			//Out!
			Width = ImageWrapper->GetWidth();
			Height = ImageWrapper->GetHeight();

			//Copy!
			void* TextureData = LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
			LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();

			//Update!
			LoadedT2D->UpdateResource();
		}
	}

	// Success!
	IsValid = true;
	return LoadedT2D;
}



bool UF_GeneralLibraryFunctions::GetMousePositionOnViewport(const APlayerController* MyPC, float& PosX, float& PosY)
{
	if (!MyPC) return false;
	//~~~~~~~~~~~~~

	//Get Player
	const ULocalPlayer * MyPlayer = Cast<ULocalPlayer>(MyPC->Player);
	//PlayerController::Player is UPlayer

	if (!MyPlayer) return false;
	//~~~~~~~~~~~~~~~~~~~~

	//get view port ptr
	const UGameViewportClient * MyViewportClient =
		Cast < UGameViewportClient >(MyPlayer->ViewportClient);

	if (!MyViewportClient) return false;
	//~~~~~~~~~~~~~~~~~~~~

	FViewport * MyViewport = MyViewportClient->Viewport;

	if (!MyViewport) return false;
	//~~~~~~~~~~~~~~~~~~~~

	PosX = float(MyViewport->GetMouseX());
	PosY = float(MyViewport->GetMouseY());

	return true;
}



bool UF_GeneralLibraryFunctions::SetMousePositionOnViewport(const APlayerController* MyPC, const float& PosX, const float& PosY)
{
	if (!MyPC) return false;


	const ULocalPlayer* MyPlayer = Cast<ULocalPlayer>(MyPC->Player);

	if (!MyPlayer) return false;


	const UGameViewportClient* MyViewportClient = Cast<UGameViewportClient>(MyPlayer->ViewportClient);


	if (!MyViewportClient) return false;

	FViewport* MyViewport = MyViewportClient->Viewport;


	if (!MyViewport) return false;


	MyViewport->SetMouse(int32(PosX), int32(PosY));

	return true;

}


bool UF_GeneralLibraryFunctions::SoundVolumeChange(USoundClass* SoundClassObject, float NewVolume)
{
	if (!SoundClassObject)
	{
		return false;
	}

	SoundClassObject->Properties.Volume = NewVolume;
	return true;

	/*
	FAudioDevice* Device = GEngine->GetAudioDevice();
	if (!Device || !SoundClassObject)
	{
	return false;
	}

	bool bFound = Device->SoundClasses.Contains(SoundClassObject);
	if(bFound)
	{
	Device->SetClassVolume(SoundClassObject, NewVolume);
	return true;
	}
	return false;
	*/

	/*
	bool SetBaseSoundMix( class USoundMix* SoundMix );

	*/
}

float UF_GeneralLibraryFunctions::GetSoundVolume(USoundClass* SoundClassObject)
{
	if (!SoundClassObject)
	{
		return -1;
	}
	return SoundClassObject->Properties.Volume;
	/*
	FAudioDevice* Device = GEngine->GetMainAudioDevice();
	if (!Device || !SoundClassObject)
	{
	return -1;
	}

	FSoundClassProperties* Props = Device->GetSoundClassCurrentProperties(SoundClassObject);
	if(!Props) return -1;
	return Props->Volume;
	*/
}




void UF_GeneralLibraryFunctions::DeleteFileFromPath(FString FilePath)
{
	FString AbsoluteFilePath = FilePath;
	if (!FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*AbsoluteFilePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could Not Find File"));
		return;
	}
}


TArray<FString> UF_GeneralLibraryFunctions::SortStringArray(const TArray<FString>& StringArray, const bool Reversed)
{
	TArray<FString> Array = StringArray;
	if (!Reversed)
	{
		Array.Sort([](const FString& A, const FString& B)
		{
			return A < B;
		});
	}
	else
	{
		Array.Sort([](const FString& A, const FString& B)
		{
			return A > B;
		});
	}
	return Array;
}