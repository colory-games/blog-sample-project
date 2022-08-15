#include "SGraphNodeMyManualSwitch.h"

#include "K2Node_MyManualSwitch.h"
#include "NodeFactory.h"

void SGraphNodeMyManualSwitch::Construct(const FArguments& InArgs, UK2Node_MyManualSwitch* InNode)
{
    GraphNode = InNode;

    SetCursor(EMouseCursor::GrabHand);

    UpdateGraphNode();
}

void SGraphNodeMyManualSwitch::CreatePinWidgets()
{
    UK2Node_MyManualSwitch* MyManualSwitch = CastChecked<UK2Node_MyManualSwitch>(GraphNode);

    // �s����UI��ǉ�
    for (auto It = GraphNode->Pins.CreateConstIterator(); It; ++It)
    {
        UEdGraphPin* Pin = *It;
        if (!Pin->bHidden)
        {
            TSharedPtr<SGraphPin> NewPin = FNodeFactory::CreatePinWidget(Pin);
            check(NewPin.IsValid());

            AddPin(NewPin.ToSharedRef());
        }
    }

    bool bExecuteA = MyManualSwitch->GetSwitchValue();

    // ���_��ǉ�
    LeftNodeBox->AddSlot()
        .AutoHeight()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Center)
        [
            SNew(SImage)
            .Image(FEditorStyle::GetBrush("Graph.Pin.DefaultPinSeparator"))
        ];
    // ���s��̃e�L�X�g��ǉ�
    LeftNodeBox->AddSlot()
        .AutoHeight()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Center)
        .Padding(10.0f, 5.0f)
        [
            SNew(STextBlock)
            .Text(FText::FromString(bExecuteA ? "Execute A" : "Execute B"))
        ];
}
