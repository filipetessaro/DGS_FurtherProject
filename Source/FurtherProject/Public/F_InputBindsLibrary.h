// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerInput.h"
#include "F_InputBindsLibrary.generated.h"

USTRUCT(BlueprintType)
struct FInputAxis
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		FString AxisName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		FString KeyAsString = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		FKey Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		float Scale = 1;

	FInputAxis() {}

	FInputAxis(const FString& InAxisName, FKey InKey, float InScale) :
		AxisName(InAxisName),
		KeyAsString(InKey.GetDisplayName().ToString()),
		Key(InKey),
		Scale(InScale)
	{
	}

	FInputAxis(const FInputAxisKeyMapping& Axis) :
		Key(Axis.Key),
		KeyAsString(Axis.Key.GetDisplayName().ToString()),
		Scale(Axis.Scale)
	{
		AxisName = Axis.AxisName.ToString();
	}
};

USTRUCT(BlueprintType)
struct FInput
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		FString ActionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		FKey Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		FString KeyAsString;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		uint32 bShift : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		uint32 bCtrl : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		uint32 bAlt : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
		uint32 bCmd : 1;


	FInput() {}
	FInput(const FString InActionName, const FKey InKey, const bool bInShift, const bool bInCtrl, const bool bInAlt, const bool bInCmd)
		: Key(InKey)
		, KeyAsString(Key.GetDisplayName().ToString())
		, bShift(bInShift)
		, bCtrl(bInCtrl)
		, bAlt(bInAlt)
		, bCmd(bInCmd)
	{
		ActionName = InActionName;
	}

	FInput(const FInputActionKeyMapping& Action)
		: Key(Action.Key)
		, KeyAsString(Action.Key.GetDisplayName().ToString())
		, bShift(Action.bShift)
		, bCtrl(Action.bCtrl)
		, bAlt(Action.bAlt)
		, bCmd(Action.bCmd)
	{
		ActionName = Action.ActionName.ToString();
	}
};

UCLASS()
class FURTHERPROJECT_API UF_InputBindsLibrary : public UObject
{
	GENERATED_BODY()

public:

	static FORCEINLINE void UpdateAxisMapping(FInputAxisKeyMapping& Destination, const FInputAxis& InputBind)
	{
		Destination.Key = InputBind.Key;
		Destination.Scale = InputBind.Scale;
	}

	static FORCEINLINE void UpdateActionMapping(FInputActionKeyMapping& Destination, const FInput& InputBind)
	{
		Destination.Key = InputBind.Key;
		Destination.bShift = InputBind.bShift;
		Destination.bCtrl = InputBind.bCtrl;
		Destination.bAlt = InputBind.bAlt;
		Destination.bCmd = InputBind.bCmd;
	}

	UFUNCTION(BlueprintCallable, Category = "Input")
		static bool RebindAxisKey(FInputAxis Original, FInputAxis NewBinding);

	UFUNCTION(BlueprintCallable, Category = "Input")
		static bool RebindActionKey(FInput Original, FInput NewBinding);

	UFUNCTION(BlueprintCallable, Category = "Input")
		static void GetAllAxisKeyBindings(TArray<FInputAxis>& Bindings);

	UFUNCTION(BlueprintCallable, Category = "Input")
		static void GetAllActionKeyBindings(TArray<FInput>& Bindings);

	UFUNCTION(BlueprintCallable, Category = "Input")
		static void RemoveAxisKeyBind(FInputAxis ToRemove);

	UFUNCTION(BlueprintCallable, Category = "Input")
		static void RemoveActionKeyBind(FInput ToRemove);

	UFUNCTION(BlueprintCallable, Category = "Input")
		static void GetAllAxisAndActionMappingsForKey(FKey Key, TArray<FInput>& ActionBindings, TArray<FInputAxis>& AxisBindings);


};