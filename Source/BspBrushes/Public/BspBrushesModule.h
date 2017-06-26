// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "UObject/WeakObjectPtr.h"
#include "ModuleManager.h"

struct FSlateBrush;

struct FBspBuilderType
{
	FBspBuilderType(class UClass* InBuilderClass, const FText& InText, const FText& InToolTipText, const FSlateBrush* InIcon)
		: BuilderClass(InBuilderClass)
		, Text(InText)
		, ToolTipText(InToolTipText)
		, Icon(InIcon)
	{
	}

	/** The class of the builder brush */
	TWeakObjectPtr<UClass> BuilderClass;

	/** The name to be displayed for this builder */
	FText Text;

	/** The name to be displayed for this builder */
	FText ToolTipText;

	/** The icon to be displayed for this builder */
	const FSlateBrush* Icon;
};


class FBspBrushesModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


	void RegisterBspBuilderType(class UClass* InBuilderClass, const FText& InBuilderName, const FText& InBuilderTooltip, const FSlateBrush* InBuilderIcon);
	void UnregisterBspBuilderType(class UClass* InBuilderClass);

	const TArray< TSharedPtr<FBspBuilderType> >& GetBspBuilderTypes();
	TSharedPtr<FBspBuilderType> FindBspBuilderType(UClass* InBuilderClass) const;

private:

	TArray< TSharedPtr<FBspBuilderType> > BspBuilderTypes;
};