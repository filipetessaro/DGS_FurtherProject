// Fill out your copyright notice in the Description page of Project Settings.



#include "F_InputBindsLibrary.h"
#include "FurtherProject.h"



bool UF_InputBindsLibrary::RebindAxisKey(FInputAxis Original, FInputAxis NewBinding)
{
	UInputSettings* Settings = GetMutableDefault<UInputSettings>();
	if (!Settings)
	{
		return false;
	}

	bool bIsFound = false;
	TArray<FInputAxisKeyMapping>& Axi = Settings->AxisMappings;
	for (FInputAxisKeyMapping& Axis : Axi)
	{
		if (Axis.AxisName.ToString() != Original.AxisName)
		{
			continue;
		}

		if (Axis.Key != Original.Key)
		{
			continue;
		}

		UpdateAxisMapping(Axis, NewBinding);
		bIsFound = true;
	}

	if (!bIsFound)
	{
		return false;
	}

	// save the input to file and rebuild them
	Settings->SaveKeyMappings();
	for (TObjectIterator<UPlayerInput> It; It; ++It)
	{
		It->ForceRebuildingKeyMaps(true);
	}
	return true;
}

void UF_InputBindsLibrary::RemoveAxisKeyBind(FInputAxis ToRemove)
{
	UInputSettings* Settings = GetMutableDefault<UInputSettings>();

	if (!Settings)
	{
		return;
	}

	TArray<FInputAxisKeyMapping>& Axi = Settings->AxisMappings;

	bool bIsFound = false;
	for (int32 i = 0; i < Axi.Num(); i++)
	{
		const FInputAxisKeyMapping& Axes = Axi[i];

		if (Axes.Key != ToRemove.Key)
		{
			continue;
		}
		if (Axes.AxisName.ToString() != ToRemove.AxisName)
		{
			continue;
		}

		bIsFound = true;
		Axi.RemoveAt(i);
		break;
	}

	if (!bIsFound)
	{
		return;
	}

	// save to disk and rebuild
	Settings->SaveKeyMappings();
	for (TObjectIterator<UPlayerInput> It; It; ++It)
	{
		It->ForceRebuildingKeyMaps(true);
	}
}

void UF_InputBindsLibrary::GetAllAxisKeyBindings(TArray<FInputAxis>& Bindings)
{
	Bindings.Empty();

	const UInputSettings* Settings = GetDefault<UInputSettings>();
	if (!Settings)
	{
		return;
	}

	const TArray<FInputAxisKeyMapping>& Axi = Settings->AxisMappings;
	for (const FInputAxisKeyMapping& Axis : Axi)
	{
		Bindings.Add(FInputAxis(Axis));
	}
}

void UF_InputBindsLibrary::GetAllActionKeyBindings(TArray<FInput>& Bindings)
{
	Bindings.Empty();

	const UInputSettings* Settings = GetDefault<UInputSettings>();
	if (!Settings)
	{
		return;
	}

	const TArray<FInputActionKeyMapping>& Actions = Settings->ActionMappings;
	for (const FInputActionKeyMapping& Action : Actions)
	{
		Bindings.Add(FInput(Action));
	}
}

bool UF_InputBindsLibrary::RebindActionKey(FInput Original, FInput NewBinding)
{
	UInputSettings* Settings = GetMutableDefault<UInputSettings>();
	if (!Settings)
	{
		return false;
	}

	bool bIsFound = false;
	TArray<FInputActionKeyMapping>& Actions = Settings->ActionMappings;
	for (FInputActionKeyMapping& Action : Actions)
	{
		if (Action.ActionName.ToString() != Original.ActionName)
		{
			continue;
		}
		if (Action.Key != Original.Key)
		{
			continue;
		}

		UpdateActionMapping(Action, NewBinding);
		bIsFound = true;
		break;
	}

	if (!bIsFound)
	{
		return false;
	}

	//save to disk and rebuild input
	Settings->SaveKeyMappings();
	for (TObjectIterator<UPlayerInput> It; It; ++It)
	{
		It->ForceRebuildingKeyMaps(true);
	}
	return true;
}

void UF_InputBindsLibrary::GetAllAxisAndActionMappingsForKey(FKey Key, TArray<FInput>& ActionBindings, TArray<FInputAxis>& AxisBindings)
{
	ActionBindings.Empty();
	AxisBindings.Empty();

	const UInputSettings* Settings = GetDefault<UInputSettings>();
	if (!Settings)
	{
		return;
	}

	const TArray<FInputActionKeyMapping>& Actions = Settings->ActionMappings;
	for (const FInputActionKeyMapping& Each : Actions)
	{
		if (Each.Key == Key)
		{
			ActionBindings.Add(FInput(Each));
		}
	}

	const TArray<FInputAxisKeyMapping>& Axi = Settings->AxisMappings;
	for (const FInputAxisKeyMapping& Each : Axi)
	{
		if (Each.Key == Key)
		{
			AxisBindings.Add(FInputAxis(Each));
		}
	}
}

void UF_InputBindsLibrary::RemoveActionKeyBind(FInput ToRemove)
{
	UInputSettings* Settings = GetMutableDefault<UInputSettings>();
	if (!Settings)
	{
		return;
	}

	TArray<FInputActionKeyMapping>& Actions = Settings->ActionMappings;

	bool bIsFound = false;
	for (int32 i = 0; i < Actions.Num(); i++)
	{
		FInputActionKeyMapping& Action = Actions[i];
		if (Action.Key == ToRemove.Key)
		{
			continue;
		}

		if (Action.ActionName.ToString() == ToRemove.ActionName)
		{
			continue;
		}

		bIsFound = true;
		Actions.RemoveAt(i);
		break;
	}

	if (!bIsFound)
	{
		return;
	}

	//save to disk and rebuild input
	Settings->SaveKeyMappings();
	for (TObjectIterator<UPlayerInput> It; It; ++It)
	{
		It->ForceRebuildingKeyMaps(true);
	}
}
