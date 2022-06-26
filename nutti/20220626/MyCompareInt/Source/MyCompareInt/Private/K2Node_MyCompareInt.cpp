#include "K2Node_MyCompareInt.h"

#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "EditorCategoryUtils.h"
#include "GraphEditorSettings.h"
#include "K2Node_CallFunction.h"
#include "Kismet/KismetMathLibrary.h"
#include "EdGraphUtilities.h"

#define LOCTEXT_NAMESPACE "K2Node"

namespace
{
static const FName APinName(TEXT("A"));
static const FName BPinName(TEXT("B"));
static const FName GreaterPinName(TEXT("Greater"));
static const FName EqualPinName(TEXT("Equal"));
static const FName LessPinName(TEXT("Less"));
}

class FKCHandler_MyCompareInt : public FNodeHandlingFunctor
{
    TMap<FName, FBPTerminal*> BoolTermMap;
public:
    FKCHandler_MyCompareInt(FKismetCompilerContext& InCompilerContext) : FNodeHandlingFunctor(InCompilerContext)
    {
    }

    virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* Node) override
    {
        FNodeHandlingFunctor::RegisterNets(Context, Node);

        {
            // ���C�u�����֐��uUKismetMathLibrary::GreaterEqual_IntInt�v�̎��s���ʂ��i�[����
            // Boolean�^�̈ꎞ�ϐ����쐬����
            FBPTerminal* BoolTerm = Context.CreateLocalTerminal();
            BoolTerm->Type.PinCategory = UEdGraphSchema_K2::PC_Boolean;
            BoolTerm->Source = Node;
            BoolTerm->Name = Context.NetNameMap->MakeValidName(Node, TEXT("IsGreaterEqual"));
            BoolTermMap.Add(TEXT("IsGreaterEqual"), BoolTerm);
        }
        {
            // ���C�u�����֐��uUKismetMathLibrary::LessEqual_IntInt�v�̎��s���ʂ��i�[����
            // Boolean�^�̈ꎞ�ϐ����쐬����
            FBPTerminal* BoolTerm = Context.CreateLocalTerminal();
            BoolTerm->Type.PinCategory = UEdGraphSchema_K2::PC_Boolean;
            BoolTerm->Source = Node;
            BoolTerm->Name = Context.NetNameMap->MakeValidName(Node, TEXT("IsLessEqual"));
            BoolTermMap.Add(TEXT("IsLessEqual"), BoolTerm);
        }
    }

    virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
    {
        UK2Node_MyCompareInt* MyCompareIntNode = CastChecked<UK2Node_MyCompareInt>(Node);

        // �o�͑��̎��s�s�����擾����
        UEdGraphPin* GreaterPin = MyCompareIntNode->FindPin(GreaterPinName);
        UEdGraphPin* EqualPin = MyCompareIntNode->FindPin(EqualPinName);
        UEdGraphPin* LessPin = MyCompareIntNode->FindPin(LessPinName);

        // �ꎞ�ϐ����擾����
        FBPTerminal* IsGreaterEqualTerm = BoolTermMap.FindRef(TEXT("IsGreaterEqual"));
        FBPTerminal* IsLessEqualTerm = BoolTermMap.FindRef(TEXT("IsLessEqual"));

        // ���̓s���uA�v�̕ϐ����擾����
        UEdGraphPin* APin = MyCompareIntNode->FindPin(APinName);
        UEdGraphPin* ANet = FEdGraphUtilities::GetNetFromPin(APin);
        FBPTerminal* ATerm = Context.NetMap.FindRef(ANet);

        // ���̓s���uB�v�̕ϐ����擾����
        UEdGraphPin* BPin = MyCompareIntNode->FindPin(BPinName);
        UEdGraphPin* BNet = FEdGraphUtilities::GetNetFromPin(BPin);
        FBPTerminal* BTerm = Context.NetMap.FindRef(BNet);

        {
            // ���C�u�����֐��uUKismetMathLibrary::GreaterEqual_IntInt�v��UFunction���擾����
            UEdGraphPin* FunctionPin = MyCompareIntNode->FindPin(TEXT("GreaterEqual_IntInt"));
            FBPTerminal* FunctionContext = Context.NetMap.FindRef(FunctionPin);
            UClass* FunctionClass = Cast<UClass>(FunctionPin->PinType.PinSubCategoryObject.Get());
            UFunction* FunctionPtr = FindUField<UFunction>(FunctionClass, FunctionPin->PinName);

            // ���C�u�����֐����Ăяo���X�e�[�g�����g���쐬����
            FBlueprintCompiledStatement& CallFuncStatement = Context.AppendStatementForNode(MyCompareIntNode);
            CallFuncStatement.Type = KCST_CallFunction;
            CallFuncStatement.FunctionToCall = FunctionPtr;
            CallFuncStatement.FunctionContext = FunctionContext;
            CallFuncStatement.bIsParentContext = false;
            CallFuncStatement.LHS = IsGreaterEqualTerm;
            CallFuncStatement.RHS.Add(ATerm);
            CallFuncStatement.RHS.Add(BTerm);

            // ���C�u�����֐��̌Ăяo�����ʂ���False�̂Ƃ��ɁA���s�s���uLessPin�v�Ɏ��s���䂪�ڂ�X�e�[�g�����g���쐬����
            FBlueprintCompiledStatement& GotoStatement = Context.AppendStatementForNode(MyCompareIntNode);
            GotoStatement.Type = KCST_GotoIfNot;
            GotoStatement.LHS = IsGreaterEqualTerm;
            Context.GotoFixupRequestMap.Add(&GotoStatement, LessPin);
        }

        {
            // ���C�u�����֐��uUKismetMathLibrary::LessEqual_IntInt�v��UFunction���擾����
            UEdGraphPin* FunctionPin = MyCompareIntNode->FindPin(TEXT("LessEqual_IntInt"));
            FBPTerminal* FunctionContext = Context.NetMap.FindRef(FunctionPin);
            UClass* FunctionClass = Cast<UClass>(FunctionPin->PinType.PinSubCategoryObject.Get());
            UFunction* FunctionPtr = FindUField<UFunction>(FunctionClass, FunctionPin->PinName);

            // ���C�u�����֐����Ăяo���X�e�[�g�����g���쐬����
            FBlueprintCompiledStatement& CallFuncStatement = Context.AppendStatementForNode(MyCompareIntNode);
            CallFuncStatement.Type = KCST_CallFunction;
            CallFuncStatement.FunctionToCall = FunctionPtr;
            CallFuncStatement.FunctionContext = FunctionContext;
            CallFuncStatement.bIsParentContext = false;
            CallFuncStatement.LHS = IsLessEqualTerm;
            CallFuncStatement.RHS.Add(ATerm);
            CallFuncStatement.RHS.Add(BTerm);

            // ���C�u�����֐��̌Ăяo�����ʂ�False�̂Ƃ��ɁA���s�s���uGreaterPin�v�Ɏ��s���䂪�ڂ�X�e�[�g�����g���쐬����
            FBlueprintCompiledStatement& GotoStatement = Context.AppendStatementForNode(MyCompareIntNode);
            GotoStatement.Type = KCST_GotoIfNot;
            GotoStatement.LHS = IsLessEqualTerm;
            Context.GotoFixupRequestMap.Add(&GotoStatement, GreaterPin);
        }

        // ������̏������������Ȃ��ꍇ�́A�������Ŏ��s�s���uEqualPin�v�Ɏ��s���䂪�ڂ�X�e�[�g�����g���쐬����
        GenerateSimpleThenGoto(Context, *MyCompareIntNode, EqualPin);
    }
};

void UK2Node_MyCompareInt::AllocateDefaultPins()
{
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Int, APinName);
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Int, BPinName);
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, GreaterPinName);
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, EqualPinName);
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, LessPinName);

    {
        // ���C�u�����֐��uUKismetMathLibrary::GreaterEqual_IntInt�v
        UEdGraphPin* FunctionPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UKismetMathLibrary::StaticClass(), TEXT("GreaterEqual_IntInt"));
        FunctionPin->bDefaultValueIsReadOnly = true;
        FunctionPin->bNotConnectable = true;
        FunctionPin->bHidden = true;
        UFunction* Function = FindUField<UFunction>(UKismetMathLibrary::StaticClass(), TEXT("GreaterEqual_IntInt"));
        if (Function != nullptr && Function->HasAllFunctionFlags(FUNC_Static))
        {
            UBlueprint* Blueprint = GetBlueprint();
            if (Blueprint != nullptr)
            {
                UClass* FunctionOwnerClass = Function->GetOuterUClass();
                if (!Blueprint->SkeletonGeneratedClass->IsChildOf(FunctionOwnerClass))
                {
                    FunctionPin->DefaultObject = FunctionOwnerClass->GetDefaultObject();
                }
            }
        }
    }

    {
        UEdGraphPin* FunctionPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UKismetMathLibrary::StaticClass(), TEXT("LessEqual_IntInt"));
        FunctionPin->bDefaultValueIsReadOnly = true;
        FunctionPin->bNotConnectable = true;
        FunctionPin->bHidden = true;
        UFunction* Function = FindUField<UFunction>(UKismetMathLibrary::StaticClass(), TEXT("LessEqual_IntInt"));
        if (Function != nullptr && Function->HasAllFunctionFlags(FUNC_Static))
        {
            UBlueprint* Blueprint = GetBlueprint();
            if (Blueprint != nullptr)
            {
                UClass* FunctionOwnerClass = Function->GetOuterUClass();
                if (!Blueprint->SkeletonGeneratedClass->IsChildOf(FunctionOwnerClass))
                {
                    FunctionPin->DefaultObject = FunctionOwnerClass->GetDefaultObject();
                }
            }
        }
    }
}

FText UK2Node_MyCompareInt::GetTooltipText() const
{
    return LOCTEXT("MyCompareInt_Tooltip", "MyCompareInt\nCompare one integer to another and indicate if it is greater, less than or the same.");
}

FLinearColor UK2Node_MyCompareInt::GetNodeTitleColor() const
{
    return GetDefault<UGraphEditorSettings>()->IntPinTypeColor;
}

FText UK2Node_MyCompareInt::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return LOCTEXT("MyCompareInt", "MyCompareInt");
}

FSlateIcon UK2Node_MyCompareInt::GetIconAndTint(FLinearColor& OutColor) const
{
    static FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
    return Icon;
}

void UK2Node_MyCompareInt::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    UClass* ActionKey = GetClass();
    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        check(NodeSpawner != nullptr);

        ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
    }
}

FText UK2Node_MyCompareInt::GetMenuCategory() const
{
    return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::FlowControl);
}

FNodeHandlingFunctor* UK2Node_MyCompareInt::CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const
{
    return new FKCHandler_MyCompareInt(CompilerContext);
}

#undef LOCTEXT_NAMESPACE