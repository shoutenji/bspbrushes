// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BspBrushesEdMode.h"
#include "BspBrushesEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"

const FEditorModeID FBspBrushesEdMode::EM_BspBrushesEdModeId = TEXT("EM_BspBrushesEdMode");

FBspBrushesEdMode::FBspBrushesEdMode()
{

}

FBspBrushesEdMode::~FBspBrushesEdMode()
{

}

void FBspBrushesEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FBspBrushesEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FBspBrushesEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

bool FBspBrushesEdMode::UsesToolkits() const
{
	return true;
}




