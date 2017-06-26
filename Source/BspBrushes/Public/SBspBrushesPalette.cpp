// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SBspBrushesPalette.h"
#include "Modules/ModuleManager.h"
#include "Widgets/SBoxPanel.h"
#include "SlateOptMacros.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Images/SImage.h"
#include "Styling/SlateTypes.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Input/SCheckBox.h"
#include "EditorStyleSet.h"
#include "Engine/Brush.h"
#include "BspBrushesModeStyle.h"
#include "DragAndDrop/BrushBuilderDragDropOp.h"
#include "EditorClassUtils.h"


#define LOCTEXT_NAMESPACE "BspBrushesPalette"

/** The list view mode of the asset view */
class SBspBrushesBuilderListView : public SListView<TSharedPtr<FBspBuilderType>>
{
public:
	virtual bool SupportsKeyboardFocus() const override { return true; }
	virtual FReply OnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override
	{
		return FReply::Unhandled();
	}
};

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SBspBrushesPalette::Construct(const FArguments& InArgs)
{
	bIsAdditive = true;

	FBspBrushesModule& BspBrushesModule = FModuleManager::GetModuleChecked<FBspBrushesModule>("BspBrushes");

	TSharedRef<SBspBrushesBuilderListView> ListViewWidget =
		SNew(SBspBrushesBuilderListView)
		.SelectionMode(ESelectionMode::Single)
		.ListItemsSource(&BspBrushesModule.GetBspBuilderTypes())
		.OnGenerateRow(this, &SBspBrushesPalette::MakeListViewWidget)
		.OnSelectionChanged(this, &SBspBrushesPalette::OnSelectionChanged)
		.ItemHeight(35);

	ChildSlot
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SNew(SScrollBorder, ListViewWidget)
			[
				ListViewWidget
			]
		]

	+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(3.0f)
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		[
			SNew(SCheckBox)
			.Style(FEditorStyle::Get(), "Toolbar.RadioButton")
		.ToolTipText(LOCTEXT("BspModeAdditiveTooltip", "Place brushes in additive mode."))
		.OnCheckStateChanged(this, &SBspBrushesPalette::OnAdditiveModeButtonClicked)
		.IsChecked(this, &SBspBrushesPalette::IsAdditiveModeChecked)
		.ForegroundColor(FCoreStyle::Get().GetSlateColor("Foreground"))
		.Content()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(SImage)
			.Image(this, &SBspBrushesPalette::GetAdditiveModeImage)
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(2.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("BspModeAdd", "Add"))
		.Font(FCoreStyle::Get().GetFontStyle("Toolbar.Label.Font"))
		.ShadowOffset(FVector2D::UnitVector)
		]
		]
		]

	+ SHorizontalBox::Slot()
		.Padding(3.0f)
		.AutoWidth()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		[
			SNew(SCheckBox)
			.Style(FEditorStyle::Get(), "Toolbar.RadioButton")
		.ToolTipText(LOCTEXT("BspModeSubtractiveTooltip", "Place brushes in subtractive mode."))
		.OnCheckStateChanged(this, &SBspBrushesPalette::OnSubtractiveModeButtonClicked)
		.IsChecked(this, &SBspBrushesPalette::IsSubtractiveModeChecked)
		.ForegroundColor(FCoreStyle::Get().GetSlateColor("Foreground"))
		.Content()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(SImage)
			.Image(this, &SBspBrushesPalette::GetSubtractiveModeImage)
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(2.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("BspModeSubtract", "Subtract"))
		.Font(FCoreStyle::Get().GetFontStyle("Toolbar.Label.Font"))
		.ShadowOffset(FVector2D::UnitVector)
		]
		]
		]
		]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

TSharedRef<ITableRow> SBspBrushesPalette::MakeListViewWidget(TSharedPtr<FBspBuilderType> BspBuilder, const TSharedRef<STableViewBase>& OwnerTable)
{
	check(BspBuilder.IsValid());
	check(BspBuilder->BuilderClass.IsValid());

	TSharedRef< STableRow<TSharedPtr<FBspBuilderType>> > TableRowWidget =
		SNew(STableRow<TSharedPtr<FBspBuilderType>>, OwnerTable)
		.Style(FBspBrushesModeStyle::Get(), "BspBrushesMode.TableRow")
		.OnDragDetected(this, &SBspBrushesPalette::OnDraggingListViewWidget);

	TSharedRef<SWidget> Content =
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
		.Padding(0)
		.ToolTip(FEditorClassUtils::GetTooltip(ABrush::StaticClass(), BspBuilder->ToolTipText))
		.Cursor(EMouseCursor::GrabHand)
		[
			SNew(SHorizontalBox)

			// Icon
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBorder)
			.Padding(4.0f)
		.BorderImage(FBspBrushesModeStyle::Get().GetBrush("BspBrushesMode.ThumbnailShadow"))
		[
			SNew(SBox)
			.WidthOverride(35.0f)
		.HeightOverride(35.0f)
		[
			SNew(SBorder)
			.BorderImage(FBspBrushesModeStyle::Get().GetBrush("BspBrushesMode.ThumbnailBackground"))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SImage)
			.Image(BspBuilder->Icon)
		]
		]
		]
		]

	+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.FillWidth(1.f)
		[
			SNew(STextBlock)
			.TextStyle(FBspBrushesModeStyle::Get(), "BspBrushesMode.ThumbnailText")
		.Text(BspBuilder->Text)
		]
		];

	TableRowWidget->SetContent(Content);

	return TableRowWidget;
}

void SBspBrushesPalette::OnSelectionChanged(TSharedPtr<FBspBuilderType> BspBuilder, ESelectInfo::Type SelectionType)
{
	if (BspBuilder.IsValid())
	{
		ActiveBrushBuilder = GEditor->FindBrushBuilder(BspBuilder->BuilderClass.Get());
	}
}

FReply SBspBrushesPalette::OnDraggingListViewWidget(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		if (ActiveBrushBuilder.IsValid())
		{
			FBspBrushesModule& BspBrushesModule = FModuleManager::GetModuleChecked<FBspBrushesModule>("BspBrushes");

			TSharedPtr<FBspBuilderType> BspBuilder = BspBrushesModule.FindBspBuilderType(ActiveBrushBuilder.Get()->GetClass());
			if (BspBuilder.IsValid())
			{
				// We have an active brush builder, start a drag-drop
				return FReply::Handled().BeginDragDrop(FBrushBuilderDragDropOp::New(ActiveBrushBuilder, BspBuilder->Icon, bIsAdditive));
			}
		}
	}

	return FReply::Unhandled();
}

void SBspBrushesPalette::OnAdditiveModeButtonClicked(ECheckBoxState CheckType)
{
	bIsAdditive = CheckType == ECheckBoxState::Checked;
}

void SBspBrushesPalette::OnSubtractiveModeButtonClicked(ECheckBoxState CheckType)
{
	bIsAdditive = CheckType != ECheckBoxState::Checked;
}

ECheckBoxState SBspBrushesPalette::IsAdditiveModeChecked() const
{
	return bIsAdditive ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

ECheckBoxState SBspBrushesPalette::IsSubtractiveModeChecked() const
{
	return !bIsAdditive ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

const FSlateBrush* SBspBrushesPalette::GetAdditiveModeImage() const
{
	return FBspBrushesModeStyle::Get().GetBrush("BspMode.CSGAdd.Small");
}

const FSlateBrush* SBspBrushesPalette::GetSubtractiveModeImage() const
{
	return FBspBrushesModeStyle::Get().GetBrush("BspMode.CSGSubtract.Small");
}

#undef LOCTEXT_NAMESPACE
