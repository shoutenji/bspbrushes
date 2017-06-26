// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BspBrushesModule.h"
#include "Modules/ModuleManager.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Textures/SlateIcon.h"
#include "EditorStyleSet.h"
#include "Engine/BrushBuilder.h"
#include "Builders/ConeBuilder.h"
#include "Builders/CubeBuilder.h"
#include "Builders/CurvedStairBuilder.h"
#include "Builders/CylinderBuilder.h"
#include "Builders/LinearStairBuilder.h"
#include "Builders/SpiralStairBuilder.h"
#include "Builders/TetrahedronBuilder.h"
#include "Widgets/SWidget.h"
#include "EditorModeRegistry.h"
#include "EditorModes.h"
#include "BspBrushesEdMode.h"
#include "BspBrushesModeStyle.h"

#define LOCTEXT_NAMESPACE "FBspBrushesModule"

/*
void FBspBrushesModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FBspBrushesEdMode>(FBspBrushesEdMode::EM_BspBrushesEdModeId, LOCTEXT("BspBrushesEdModeName", "BspBrushesEdMode"), FSlateIcon(), true);
}
*/

void FBspBrushesModule::StartupModule()
{
	FBspBrushesModeStyle::Initialize();

	/*
	FEditorModeRegistry::Get().RegisterMode<FBspMode>(
		FBuiltinEditorModes::EM_Bsp,
		NSLOCTEXT("GeometryMode", "DisplayName", "Geometry Editing"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.BspMode", "LevelEditor.BspMode.Small"),
		false,		// Visible
		100			// UI priority order
		);
	*/

	FEditorModeRegistry::Get().RegisterMode<FBspBrushesEdMode>(FBspBrushesEdMode::EM_BspBrushesEdModeId, LOCTEXT("BspBrushesEdModeName", "BspBrushesEdMode"), FSlateIcon(), true);

	RegisterBspBuilderType(UCubeBuilder::StaticClass(), LOCTEXT("CubeBuilderName", "Box"), LOCTEXT("CubeBuilderToolTip", "Make a box brush"), FBspBrushesModeStyle::Get().GetBrush(TEXT("BspBrushesMode.BoxBrush")));
	RegisterBspBuilderType(UConeBuilder::StaticClass(), LOCTEXT("ConeBuilderName", "Cone"), LOCTEXT("ConeBuilderToolTip", "Make a cone brush"), FBspBrushesModeStyle::Get().GetBrush(TEXT("BspBrushesMode.ConeBrush")));
	RegisterBspBuilderType(UCylinderBuilder::StaticClass(), LOCTEXT("CylinderBuilderName", "Cylinder"), LOCTEXT("CylinderBuilderToolTip", "Make a cylinder brush"), FBspBrushesModeStyle::Get().GetBrush(TEXT("BspBrushesMode.CylinderBrush")));
	RegisterBspBuilderType(UCurvedStairBuilder::StaticClass(), LOCTEXT("CurvedStairBuilderName", "Curved Stair"), LOCTEXT("CurvedStairBuilderToolTip", "Make a curved stair brush"), FBspBrushesModeStyle::Get().GetBrush(TEXT("BspBrushesMode.CurvedStairBrush")));
	RegisterBspBuilderType(ULinearStairBuilder::StaticClass(), LOCTEXT("LinearStairBuilderName", "Linear Stair"), LOCTEXT("LinearStairBuilderToolTip", "Make a linear stair brush"), FBspBrushesModeStyle::Get().GetBrush(TEXT("BspBrushesMode.LinearStairBrush")));
	RegisterBspBuilderType(USpiralStairBuilder::StaticClass(), LOCTEXT("SpiralStairBuilderName", "Spiral Stair"), LOCTEXT("SpiralStairBuilderToolTip", "Make a spiral stair brush"), FBspBrushesModeStyle::Get().GetBrush(TEXT("BspBrushesMode.SpiralStairBrush")));
	RegisterBspBuilderType(UTetrahedronBuilder::StaticClass(), LOCTEXT("SphereBuilderName", "Sphere"), LOCTEXT("SphereBuilderToolTip", "Make a sphere brush"), FBspBrushesModeStyle::Get().GetBrush(TEXT("BspBrushesMode.SphereBrush")));

	/*
	IPlacementModeModule& PlacementModeModule = IPlacementModeModule::Get();
	FPlacementCategoryInfo CategoryInfo(LOCTEXT("PlacementMode_Geometry", "Geometry"), "Geometry", TEXT("PMGeometry"), 35);
	CategoryInfo.CustomGenerator = []() -> TSharedRef<SWidget> { return SNew(SBspPalette); };
	PlacementModeModule.RegisterPlacementCategory(CategoryInfo);
	*/
}

/*
void FBspBrushesModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FBspBrushesEdMode::EM_BspBrushesEdModeId);
}
*/

void FBspBrushesModule::ShutdownModule()
{
	FEditorModeRegistry::Get().UnregisterMode(FBspBrushesEdMode::EM_BspBrushesEdModeId);

	BspBuilderTypes.Empty();
}

void FBspBrushesModule::RegisterBspBuilderType(class UClass* InBuilderClass, const FText& InBuilderName, const FText& InBuilderTooltip, const FSlateBrush* InBuilderIcon)
{
	check(InBuilderClass->IsChildOf(UBrushBuilder::StaticClass()));
	BspBuilderTypes.Add(MakeShareable(new FBspBuilderType(InBuilderClass, InBuilderName, InBuilderTooltip, InBuilderIcon)));
}


void FBspBrushesModule::UnregisterBspBuilderType(class UClass* InBuilderClass)
{
	BspBuilderTypes.RemoveAll(
		[InBuilderClass](const TSharedPtr<FBspBuilderType>& RemovalCandidate) -> bool
	{
		return (RemovalCandidate->BuilderClass == InBuilderClass);
	}
	);
}


const TArray< TSharedPtr<FBspBuilderType> >& FBspBrushesModule::GetBspBuilderTypes()
{
	return BspBuilderTypes;
}


TSharedPtr<FBspBuilderType> FBspBrushesModule::FindBspBuilderType(UClass* InBuilderClass) const
{
	const TSharedPtr<FBspBuilderType>* FoundBuilder = BspBuilderTypes.FindByPredicate(
		[InBuilderClass](const TSharedPtr<FBspBuilderType>& FindCandidate) -> bool
	{
		return (FindCandidate->BuilderClass == InBuilderClass);
	}
	);

	return FoundBuilder != nullptr ? *FoundBuilder : TSharedPtr<FBspBuilderType>();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBspBrushesModule, BspBrushes)