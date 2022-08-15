#include "K2Node_MyManualSwitch.h"

#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "EditorCategoryUtils.h"
#include "GraphEditorSettings.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "EdGraphUtilities.h"
#include "ToolMenu.h"

#define LOCTEXT_NAMESPACE "K2Node"

namespace
{
static const FName APinName(TEXT("A"));
static const FName BPinName(TEXT("B"));
static const FName SwitchPinName(TEXT("Switch"));
}

class FKCHandler_MyManualSwitch : public FNodeHandlingFunctor
{
public:
    FKCHandler_MyManualSwitch(FKismetCompilerContext& InCompilerContext) : FNodeHandlingFunctor(InCompilerContext)
    {
    }

    virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
    {
        UK2Node_MyManualSwitch* MyManualSwitchNode = CastChecked<UK2Node_MyManualSwitch>(Node);

        UEdGraphPin* APin = MyManualSwitchNode->FindPin(APinName);
        UEdGraphPin* BPin = MyManualSwitchNode->FindPin(BPinName);

        // �����s���uSwitch�v�̕ϐ����擾����
        UEdGraphPin* SwitchPin = MyManualSwitchNode->FindPin(SwitchPinName);
        UEdGraphPin* SwitchNet = FEdGraphUtilities::GetNetFromPin(SwitchPin);
        FBPTerminal* Switch = Context.NetMap.FindRef(SwitchNet);

        // �����s���uSwitch�v�̒l��False�̂Ƃ��ɁA���s�s���uB�v�Ɏ��s���䂪�ڂ�
        FBlueprintCompiledStatement& GotoStatement = Context.AppendStatementForNode(MyManualSwitchNode);
        GotoStatement.Type = KCST_GotoIfNot;
        GotoStatement.LHS = Switch;
        Context.GotoFixupRequestMap.Add(&GotoStatement, BPin);

        // �����s���uSwitch�v�̒l��True�̂Ƃ��ɁA���s�s���uA�v�Ɏ��s���䂪�ڂ�
        GenerateSimpleThenGoto(Context, *MyManualSwitchNode, APin);
    }
};

void UK2Node_MyManualSwitch::AllocateDefaultPins()
{
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, APinName);
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, BPinName);

    // �����s���̍쐬
    UEdGraphPin* SwitchPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, SwitchPinName);
    SwitchPin->DefaultValue = "true";
    SwitchPin->bHidden = true;
    SwitchPin->bDefaultValueIsReadOnly = true;
    SwitchPin->bNotConnectable = true;
}

FText UK2Node_MyManualSwitch::GetTooltipText() const
{
    return LOCTEXT("MyManualSwitch_Tooltip", "MyManualSwitch\nSelect execution path by following the state");
}

FLinearColor UK2Node_MyManualSwitch::GetNodeTitleColor() const
{
    return FLinearColor::White;
}

FText UK2Node_MyManualSwitch::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return LOCTEXT("MyManualSwitch", "MyManualSwitch");
}

FSlateIcon UK2Node_MyManualSwitch::GetIconAndTint(FLinearColor& OutColor) const
{
    static FSlateIcon Icon("EditorStyle", "GraphEditor.Branch_16x");
    return Icon;
}

void UK2Node_MyManualSwitch::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    UClass* ActionKey = GetClass();
    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        check(NodeSpawner != nullptr);

        ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
    }
}

FText UK2Node_MyManualSwitch::GetMenuCategory() const
{
    return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::FlowControl);
}

FNodeHandlingFunctor* UK2Node_MyManualSwitch::CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const
{
    return new FKCHandler_MyManualSwitch(CompilerContext);
}

void UK2Node_MyManualSwitch::GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const
{
    Super::GetNodeContextMenuActions(Menu, Context);

    FToolMenuSection& Section = Menu->AddSection("K2NodeMyManualSwitch", NSLOCTEXT("K2Node", "MyManualSwitchHeader", "My Manual Switch"));

    // �����s���uSwitch�v�̒l���擾����
    UEdGraphPin* Pin = FindPin(SwitchPinName);
    bool bExecuteA = Pin->GetDefaultAsString() == "true";

    if (!bExecuteA)
    {
        // ���j���[���ځuExecute A�v��ǉ�����
        Section.AddMenuEntry(
            "ExecuteA",
            LOCTEXT("ExecuteA", "Execute A"),
            LOCTEXT("ExecuteATooltip", "Switch to execute A pin"),
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateUObject(const_cast<UK2Node_MyManualSwitch*>(this), &UK2Node_MyManualSwitch::ToggleSwitch)));
    }
    else
    {
        // ���j���[���ځuExecute B�v��ǉ�����
        Section.AddMenuEntry(
            "ExecuteB",
            LOCTEXT("ExecuteB", "Execute B"),
            LOCTEXT("ExecuteBTooltip", "Switch to execute B pin"),
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateUObject(const_cast<UK2Node_MyManualSwitch*>(this), &UK2Node_MyManualSwitch::ToggleSwitch)));
    }
}

void UK2Node_MyManualSwitch::ToggleSwitch()
{
    Modify();

    // �����s���uSwitch�v�̒l��ύX����
    UEdGraphPin* Pin = FindPin(SwitchPinName);
    if (Pin->GetDefaultAsString() == "true")
    {
        Pin->DefaultValue = "false";
    }
    else
    {
        Pin->DefaultValue = "true";
    }

    // �u���[�v�����g�G�f�B�^�Ƀf�[�^���ύX���ꂽ���Ƃ�ʒm
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
}

bool UK2Node_MyManualSwitch::GetSwitchValue() const
{
    UEdGraphPin* Pin = FindPin(SwitchPinName);

    return Pin->GetDefaultAsString() == "true";
}

#undef LOCTEXT_NAMESPACE