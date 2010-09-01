/*******************************************************************************
 * Copyright (c) 2006, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include "org_eclipse_ercp_swt_mobile_internal_OS.h"
#include "eswtmobileextensions.h"
#include "mmf/common/mmfcontrollerpluginresolver.h"
#include "templatehelpers.h"
#include "utils.h"
#include "swtdialogbroker.h"


// ======== LOCAL FUNCTIONS ========


template<class C>
static jint NewMobileWidget(JNIEnv* aJniEnv,
                            C*(MSwtFactory::*aMethodL)(MSwtDisplay&, TSwtPeer, MSwtComposite&, TInt) const,
                            jobject aPeer,
                            jint aParent,
                            jint aStyle)
{
    CSwtDisplay& display = CSwtDisplay::Current();
    const MSwtFactory* factory = &display.Factory();
    return NewWidget(aJniEnv, factory, aMethodL, display, aPeer, aParent, aStyle);
}


extern "C"
{

    /*
     * Class CaptionedControl
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_CaptionedControl_1New(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jint aParent,
        jint aStyle)
    {
        CSwtDisplay& display = CSwtDisplay::Current();
        const MSwtFactory& factory = display.Factory();
        jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
        if (peerRef == NULL)
            return 0;

        MSwtComposite*   parent  = reinterpret_cast<MSwtComposite*>(aParent);
        MSwtCaptionedControl* result  = NULL;
        TRAPD(error, CallMethodL(result, &factory, &MSwtFactory::NewCaptionedControlL,
                                 display, peerRef, *parent, aStyle));
        if (error)
        {
            aJniEnv->DeleteWeakGlobalRef(peerRef);
            Throw(error, aJniEnv);
        }

        INCREASE_INSTANCE_COUNT(result, CaptionedControl);

        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_CaptionedControl_1CompositeHandle(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtCaptionedControl* theControl = reinterpret_cast<MSwtCaptionedControl*>(aHandle);
        MSwtComposite*      result;
        CallMethod(result, theControl, &MSwtCaptionedControl::Composite);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_CaptionedControl_1SetText(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jstring aString)
    {
        CallTextMethodThrow(aJniEnv, aHandle, &MSwtCaptionedControl::SetTextL, aString);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_CaptionedControl_1SetTrailingText(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jstring aString)
    {
        CallTextMethodThrow(aJniEnv, aHandle, &MSwtCaptionedControl::SetTrailingTextL, aString);
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_CaptionedControl_1GetText(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        const MSwtCaptionedControl* theControl = reinterpret_cast<MSwtCaptionedControl*>(aHandle);
        const TDesC* text;
        CallMethod(text, theControl, &MSwtCaptionedControl::GetText);
        return NewJavaString(aJniEnv, (text != NULL) ? *text : KNullDesC());
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_CaptionedControl_1GetTrailingText(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        const MSwtCaptionedControl* theControl = reinterpret_cast<MSwtCaptionedControl*>(aHandle);
        const TDesC* text;
        CallMethod(text, theControl, &MSwtCaptionedControl::GetTrailingText);
        return NewJavaString(aJniEnv, (text != NULL) ? *text : KNullDesC());
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_CaptionedControl_1SetImage(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aImageHandle)
    {
        MSwtCaptionedControl* theControl = reinterpret_cast<MSwtCaptionedControl*>(aHandle);
        MSwtImage* image = reinterpret_cast<MSwtImage*>(aImageHandle);
        TRAPD(error, CallMethodL(theControl, &MSwtCaptionedControl::SetImageL, image));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_CaptionedControl_1SetChildPreferredSize(JNIEnv*, jclass, jint aHandle, jint aWidth, jint aHeight)
    {
        MSwtCaptionedControl* theControl = reinterpret_cast<MSwtCaptionedControl*>(aHandle);
        TSize size(aWidth, aHeight);
        CallMethod(theControl, &MSwtCaptionedControl::SetChildPreferredSize, size);
    }


    /*
     * Class Command
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Command_1New(JNIEnv* aJniEnv,
            jclass, jobject aJavaPeer, jint aParentControl, jint aParentCommand, jint aType, jint aPriority)
    {
        CSwtDisplay& display = CSwtDisplay::Current();
        MSwtControl* parentControl = reinterpret_cast<MSwtControl*>(aParentControl);
        MSwtCommand* parentCommand = reinterpret_cast<MSwtCommand*>(aParentCommand);
        const MSwtFactory* factory = &display.Factory();

        jweak peerRef = aJniEnv->NewWeakGlobalRef(aJavaPeer);
        if (peerRef == NULL)
            return 0;

        MSwtCommand* result = NULL;
        TRAPD(error, CallMethodL(result, factory, &MSwtFactory::NewCommandL,
                                 display, peerRef, *parentControl, parentCommand, aType, aPriority));
        if (error)
        {
            aJniEnv->DeleteWeakGlobalRef(peerRef);
            Throw(error, aJniEnv);
        }

        INCREASE_INSTANCE_COUNT(result, Command);

        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Command_1Dispose(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtCommand* command = reinterpret_cast<MSwtCommand*>(aHandle);
        TSwtPeer     peerRef;
        CallMethod(peerRef, command, &MSwtCommand::Dispose);

        ReleasePeer(aJniEnv, peerRef);

        DECREASE_INSTANCE_COUNT(Command);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Command_1SetAccelerator(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aAccelerator)
    {
        MSwtCommand* command = reinterpret_cast<MSwtCommand*>(aHandle);
        TRAPD(error, CallMethodL(command, &MSwtCommand::SetAcceleratorL, aAccelerator));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Command_1IsDefaultCommand(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        const MSwtCommand* command = reinterpret_cast<MSwtCommand*>(aHandle);
        TBool result;
        CallMethod(result, command, &MSwtCommand::IsDefaultCommand);
        return ConvertBoolean(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Command_1SetDefaultCommand(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtCommand* command = reinterpret_cast<MSwtCommand*>(aHandle);
        TBool thisIsTrue = ETrue;
        TRAPD(error, CallMethodL(command, &MSwtCommand::SetDefaultCommand, thisIsTrue));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Command_1SetText(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jstring aText)
    {
        CallTextMethodThrow(aJniEnv, aHandle, &MSwtCommand::SetTextL, aText);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Command_1SetImage(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aImageHandle)
    {
        MSwtCommand* command = reinterpret_cast<MSwtCommand*>(aHandle);
        MSwtImage*    image  = reinterpret_cast<MSwtImage*>(aImageHandle);
        TRAPD(error, CallMethodL(command, &MSwtCommand::SetImageL, image));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Command_1SetLongLabel(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jstring aText)
    {
        CallTextMethodThrow(aJniEnv, aHandle, &MSwtCommand::SetLongLabelL, aText);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Command_1SetEnabled(
        JNIEnv*, jclass,
        jint aHandle,
        jboolean aEnabled)
    {
        MSwtCommand* command = reinterpret_cast<MSwtCommand*>(aHandle);
        CallMethod(command, &MSwtCommand::SetEnabled, aEnabled);
    }


    /*
     * Class ConstrainedText
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1New(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jint aParent,
        jint aStyle)
    {
        jint handle = NewMobileWidget(aJniEnv, &MSwtFactory::NewConstrainedTextL,
                                      aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, ConstrainedText);
        return handle;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1SetConstraint(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aConstraint)
    {
        MSwtConstrainedText* constrText = reinterpret_cast<MSwtConstrainedText*>(aHandle);
        MSwtConstrainedText::TConstraint constraint = static_cast<MSwtConstrainedText::TConstraint>(
                    aConstraint);
        TRAPD(error, CallMethodL(constrText, &MSwtConstrainedText::SetConstraintL, constraint));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1ControlHandle(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtConstrainedText* constrText = reinterpret_cast<MSwtConstrainedText*>(aHandle);
        MSwtControl*         result;
        CallMethod(result, constrText, &MSwtConstrainedText::Control);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1GetText(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        return CallTextGetterThrow(aJniEnv, aHandle, &MSwtConstrainedText::GetTextL);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1SetText(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jstring aText)
    {
        return ConvertBoolean(CallBoolTextMethodThrow(aJniEnv, aHandle, &MSwtConstrainedText::SetTextL,
                              aText, EFalse));
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1Append(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jstring aText)
    {
        return ConvertBoolean(CallBoolTextMethodThrow(aJniEnv, aHandle, &MSwtConstrainedText::AppendL,
                              aText, EFalse));
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1Insert(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jstring aText)
    {
        return ConvertBoolean(CallBoolTextMethodThrow(aJniEnv, aHandle, &MSwtConstrainedText::InsertL,
                              aText, EFalse));
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1GetCharCount(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        const MSwtConstrainedText* constrText = reinterpret_cast<MSwtConstrainedText*>(aHandle);
        TInt result;
        CallMethod(result, constrText, &MSwtConstrainedText::GetCharCount);
        return result;
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1SetTextLimit(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aLimit)
    {
        MSwtConstrainedText* constrText = reinterpret_cast<MSwtConstrainedText*>(aHandle);
        TBool result;
        TRAPD(error, CallMethodL(result, constrText, &MSwtConstrainedText::SetTextLimitL, aLimit));
        ThrowIfError(error, aJniEnv);
        return ConvertBoolean(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1GetCaretPosition(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        const MSwtConstrainedText* constrText = reinterpret_cast<MSwtConstrainedText*>(aHandle);
        TInt result;
        CallMethod(result, constrText, &MSwtConstrainedText::GetCaretPosition);
        return result;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1SetSelection(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aStart,
        jint aEnd)
    {
        MSwtConstrainedText* constrText = reinterpret_cast<MSwtConstrainedText*>(aHandle);
        TRAPD(error, CallMethodL(constrText, &MSwtConstrainedText::SetSelectionL, aStart, aEnd));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1ClearSelection(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtConstrainedText* constrText = reinterpret_cast<MSwtConstrainedText*>(aHandle);
        TRAPD(error, CallMethodL(constrText, &MSwtConstrainedText::ClearSelectionL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1SelectAll(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtConstrainedText* constrText = reinterpret_cast<MSwtConstrainedText*>(aHandle);
        TRAPD(error, CallMethodL(constrText, &MSwtConstrainedText::SelectAllL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1ShowSelection(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtConstrainedText* constrText = reinterpret_cast<MSwtConstrainedText*>(aHandle);
        TRAPD(error, CallMethodL(constrText, &MSwtConstrainedText::ShowSelectionL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1Cut(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtConstrainedText* constrText = reinterpret_cast<MSwtConstrainedText*>(aHandle);
        TBool result = EFalse;
        TRAPD(error, CallMethodL(result, constrText, &MSwtConstrainedText::CutL));
        ThrowIfError(error, aJniEnv);
        return ConvertBoolean(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1Copy(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtConstrainedText* constrText = reinterpret_cast<MSwtConstrainedText*>(aHandle);
        TRAPD(error, CallMethodL(constrText, &MSwtConstrainedText::CopyL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1Paste(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtConstrainedText* constrText = reinterpret_cast<MSwtConstrainedText*>(aHandle);
        TBool result = EFalse;
        TRAPD(error, CallMethodL(result, constrText, &MSwtConstrainedText::PasteL));
        ThrowIfError(error, aJniEnv);
        return ConvertBoolean(result);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1GetEditable(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        const MSwtConstrainedText* constrText = reinterpret_cast<MSwtConstrainedText*>(aHandle);
        TBool result;
        CallMethod(result, constrText, &MSwtConstrainedText::GetEditable);
        return ConvertBoolean(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ConstrainedText_1SetEditable(
        JNIEnv* /*aJniEnv*/,
        jclass,
        jint aHandle,
        jboolean aEditable)
    {
        MSwtConstrainedText* constrText = reinterpret_cast<MSwtConstrainedText*>(aHandle);
        CallMethod(constrText, &MSwtConstrainedText::SetEditable, aEditable);
    }


    /*
     * Class DateEditor
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_DateEditor_1New(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jint aParent,
        jint aStyle)
    {
        jint handle = NewMobileWidget(aJniEnv, &MSwtFactory::NewDateEditorL, aPeer,
                                      aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, DateEditor);
        return handle;
    }

    JNIEXPORT jintArray JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_DateEditor_1GetDate(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtDateEditor* dateEditor = reinterpret_cast<MSwtDateEditor*>(aHandle);
        TInt64 time;
        CallMethod(time, dateEditor, &MSwtDateEditor::GetDate);

        time /= 1000;
        TInt splitDate[2];
        splitDate[0] = I64HIGH(time);
        splitDate[1] = I64LOW(time);
        return NewJavaIntArray(aJniEnv, splitDate, 2);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_DateEditor_1GetOffset(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        const MSwtDateEditor* dateEditor = reinterpret_cast<MSwtDateEditor*>(aHandle);
        TInt result(0);
        CallMethod(result, dateEditor, &MSwtDateEditor::GetOffset);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_DateEditor_1Handle(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtDateEditor* dateEditor = reinterpret_cast<MSwtDateEditor*>(aHandle);
        MSwtControl*    result;
        CallMethod(result, dateEditor, &MSwtDateEditor::Control);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_DateEditor_1SetDate(
        JNIEnv*,
        jclass,
        jint aHandle,
        jint aDateHigh,
        jint aDateLow)
    {
        MSwtDateEditor* dateEditor = reinterpret_cast<MSwtDateEditor*>(aHandle);
        TInt64 dateInMicroSec(MAKE_TINT64(static_cast<TUint>(aDateHigh),
                                          static_cast<TUint>(aDateLow)));
        dateInMicroSec *= 1000;
        CallMethod(dateEditor, &MSwtDateEditor::SetDate, dateInMicroSec);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_DateEditor_1SetOffset(
        JNIEnv*,
        jclass,
        jint aHandle,
        jint aSeconds)
    {
        MSwtDateEditor* dateEditor = reinterpret_cast<MSwtDateEditor*>(aHandle);
        CallMethod(dateEditor, &MSwtDateEditor::SetOffset, aSeconds);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_DateEditor_1SetDateEditorStyle(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aDateEditorStyle)
    {
        MSwtDateEditor* dateEditor = reinterpret_cast<MSwtDateEditor*>(aHandle);
        TRAPD(error, CallMethodL(dateEditor, &MSwtDateEditor::SetDateEditorStyleL, aDateEditorStyle));
        ThrowIfError(error, aJniEnv);
    }


    /*
     * Class HyperLink
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_HyperLink_1ControlHandle(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtHyperLink* hyperLink = reinterpret_cast<MSwtHyperLink*>(aHandle);
        MSwtControl*    result;
        CallMethod(result, hyperLink, &MSwtHyperLink::Control);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_HyperLink_1New(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jint aParent,
        jint aStyle)
    {
        jint handle = NewMobileWidget(aJniEnv, &MSwtFactory::NewHyperLinkL,
                                      aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, HyperLink);
        return handle;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_HyperLink_1SetText(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jstring aString)
    {
        CallTextMethodThrow(aJniEnv, aHandle, &MSwtHyperLink::SetTextL, aString);
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_HyperLink_1GetText(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtHyperLink* hyperLink = reinterpret_cast<MSwtHyperLink*>(aHandle);
        const TDesC* text = NULL;
        CallMethod(text, hyperLink, &MSwtHyperLink::GetText);
        return NewJavaString(aJniEnv, (text != NULL) ? *text : KNullDesC());
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_HyperLink_1SetFormat(
        JNIEnv* /*aJniEnv*/,
        jclass,
        jint aHandle,
        jint aFormat)
    {
        MSwtHyperLink* hyperLink = reinterpret_cast<MSwtHyperLink*>(aHandle);
        CallMethod(hyperLink, &MSwtHyperLink::SetFormat, aFormat);
    }


    /*
     * Class Input
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Input_1New(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jint aId)
    {
        CSwtDisplay& display = CSwtDisplay::Current();
        const MSwtFactory* mobileFactory = &display.Factory();
        jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
        if (peerRef == NULL)
            return 0;

        MSwtInput* input = NULL;
        TRAPD(error, CallMethodL(input, mobileFactory, &MSwtFactory::NewInputL,
                                 peerRef, aId));
        if (error)
        {
            aJniEnv->DeleteWeakGlobalRef(peerRef);
            Throw(error, aJniEnv);
        }

        INCREASE_INSTANCE_COUNT(input, Input);

        return reinterpret_cast<jint>(input);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Input_1Dispose(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtInput* input = reinterpret_cast<MSwtInput*>(aHandle);
        TSwtPeer   peerRef;
        CallMethod(peerRef, input, &MSwtInput::Dispose);

        ReleasePeer(aJniEnv, peerRef);

        DECREASE_INSTANCE_COUNT(Input);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Input_1GetLocation(
        JNIEnv* /*aJniEnv*/,
        jclass,
        jint aHandle)
    {
        MSwtInput* input = reinterpret_cast<MSwtInput*>(aHandle);
        TInt location = 0;
        CallMethod(location, input, &MSwtInput::GetLocation);
        return location;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Input_1GetType(
        JNIEnv* /*aJniEnv*/,
        jclass,
        jint aHandle)
    {
        MSwtInput* input = reinterpret_cast<MSwtInput*>(aHandle);
        TInt type = 0;
        CallMethod(type, input, &MSwtInput::GetType);
        return type;
    }


    /*
     * Class ListBox
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1New(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jint aParent,
        jint aStyle)
    {
        jint handle = NewMobileWidget(
                          aJniEnv,
                          &MSwtFactory::NewListBoxL,
                          aPeer,
                          aParent,
                          aStyle);
        INCREASE_INSTANCE_COUNT(handle, ListBox);
        return handle;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1SetLayoutStyle(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aLayoutStyle)
    {
        MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        TRAPD(
            error,
            CallMethodL(
                theListBox,
                &MSwtListBox::SetLayoutStyleL,
                aLayoutStyle));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1ScrollableHandle(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        MSwtScrollable* result;
        CallMethod(
            result,
            theListBox,
            &MSwtListBox::Scrollable);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1Deselect__II(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aIndex)
    {
        MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        TRAPD(
            error,
            CallMethodL(
                theListBox,
                &MSwtListBox::DeselectItemL,
                aIndex));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1Deselect__III(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aStart,
        jint aEnd)
    {
        MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        TRAPD(
            error,
            CallMethodL(
                theListBox,
                &MSwtListBox::DeselectRangeL,
                aStart,
                aEnd));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1Deselect__I_3I(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jintArray aIndices)
    {
        TInt* ints = NULL;
        TInt count = JavaIntsToInts(aJniEnv, aIndices, ints);
        if (count == 0)
        {
            return;
        }

        MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        TRAPD(
            error,
            CallMethodL(
                theListBox,
                &MSwtListBox::DeselectItemsL,
                ints,
                count));
        delete[] ints;
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1DeselectAll(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        TRAPD(
            error,
            CallMethodL(
                theListBox,
                &MSwtListBox::DeselectAllL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1GetSelectionCount(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        const MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        TInt result;
        CallMethod(
            result,
            theListBox,
            &MSwtListBox::GetSelectionCount);
        return result;
    }

    JNIEXPORT jintArray JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1GetSelectionIndices(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        const MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        const CArrayFix<TInt>* selectionArray;
        CallMethod(
            selectionArray,
            theListBox,
            &MSwtListBox::GetSelectionIndices);
        TInt count = selectionArray ? selectionArray->Count() : 0;
        jintArray result = aJniEnv->NewIntArray(count);
        if (!result)
        {
            return NULL;
        }

        for (TInt i = 0; i < count; ++i)
        {
            TInt value = (*selectionArray)[i];   //lint !e613
            aJniEnv->SetIntArrayRegion(result, i, 1, &value);
        }
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1GetFocusIndex(
        JNIEnv *,
        jclass,
        jint aHandle)
    {
        const MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        TInt result = 0;
        CallMethod(
            result,
            theListBox,
            &MSwtListBox::GetFocusIndex);
        return result;
    }

    static void ListBox_RefreshItemL(JNIEnv* aJniEnv,
                                     MSwtListBox* aListBox,
                                     TInt aIndex,
                                     jobject aJavaItem)
    {
        CSwtListBoxItem* item = ConvertListBoxItemL(aJniEnv, aJavaItem);
        if (item)
        {
            CleanupStack::PushL(item);
        }
        // item can be also null
        CallMethodL(aListBox, &MSwtListBox::RefreshItemL, aIndex, item);
        if (item)
        {
            CleanupStack::PopAndDestroy(item);
        }
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1RefreshItem(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aIndex,
        jobject aJavaItem)
    {
        MSwtListBox* theListBox = reinterpret_cast< MSwtListBox* >(aHandle);
        TRAPD(
            error,
            ListBox_RefreshItemL(
                aJniEnv,
                theListBox,
                aIndex,
                aJavaItem));
        ThrowIfError(error, aJniEnv);
    }

// LeaveScan does not like method pointers in argument lists, this typedef
// helps preventing it from misreporting
    typedef void (MSwtListBox::*TListBoxSetModelMethod)(const RSwtListBoxItemsArray&);

    static void ListBox_ConvertModelAndCallMethodL(
        JNIEnv* aJniEnv,
        const jobjectArray aDataModelArray,
        MSwtListBox* aListBox,
        TListBoxSetModelMethod aMethodL)
    {
        TBool failed = EFalse;
        RSwtListBoxItemsArray array = ConvertListBoxItemArrayL(
                                          aJniEnv,
                                          aDataModelArray,
                                          failed);
        if (!failed)
        {
            CleanupResetAndDestroyPushL(array);
            CallMethodL(aListBox, aMethodL, array);
            CleanupStack::PopAndDestroy(); // array
        }
        else
        {
            array.ResetAndDestroy();
        }
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1RefreshList(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jobjectArray aDataModelArray)
    {
        ASSERT(aDataModelArray);
        MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        TRAPD(
            error,
            ListBox_ConvertModelAndCallMethodL(
                aJniEnv,
                aDataModelArray,
                theListBox,
                &MSwtListBox::RefreshListL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1Select__IIZ(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aIndex,
        jboolean aScroll)
    {
        MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        TRAPD(
            error,
            CallMethodL(
                theListBox,
                &MSwtListBox::SelectItemL,
                aIndex, aScroll));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1Select__III(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aStart,
        jint aEnd)
    {
        MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        TRAPD(
            error,
            CallMethodL(
                theListBox,
                &MSwtListBox::SelectRangeL,
                aStart,
                aEnd));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1SelectAll(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        TRAPD(
            error,
            CallMethodL(
                theListBox,
                &MSwtListBox::SelectAllL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1SetFocusIndex(JNIEnv*, jclass, jint aHandle, jint aIndex)
    {
        MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        CallMethod(theListBox, &MSwtListBox::SetFocusIndex, aIndex);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1SetDataModel(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jobjectArray aDataModelArray)
    {
        MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        TRAPD(
            error,
            ListBox_ConvertModelAndCallMethodL(
                aJniEnv,
                aDataModelArray,
                theListBox,
                &MSwtListBox::SetDataModelL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1SetHeadingFont(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aFontHandle)
    {
        MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        MSwtFont* font = reinterpret_cast<MSwtFont*>(aFontHandle);
        TRAPD(
            error,
            CallMethodL(
                theListBox,
                &MSwtListBox::SetHeadingFontL,
                font));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListBox_1ShowSelection(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtListBox* theListBox = reinterpret_cast<MSwtListBox*>(aHandle);
        CallMethod(theListBox, &MSwtListBox::ShowSelection);
    }


    /*
     * Class ListView
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1New(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jint aParent,
        jint aStyle)
    {
        jint handle = NewMobileWidget(aJniEnv, &MSwtFactory::NewListViewL, aPeer,
                                      aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, ListView);
        return handle;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1ScrollableHandle(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        MSwtScrollable*      result;
        CallMethod(result, theListView, &MSwtListView::Scrollable);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1Add__ILjava_lang_String_2I(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jstring aString,
        jint aIconHandle)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        MSwtImage* icon = reinterpret_cast<MSwtImage*>(aIconHandle);
        TRAPD(error,
              HBufC* convertedText = ConvertStringL(aJniEnv, aString);
              CleanupStack::PushL(convertedText);
              TPtr des(convertedText->Des());
              CallMethodL(theListView, &MSwtListView::AppendL, des, icon);
              CleanupStack::PopAndDestroy(convertedText);
             );
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1Add__ILjava_lang_String_2II(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jstring aString,
        jint aIconHandle,
        jint aIndex)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        MSwtImage* icon = reinterpret_cast<MSwtImage*>(aIconHandle);
        TRAPD(error,
              HBufC* convertedText = ConvertStringL(aJniEnv, aString);
              CleanupStack::PushL(convertedText);
              TPtr des(convertedText->Des());
              CallMethodL(theListView, &MSwtListView::InsertL, aIndex, des, icon);
              CleanupStack::PopAndDestroy(convertedText);
             );
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1Deselect__II(
        JNIEnv*,
        jclass,
        jint aHandle,
        jint aIndex)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        CallMethod(theListView, &MSwtListView::DeselectItem, aIndex);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1Deselect__III(
        JNIEnv*,
        jclass,
        jint aHandle,
        jint aStart,
        jint aEnd)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        CallMethod(theListView, &MSwtListView::DeselectRange, aStart, aEnd);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1Deselect__I_3I(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jintArray aIndices)
    {
        TInt* ints = NULL;
        TInt count = JavaIntsToInts(aJniEnv, aIndices, ints);
        if (count == 0)
            return;

        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        CallMethod(theListView, &MSwtListView::DeselectItems, ints, count);
        delete[] ints;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1DeselectAll(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        CallMethod(theListView, &MSwtListView::DeselectAll);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1GetFocusIndex(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        const MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TInt result = 0;
        CallMethod(result, theListView, &MSwtListView::GetFocusIndex);
        return result;
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1GetItem(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aIndex)
    {
        const MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TPtrC result;
        TRAPD(error, CallMethodL(theListView, &MSwtListView::GetItemL, aIndex, result));
        if (error == KErrNone)
        {
            return NewJavaString(aJniEnv, result);
        }
        else
        {
            Throw(error, aJniEnv);
            return 0;
        }
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1GetIconSize(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aStyle)
    {
        const MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TPoint result(TPoint::EUninitialized);
        CallMethod(result, theListView, &MSwtListView::GetIconSize, aStyle);
        return NewJavaPoint(aJniEnv, result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1GetItemCount(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        const MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TInt result = 0;
        CallMethod(result, theListView, &MSwtListView::GetItemCount);
        return result;
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1GetItems(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        const MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        CPtrCArray* array = NULL;
        TRAPD(error, CallMethodL(array, theListView, &MSwtListView::GetItemsL));
        if (error != KErrNone)
        {
            Throw(error, aJniEnv);
            DeleteInUiHeap(array);
            return NULL;
        }
        if (array == NULL)
        {
            return NULL;
        }

        // Construct the array of strings.
        jobjectArray javaStringArray = NULL;
        jclass stringClass = aJniEnv->FindClass("java/lang/String");
        if (stringClass != NULL)
        {
            const TInt count = array->Count();
            javaStringArray = aJniEnv->NewObjectArray(count, stringClass, NULL);
            if (javaStringArray != NULL)
            {
                jstring javaString;
                for (TInt i=0; i<count; ++i)
                {
                    javaString = NewJavaString(aJniEnv, array->At(i));
                    aJniEnv->SetObjectArrayElement(javaStringArray, i, javaString);   //lint !e613
                    aJniEnv->DeleteLocalRef(javaString);
                }
            }
            aJniEnv->DeleteLocalRef(stringClass);
        }

        DeleteInUiHeap(array);
        return javaStringArray;
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1GetSelection(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        const MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        CPtrCArray* selectedItemArray = NULL;
        TRAPD(error, CallMethodL(selectedItemArray, theListView, &MSwtListView::GetSelectionL));
        if (error!=KErrNone)
        {
            Throw(error, aJniEnv);
            DeleteInUiHeap(selectedItemArray);
            return NULL;
        }

        if (selectedItemArray==NULL)
        {
            return NULL;
        }

        jobjectArray javaItemArray = NULL;
        jclass       stringClass = aJniEnv->FindClass("java/lang/String");
        if (stringClass != NULL)
        {
            const TInt count = selectedItemArray->Count();
            javaItemArray = aJniEnv->NewObjectArray(count, stringClass, NULL);
            if (javaItemArray != NULL)
            {
                jstring javaString;
                for (TInt i=0; i<count; ++i)
                {
                    javaString = NewJavaString(aJniEnv, selectedItemArray->At(i));
                    aJniEnv->SetObjectArrayElement(javaItemArray, i, javaString);   //lint !e613
                    aJniEnv->DeleteLocalRef(javaString);
                }
            }
            aJniEnv->DeleteLocalRef(stringClass);
        }

        DeleteInUiHeap(selectedItemArray);
        return javaItemArray;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1GetSelectionCount(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        const MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TInt count;
        CallMethod(count, theListView, &MSwtListView::GetSelectionCount);
        return count;
    }

    JNIEXPORT jintArray JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1GetSelectionIndices(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        const MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        const CArrayFix<TInt>* selectionArray;
        CallMethod(selectionArray, theListView, &MSwtListView::GetSelectionIndices);
        TInt count = (selectionArray!=NULL) ? selectionArray->Count() : 0;
        jintArray result = aJniEnv->NewIntArray(count);
        if (!result)
        {
            return NULL;
        }

        for (TInt i=0; i<count; ++i)
        {
            TInt value = (*selectionArray)[i];   //lint !e613
            aJniEnv->SetIntArrayRegion(result, i, 1, &value);
        }
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1GetTopIndex(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        const MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TInt result;
        CallMethod(result, theListView, &MSwtListView::GetTopIndex);
        return result;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1IndexOf(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jstring aString,
        jint aStart)
    {
        HBufC* string = NULL;
        TRAPD(error, (string=ConvertStringL(aJniEnv, aString)));
        if (string == NULL)
        {
            Throw(error, aJniEnv);
            return 0;
        }

        const MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TInt result;
        CallMethod(result, theListView, &MSwtListView::IndexOf, *string, aStart);
        delete string;
        return result;
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1IsSelected(
        JNIEnv*,
        jclass,
        jint aHandle,
        jint aIndex)
    {
        const MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TBool result = EFalse;
        CallMethod(result, theListView, &MSwtListView::IsSelected, aIndex);
        return ConvertBoolean(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1Remove__II(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aIndex)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TRAPD(error, CallMethodL(theListView, &MSwtListView::RemoveItemL, aIndex));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1Remove__III(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aStart,
        jint aEnd)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TRAPD(error, CallMethodL(theListView, &MSwtListView::RemoveRangeL, aStart, aEnd));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1Remove__I_3I(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jintArray aIndices)
    {
        TInt* ints = NULL;
        TInt count = JavaIntsToInts(aJniEnv, aIndices, ints);
        if (count == 0)
            return;

        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TRAPD(error, CallMethodL(theListView, &MSwtListView::RemoveItemsL, ints, count));
        ThrowIfError(error, aJniEnv);
        delete[] ints;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1RemoveAll(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TRAPD(error, CallMethodL(theListView, &MSwtListView::RemoveAllL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1Select__IIZ(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aIndex,
        jboolean aScroll)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TRAPD(error, CallMethodL(theListView, &MSwtListView::SelectItemL, aIndex, aScroll));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1Select__III(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aStart,
        jint aEnd)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TRAPD(error, CallMethodL(theListView, &MSwtListView::SelectRangeL, aStart, aEnd));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1SelectAll(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TRAPD(error, CallMethodL(theListView, &MSwtListView::SelectAllL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1SetFocusIndex(JNIEnv*, jclass, jint aHandle, jint aIndex)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        CallMethod(theListView, &MSwtListView::SetFocusIndex, aIndex);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1SetItems(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jobjectArray aStringArray,
        jintArray aIconsHandles)
    {
        CDesCArray* resultArray = NULL;
        TRAPD(error, (resultArray=ConvertStringArrayL(aJniEnv, aStringArray)));
        if (error != KErrNone || resultArray == NULL)
        {
            delete resultArray;
            ThrowIfError(error, aJniEnv);
            return;
        }

        RPointerArray<MSwtImage> iconsHandles;
        if (aIconsHandles != NULL)
        {
            TInt count = (aJniEnv->GetArrayLength(aIconsHandles));

            jboolean isCopy;
            jint* elements = aJniEnv->GetIntArrayElements(aIconsHandles, &isCopy);
            if (elements != NULL)
            {
                TInt err = KErrNone;
                for (TInt i=0; i<count && !err; ++i)
                {
                    err = iconsHandles.Append(reinterpret_cast<MSwtImage*>(elements[i]));
                }
                // Cleanup
                aJniEnv->ReleaseIntArrayElements(aIconsHandles, elements, JNI_ABORT);

                if (err != KErrNone )
                {
                    ThrowIfError(err, aJniEnv);
                    return;
                }
            }
        }

        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TRAP(error, CallMethodL(theListView, &MSwtListView::SetItemsL, resultArray, iconsHandles));
        ThrowIfError(error, aJniEnv);
        delete resultArray;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1SetLayout(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aLayout)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TRAPD(error, CallMethodL(theListView, &MSwtListView::SetLayoutL, aLayout));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1SetLayoutDensity(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aDensity)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        TRAPD(error, CallMethodL(theListView, &MSwtListView::SetLayoutDensityL, aDensity));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_ListView_1ShowSelection(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtListView* theListView = reinterpret_cast<MSwtListView*>(aHandle);
        CallMethod(theListView, &MSwtListView::ShowSelection);
    }


    /*
     * Class MobileDevice
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MobileDevice_1New(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer)
    {
        CSwtDisplay& display = CSwtDisplay::Current();
        const MSwtFactory* factory = &display.Factory();
        jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
        if (peerRef == NULL)
            return 0;

        MSwtMobileDevice*  result = NULL;
        TRAPD(error, CallMethodL(result, factory, &MSwtFactory::NewMobileDeviceL,
                                 display, peerRef));
        if (error)
        {
            aJniEnv->DeleteWeakGlobalRef(peerRef);
            Throw(error, aJniEnv);
        }

        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MobileDevice_1Dispose(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        CSwtDisplay& display = CSwtDisplay::Current();
        MSwtMobileDevice* mobileDevice = reinterpret_cast<MSwtMobileDevice*>(aHandle);
        TSwtPeer          peerRef;
        CallMethod(peerRef, mobileDevice, &MSwtMobileDevice::Dispose);

        ReleasePeer(aJniEnv, peerRef);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MobileDevice_1IsOpen(
        JNIEnv* /*aJniEnv*/,
        jclass,
        jint aHandle)
    {
        const MSwtMobileDevice* mobileDevice = reinterpret_cast<MSwtMobileDevice*>(aHandle);

        TBool isOpen(EFalse);
        CallMethod(isOpen, mobileDevice, &MSwtMobileDevice::IsOpen);

        return ConvertBoolean(isOpen);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MobileDevice_1Alert(
        JNIEnv* /*aJniEnv*/,
        jclass,
        jint aHandle,
        jint aLevel)
    {
        MSwtMobileDevice* mobileDevice = reinterpret_cast<MSwtMobileDevice*>(aHandle);
        MSwtMobileDevice::TAlertLevel level = static_cast<MSwtMobileDevice::TAlertLevel>(aLevel);
        CallMethod(mobileDevice, &MSwtMobileDevice::Alert, level);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MobileDevice_1GetInputNumber(
        JNIEnv* /*aJniEnv*/,
        jclass,
        jint aHandle)
    {
        const MSwtMobileDevice* mobileDevice = reinterpret_cast<MSwtMobileDevice*>(aHandle);

        TInt nbInputs;
        CallMethod(nbInputs, mobileDevice, &MSwtMobileDevice::GetInputNumber);

        return nbInputs;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MobileDevice_1GetScreenNumber(
        JNIEnv* /*aJniEnv*/,
        jclass,
        jint aHandle)
    {
        const MSwtMobileDevice* mobileDevice = reinterpret_cast<MSwtMobileDevice*>(aHandle);

        TInt nbScreens;
        CallMethod(nbScreens, mobileDevice, &MSwtMobileDevice::GetScreenNumber);
        return nbScreens;
    }


    /*
     * Class MobileShell
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MobileShell_1New(
        JNIEnv* aJniEnv, jclass, jobject aPeer, jint aParent, jint aStyle)
    {
        CSwtDisplay& display = CSwtDisplay::Current();
        const MSwtFactory* factory = &display.Factory();
        jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
        if (!peerRef)
        {
            return 0;
        }

        MSwtShell* parent = reinterpret_cast<MSwtShell*>(aParent);
        MSwtMobileShell* result = NULL;
        TRAPD(error, CallMethodL(result, factory,
                                 &MSwtFactory::NewMobileShellL,
                                 display, peerRef, parent, aStyle));
        if (error)
        {
            aJniEnv->DeleteWeakGlobalRef(peerRef);
            Throw(error, aJniEnv);
        }

        INCREASE_INSTANCE_COUNT(result, MobileShell);

        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MobileShell_1ChangeTrim(
        JNIEnv* aJniEnv, jclass, jint aHandle, jint aStyle, jint aStatusStyle)
    {
        MSwtMobileShell* mobileShell = reinterpret_cast<MSwtMobileShell*>(aHandle);
        TRAPD(error, CallMethodL(mobileShell, &MSwtMobileShell::ChangeTrimL,
                                 aStyle, aStatusStyle));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MobileShell_1GetFullScreenMode(
        JNIEnv*, jclass, jint aHandle)
    {
        const MSwtMobileShell* mobileShell = reinterpret_cast<MSwtMobileShell*>(aHandle);
        const MSwtShell* shell = mobileShell->Shell();
        TBool isFullScreen(EFalse);
        CallMethod(isFullScreen, shell, &MSwtShell::FullScreenMode);
        return ConvertBoolean(isFullScreen);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MobileShell_1GetKeyState(
        JNIEnv*, jclass, jint aHandle, jint aSwtKeyCode)
    {
        MSwtMobileShell* mobileShell = reinterpret_cast<MSwtMobileShell*>(aHandle);
        TBool result = EFalse;
        CallMethod(result, mobileShell, &MSwtMobileShell::GetKeyState, aSwtKeyCode);
        return ConvertBoolean(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MobileShell_1GetShellHandle(
        JNIEnv*, jclass, jint aHandle)
    {
        const MSwtMobileShell* mobileShell = reinterpret_cast<MSwtMobileShell*>(aHandle);
        const MSwtShell* shell = NULL;
        CallMethod(shell, mobileShell, &MSwtMobileShell::Shell);
        return reinterpret_cast<jint>(shell);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MobileShell_1SetFullScreenMode(
        JNIEnv*, jclass, jint aHandle, jboolean aIsFullScreen)
    {
        MSwtMobileShell* mobileShell = reinterpret_cast<MSwtMobileShell*>(aHandle);
        MSwtShell* shell = const_cast<MSwtShell*>(mobileShell->Shell());
        CallMethod(shell, &MSwtShell::SetFullScreen, aIsFullScreen);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MobileShell_1SetStatusText(
        JNIEnv* aJniEnv, jclass, jint aHandle, jstring aText)
    {
        CallTextMethodThrow(aJniEnv, aHandle, &MSwtMobileShell::SetStatusTextL, aText);
    }


    /*
     * Class MultiPageDialog
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultipageDialog_1TabGroup_1ControlHandle(
        JNIEnv*,
        jclass,
        jint aTabGroupHandle)
    {
        MSwtMultiPageTabGroup* tabGroup = reinterpret_cast<MSwtMultiPageTabGroup*>(aTabGroupHandle);
        MSwtControl*         result;
        CallMethod(result, tabGroup, &MSwtMultiPageTabGroup::Control);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultipageDialog_1Pager_1New(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jint aParentHandle)
    {
        CSwtDisplay& display = CSwtDisplay::Current();
        const MSwtFactory* factory = &display.Factory();
        jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
        if (peerRef == NULL)
            return 0;

        MSwtShell*   parent  = reinterpret_cast<MSwtShell*>(aParentHandle);
        MSwtMultiPagePager* result  = NULL;
        TRAPD(error, CallMethodL(result, factory, &MSwtFactory::NewMultiPagePagerL,
                                 display, peerRef, *parent));
        if (error)
        {
            aJniEnv->DeleteWeakGlobalRef(peerRef);
            Throw(error, aJniEnv);
        }

        INCREASE_INSTANCE_COUNT(result, MultiPagePager);

        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1Pager_1CompositeHandle(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtMultiPagePager* pager = reinterpret_cast<MSwtMultiPagePager*>(aHandle);
        MSwtComposite* result;
        CallMethod(result, pager, &MSwtMultiPagePager::Composite);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jintArray JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1Pager_1GetDialogMargins(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        const MSwtMultiPagePager* pager = reinterpret_cast<MSwtMultiPagePager*>(aHandle);
        TMargins margins;
        CallMethod(margins, pager, &MSwtMultiPagePager::GetDialogMargins);

        TInt marginsArray[4];
        marginsArray[0] = margins.iLeft;
        marginsArray[1] = margins.iTop;
        marginsArray[2] = margins.iRight;
        marginsArray[3] = margins.iBottom;
        return NewJavaIntArray(aJniEnv, marginsArray, 4);
    }

    JNIEXPORT jintArray JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1Pager_1GetPagerMargins(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        const MSwtMultiPagePager* pager = reinterpret_cast<MSwtMultiPagePager*>(aHandle);
        TMargins margins;
        CallMethod(margins, pager, &MSwtMultiPagePager::GetPagerMargins);

        TInt marginsArray[4];
        marginsArray[0] = margins.iLeft;
        marginsArray[1] = margins.iTop;
        marginsArray[2] = margins.iRight;
        marginsArray[3] = margins.iBottom;
        return NewJavaIntArray(aJniEnv, marginsArray, 4);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultipageDialog_1TabGroup_1New(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jint aParentHandle)
    {
        CSwtDisplay&   display = CSwtDisplay::Current();
        const MSwtFactory* factory = &display.Factory();
        jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
        if (peerRef == NULL)
            return 0;

        MSwtShell* parent = reinterpret_cast<MSwtShell*>(aParentHandle);
        MSwtMultiPageTabGroup* result  = NULL;
        TRAPD(error, CallMethodL(result, factory, &MSwtFactory::NewMultiPageTabGroupL, display, peerRef, *parent));
        if (error)
        {
            aJniEnv->DeleteWeakGlobalRef(peerRef);
            Throw(error, aJniEnv);
        }

        INCREASE_INSTANCE_COUNT(result, MultiPageTabGroup);

        return reinterpret_cast<jint>(result);
    }

    static TInt MultiPageDialog_TabGroup_AddPageL(JNIEnv* aJniEnv,
            jint aTabGroupHandle,
            jstring aTitle,
            jint aImageHandle)
    {
        MSwtMultiPageTabGroup* tabGroup = reinterpret_cast<MSwtMultiPageTabGroup*>(aTabGroupHandle);
        MSwtImage* image = reinterpret_cast<MSwtImage*>(aImageHandle);
        TInt selectedIndex = 0;

        HBufC* title = ConvertStringLC(aJniEnv, aTitle);

        CallMethodL(selectedIndex, tabGroup, &MSwtMultiPageTabGroup::AddPageL, *title, image);
        CleanupStack::PopAndDestroy(title);

        return selectedIndex;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1TabGroup_1AddPage(
        JNIEnv* aJniEnv,
        jclass,
        jint aTabGroupHandle,
        jstring aTitle,
        jint aImageHandle)
    {
        TInt selectedIndex = 0;
        TRAPD(error, (selectedIndex = MultiPageDialog_TabGroup_AddPageL(aJniEnv, aTabGroupHandle, aTitle, aImageHandle)));
        ThrowIfError(error, aJniEnv);
        return selectedIndex;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultipageDialog_1TabGroup_1SetPager(
        JNIEnv*,
        jclass,
        jint aHandle,
        jint aPagerHandle)
    {
        MSwtMultiPageTabGroup* tabGroup = reinterpret_cast<MSwtMultiPageTabGroup*>(aHandle);
        MSwtMultiPagePager*    pager = reinterpret_cast<MSwtMultiPagePager*>(aPagerHandle);
        CallMethod(tabGroup, &MSwtMultiPageTabGroup::SetPager, pager);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1TabGroup_1SelectPage(
        JNIEnv*,
        jclass,
        jint aTabGroupHandle,
        jint aIndex)
    {
        MSwtMultiPageTabGroup* tabGroup = reinterpret_cast<MSwtMultiPageTabGroup*>(aTabGroupHandle);
        TInt selectedIndex = 0;
        CallMethod(selectedIndex, tabGroup, &MSwtMultiPageTabGroup::SelectPage, aIndex);
        return selectedIndex;
    }

    static void MultiPageDialog_TabGroup_SetTabTitleL(JNIEnv* aJniEnv,
            jint aHandle,
            jint aIndex,
            jstring aString)
    {
        MSwtMultiPageTabGroup* tabGroup = reinterpret_cast<MSwtMultiPageTabGroup*>(aHandle);
        HBufC* convertedText = ConvertStringLC(aJniEnv, aString);
        TPtr des(convertedText->Des());
        CallMethodL(tabGroup, &MSwtMultiPageTabGroup::SetTabTitleL, aIndex, des);
        CleanupStack::PopAndDestroy(convertedText);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultipageDialog_1TabGroup_1SetTabTitle(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aIndex,
        jstring aString)
    {
        TRAPD(error, MultiPageDialog_TabGroup_SetTabTitleL(aJniEnv, aHandle, aIndex, aString));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1TabGroup_1RemovePage(
        JNIEnv*,
        jclass,
        jint aTabGroupHandle,
        jint aIndex)
    {
        MSwtMultiPageTabGroup* tabGroup = reinterpret_cast<MSwtMultiPageTabGroup*>(aTabGroupHandle);
        TInt selectedIndex = 0;
        CallMethod(selectedIndex, tabGroup, &MSwtMultiPageTabGroup::RemovePage, aIndex);
        return selectedIndex;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1New(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jint aParentHandle)
    {
        CSwtDisplay& display = CSwtDisplay::Current();
        const MSwtFactory* factory = &display.Factory();
        jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
        if (peerRef == NULL)
            return 0;

        MSwtShell*   parent  = reinterpret_cast<MSwtShell*>(aParentHandle);
        MSwtMultiPageDialog* result  = NULL;
        TRAPD(error, CallMethodL(result, factory, &MSwtFactory::NewMultiPageDialogL,
                                 display, peerRef, *parent));
        if (error)
        {
            aJniEnv->DeleteWeakGlobalRef(peerRef);
            Throw(error, aJniEnv);
        }

        INCREASE_INSTANCE_COUNT(result, MultiPageDialog);

        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1ControlHandle(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtMultiPageDialog* theControl = reinterpret_cast<MSwtMultiPageDialog*>(aHandle);
        MSwtControl*      result;
        CallMethod(result, theControl, &MSwtMultiPageDialog::Control);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1Close(
        JNIEnv *,
        jclass,
        jint aHandle)
    {
        MSwtMultiPageDialog* dialog = reinterpret_cast<MSwtMultiPageDialog*>(aHandle);
        CallMethod(dialog, &MSwtMultiPageDialog::Close);
    }

    static void MultiPageDialog_CreatePageL(
        JNIEnv* aJniEnv,
        jint aHandle,
        jint aCompositeHandle,
        jstring aTitle,
        jint aImageHandle)
    {
        MSwtMultiPageDialog* dialog = reinterpret_cast<MSwtMultiPageDialog*>(aHandle);
        MSwtImage* image = reinterpret_cast<MSwtImage*>(aImageHandle);
        MSwtComposite* composite = reinterpret_cast<MSwtComposite*>(aCompositeHandle);
        HBufC* title = ConvertStringLC(aJniEnv, aTitle);
        CallMethodL(dialog, &MSwtMultiPageDialog::CreatePageL, composite, *title, image);
        CleanupStack::PopAndDestroy(title);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1CreatePage(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aCompositeHandle,
        jstring aTitle,
        jint aImageHandle)
    {
        TRAPD(error, MultiPageDialog_CreatePageL(aJniEnv, aHandle, aCompositeHandle, aTitle,
                aImageHandle));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1DeletePage(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aIndex)
    {
        MSwtMultiPageDialog* dialog = reinterpret_cast<MSwtMultiPageDialog*>(aHandle);
        TRAPD(error, CallMethodL(dialog, &MSwtMultiPageDialog::DeletePageL, aIndex));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1GetSelectedIndex(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtMultiPageDialog* dialog = reinterpret_cast<MSwtMultiPageDialog*>(aHandle);
        TInt index = 0;
        CallMethod(index, dialog, &MSwtMultiPageDialog::GetSelectedIndex);
        return index;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1Open(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtMultiPageDialog* dialog = reinterpret_cast<MSwtMultiPageDialog*>(aHandle);
        TRAPD(error, CallMethodL(dialog, &MSwtMultiPageDialog::OpenL));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1SetSelection(
        JNIEnv*,
        jclass,
        jint aHandle,
        jint aIndex)
    {
        MSwtMultiPageDialog* dialog = reinterpret_cast<MSwtMultiPageDialog*>(aHandle);
        CallMethod(dialog, &MSwtMultiPageDialog::SetSelection, aIndex);
    }

    static void MultiPageDialog_SetTitleAndImageL(
        JNIEnv* aJniEnv,
        jint aHandle,
        jint aIndex,
        jstring aTitle,
        jint aImageHandle)
    {
        MSwtMultiPageDialog* dialog = reinterpret_cast<MSwtMultiPageDialog*>(aHandle);
        MSwtImage* image = reinterpret_cast<MSwtImage*>(aImageHandle);
        HBufC* title = ConvertStringLC(aJniEnv, aTitle);
        CallMethodL(dialog, &MSwtMultiPageDialog::SetTitleAndImageL, aIndex, *title, image);
        CleanupStack::PopAndDestroy(title);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1SetTitleAndImage(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aIndex,
        jstring aTitle,
        jint aImageHandle)
    {
        TRAPD(error, MultiPageDialog_SetTitleAndImageL(aJniEnv, aHandle, aIndex, aTitle, aImageHandle));
        ThrowIfError(error, aJniEnv);
    }

    static void MultiPageDialog_SetTitleL(JNIEnv* aJniEnv, jint aHandle, jint aIndex, jstring aTitle)
    {
        MSwtMultiPageDialog* dialog = reinterpret_cast<MSwtMultiPageDialog*>(aHandle);
        HBufC* title = ConvertStringLC(aJniEnv, aTitle);
        CallMethodL(dialog, &MSwtMultiPageDialog::SetTitleL, aIndex, *title);
        CleanupStack::PopAndDestroy(title);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_MultiPageDialog_1SetTitle(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aIndex,
        jstring aTitle)
    {
        TRAPD(error, MultiPageDialog_SetTitleL(aJniEnv, aHandle, aIndex, aTitle));
        ThrowIfError(error, aJniEnv);
    }


    /*
     * Class QueryDialog
     */
    static void QueryDialog_OpenL(JNIEnv* aJniEnv,
                                  TSwtPeer aPeer,
                                  const jstring aTitle,
                                  TInt aStyle,
                                  TInt aQueryType,
                                  const jstring aPromptText,
                                  const jstring aDefaultValue,
                                  TInt aMinimum,
                                  TInt aMaximum)
    {
        CSwtDisplay& display = CSwtDisplay::Current();

        HBufC* title        = (aTitle       !=NULL) ? ConvertStringLC(aJniEnv,
                              aTitle)        : HBufC::NewLC(0);
        HBufC* promptText   = (aPromptText  !=NULL) ? ConvertStringLC(aJniEnv,
                              aPromptText)   : HBufC::NewLC(0);
        HBufC* defaultValue = (aDefaultValue!=NULL) ? ConvertStringLC(aJniEnv,
                              aDefaultValue) : HBufC::NewLC(0);


        display.RequestRunDialogL(aPeer , CSwtDialogBroker::EQueryDialog, aStyle, *title,
                                  *promptText, *defaultValue, aQueryType, aMinimum, aMaximum);

        CleanupStack::PopAndDestroy(3, title);   // defaultValue, promptText, title
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_QueryDialog_1Open(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jstring aTitle,
        jint aStyle,
        jint aQueryType,
        jstring aPromptText,
        jstring aDefaultValue,
        jint aMinimum,
        jint aMaximum)
    {
        TRAPD(error, QueryDialog_OpenL(aJniEnv, reinterpret_cast< TSwtPeer >(aPeer), aTitle, aStyle, aQueryType, aPromptText,
                                       aDefaultValue, aMinimum, aMaximum));
        ThrowIfError(error, aJniEnv);

    }

    static jstring QueryDialog_ConvertDefaultValueL(JNIEnv* aJniEnv, const jstring aString,
            TInt aQueryType)
    {
        CSwtDisplay&    display = CSwtDisplay::Current();
        const MSwtFactory* mobileFactory = &display.Factory();
        HBufC* string = (aString!=NULL) ? ConvertStringLC(aJniEnv, aString) : HBufC::NewLC(0);

        HBufC* result = NULL;
        CallMethodL(result, mobileFactory,
                    &MSwtFactory::RunQueryDialogConvertDefaultValueL, *string, aQueryType);

        jstring javaResult = NULL;
        if (result != NULL)
        {
            javaResult = NewJavaString(aJniEnv,*result);
            DeleteInUiHeap(result);
        }

        CleanupStack::PopAndDestroy(string);
        return javaResult;
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_QueryDialog_1ConvertDefaultValue(
        JNIEnv* aJniEnv,
        jclass,
        jstring aString,
        jint aQueryType)
    {
        jstring result = NULL;
        TRAPD(error, (result = QueryDialog_ConvertDefaultValueL(aJniEnv, aString, aQueryType)));
        ThrowIfError(error, aJniEnv);
        return result;
    }


    /*
     * Class Screen
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Screen_1New(
        JNIEnv* aJniEnv, jclass, jobject aPeer, jint aId)
    {
        CSwtDisplay& display = CSwtDisplay::Current();
        const MSwtFactory* mobileFactory = &display.Factory();
        jweak peerRef = aJniEnv->NewWeakGlobalRef(aPeer);
        if (peerRef == NULL)
        {
            return 0;
        }

        MSwtScreen* screen = NULL;
        TRAPD(error, CallMethodL(screen, mobileFactory,
                                 &MSwtFactory::NewScreenL, display, peerRef, aId));
        if (error)
        {
            aJniEnv->DeleteWeakGlobalRef(peerRef);
            Throw(error, aJniEnv);
        }

        INCREASE_INSTANCE_COUNT(screen, Screen);

        return reinterpret_cast<jint>(screen);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Screen_1Dispose(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        CSwtDisplay& display = CSwtDisplay::Current();
        MSwtScreen* screen = reinterpret_cast<MSwtScreen*>(aHandle);
        TSwtPeer  peerRef;
        CallMethod(peerRef, screen, &MSwtScreen::Dispose);

        ReleasePeer(aJniEnv, peerRef);

        DECREASE_INSTANCE_COUNT(Screen);
    }

    JNIEXPORT jobject JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Screen_1GetBounds(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtScreen* screen = reinterpret_cast<MSwtScreen*>(aHandle);
        TRect bounds(TRect::EUninitialized);
        CallMethod(bounds, screen, &MSwtScreen::GetBounds);
        return NewJavaRectangle(aJniEnv, bounds);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Screen_1GetColorDepth(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtScreen* screen = reinterpret_cast<MSwtScreen*>(aHandle);
        TInt colorDepth;
        CallMethod(colorDepth, screen, &MSwtScreen::GetColorDepth);
        return colorDepth;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Screen_1GetLocation(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtScreen* screen = reinterpret_cast<MSwtScreen*>(aHandle);

        TInt location;
        CallMethod(location, screen, &MSwtScreen::GetLocation);

        return location;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Screen_1GetOrientation(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtScreen* screen = reinterpret_cast<MSwtScreen*>(aHandle);
        TInt orientation;
        CallMethod(orientation, screen, &MSwtScreen::GetOrientation);
        return orientation;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Screen_1SetOrientation(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aOrientation)
    {
        MSwtScreen* screen = reinterpret_cast<MSwtScreen*>(aHandle);
        TRAPD(error, CallMethodL(screen, &MSwtScreen::SetOrientationL, aOrientation));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Screen_1GetUsage(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtScreen* screen = reinterpret_cast<MSwtScreen*>(aHandle);

        TInt usage;
        CallMethod(usage, screen, &MSwtScreen::GetUsage);

        return usage;
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Screen_1IsActive(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtScreen* screen = reinterpret_cast<MSwtScreen*>(aHandle);

        TBool isActive(EFalse);
        CallMethod(isActive, screen, &MSwtScreen::IsActive);

        return ConvertBoolean(isActive);
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_Screen_1IsTouchScreen(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtScreen* screen = reinterpret_cast<MSwtScreen*>(aHandle);
        TBool isTouchScreen;
        CallMethod(isTouchScreen, screen, &MSwtScreen::IsTouchScreen);
        return ConvertBoolean(isTouchScreen);
    }


    /*
     * Class SortedList
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_SortedList_1New(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jint aParent,
        jint aStyle)
    {
        jint handle = NewMobileWidget(aJniEnv, &MSwtFactory::NewSortedListL, aPeer,
                                      aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, SortedList);
        return handle;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_SortedList_1ScrollableHandle(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtSortedList* sortedList = reinterpret_cast<MSwtSortedList*>(aHandle);
        MSwtScrollable* result;
        CallMethod(result, sortedList, &MSwtSortedList::Scrollable);
        return reinterpret_cast<jint>(result);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_SortedList_1Add(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jstring aString)
    {
        /*MSwtSortedList* sortedList = reinterpret_cast<MSwtSortedList*>( aHandle );
        CallTextMethodThrow( aJniEnv, sortedList, &MSwtSortedList::AppendL, aString );
        */
        CallTextMethodThrow(aJniEnv, aHandle, &MSwtSortedList::AppendL, aString);
    }

    JNIEXPORT jstring JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_SortedList_1GetFocusedItem(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        const MSwtSortedList* sortedList = reinterpret_cast<MSwtSortedList*>(aHandle);
        TPtrC item;
        CallMethod(sortedList, &MSwtSortedList::GetFocusedItem, item);
        return (item.Length()==0) ? NULL : NewJavaString(aJniEnv, item);
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_SortedList_1GetItemHeight(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        const MSwtSortedList* sortedList = reinterpret_cast<MSwtSortedList*>(aHandle);
        TInt height;
        CallMethod(height, sortedList, &MSwtSortedList::GetItemHeight);
        return height;
    }

    JNIEXPORT jobjectArray JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_SortedList_1GetSelection(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        const MSwtSortedList* sortedList = reinterpret_cast<MSwtSortedList*>(aHandle);
        CPtrCArray* selectedItemArray = NULL;
        TRAPD(error, CallMethodL(selectedItemArray, sortedList, &MSwtSortedList::GetSelectionL));
        if (error!=KErrNone)
        {
            Throw(error, aJniEnv);
            DeleteInUiHeap(selectedItemArray);
            return NULL;
        }

        if (selectedItemArray==NULL)
        {
            return NULL;
        }

        jobjectArray javaItemArray = NULL;
        jclass       stringClass = aJniEnv->FindClass("java/lang/String");
        if (stringClass != NULL)
        {
            const TInt count = (selectedItemArray!=NULL) ? selectedItemArray->Count() : 0;
            javaItemArray = aJniEnv->NewObjectArray(count, stringClass, NULL);
            if (javaItemArray != NULL)
            {
                jstring javaString;
                for (TInt i=0; i<count; ++i)
                {
                    javaString = NewJavaString(aJniEnv, selectedItemArray->At(i));
                    aJniEnv->SetObjectArrayElement(javaItemArray, i, javaString);   //lint !e613
                    aJniEnv->DeleteLocalRef(javaString);
                }
            }
            aJniEnv->DeleteLocalRef(stringClass);
        }

        DeleteInUiHeap(selectedItemArray);
        return javaItemArray;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_SortedList_1GetSelectionCount(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        const MSwtSortedList* sortedList = reinterpret_cast<MSwtSortedList*>(aHandle);
        TInt count;
        CallMethod(count, sortedList, &MSwtSortedList::GetSelectionCount);
        return count;
    }

    static TBool SortedList_RemoveL(JNIEnv* aJniEnv, jint aHandle, jstring aString)
    {
        MSwtSortedList* sortedList = reinterpret_cast<MSwtSortedList*>(aHandle);
        HBufC* convertedText = ConvertStringLC(aJniEnv, aString);
        TBool removeError(ETrue);
        CallMethodL(removeError, sortedList, &MSwtSortedList::RemoveL, *convertedText);
        CleanupStack::PopAndDestroy(convertedText);
        return removeError;
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_SortedList_1Remove(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jstring aString)
    {
        TBool removeError(ETrue);
        TRAPD(error, (removeError=SortedList_RemoveL(aJniEnv, aHandle, aString)));
        ThrowIfError(error, aJniEnv);
        return ConvertBoolean(removeError);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_SortedList_1RemoveAll(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle)
    {
        MSwtSortedList* sortedList = reinterpret_cast<MSwtSortedList*>(aHandle);
        TRAPD(error, CallMethodL(sortedList, &MSwtSortedList::RemoveAllL));
        ThrowIfError(error, aJniEnv);
    }

    static TBool SortedList_SelectL(JNIEnv* aJniEnv, jint aHandle, jstring aString)
    {
        MSwtSortedList* sortedList = reinterpret_cast<MSwtSortedList*>(aHandle);
        HBufC* convertedText = NULL;
        if (aString != NULL)
        {
            convertedText = ConvertStringLC(aJniEnv, aString);
        }

        TBool selectError(ETrue);
        CallMethodL(selectError, sortedList, &MSwtSortedList::SelectItemL, convertedText);

        if (convertedText != NULL)
        {
            CleanupStack::PopAndDestroy(convertedText);
        }

        return selectError;
    }

    JNIEXPORT jboolean JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_SortedList_1Select(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jstring aString)
    {
        TBool selectError(ETrue);
        TRAPD(error, (selectError=SortedList_SelectL(aJniEnv, aHandle, aString)));
        ThrowIfError(error, aJniEnv);
        return ConvertBoolean(selectError);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_SortedList_1SetItems(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jobjectArray aStringArray)
    {
        CDesCArray* resultArray = NULL;
        TRAPD(error, (resultArray=ConvertStringArrayL(aJniEnv, aStringArray)));
        if (error != KErrNone || resultArray == NULL)
        {
            delete resultArray;
            ThrowIfError(error, aJniEnv);
            return;
        }

        MSwtSortedList* sortedList = reinterpret_cast<MSwtSortedList*>(aHandle);
        TRAP(error, CallMethodL(sortedList, &MSwtSortedList::SetItemsL, resultArray));
        ThrowIfError(error, aJniEnv);
        delete resultArray;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_SortedList_1SetModeStyle(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aModeStyle)
    {
        MSwtSortedList* sortedList = reinterpret_cast<MSwtSortedList*>(aHandle);
        TRAPD(error, CallMethodL(sortedList, &MSwtSortedList::SetModeStyleL, aModeStyle));
        ThrowIfError(error, aJniEnv);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_SortedList_1SetSelection(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jobjectArray aStringArray)
    {
        CDesCArray* resultArray = NULL;
        TRAPD(error, (resultArray=ConvertStringArrayL(aJniEnv, aStringArray)));
        if (error != KErrNone || resultArray == NULL)
        {
            delete resultArray;
            ThrowIfError(error, aJniEnv);
            return;
        }

        MSwtSortedList* sortedList = reinterpret_cast<MSwtSortedList*>(aHandle);
        TRAP(error, CallMethodL(sortedList, &MSwtSortedList::SetSelectionL, resultArray));
        ThrowIfError(error, aJniEnv);
        delete resultArray;
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_SortedList_1ShowSelection(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtSortedList* sortedList = reinterpret_cast<MSwtSortedList*>(aHandle);
        CallMethod(sortedList, &MSwtSortedList::ShowSelection);
    }


    /**
     * Class TaskTip
     */
    JNIEXPORT jobject JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_TaskTip_1DefaultBounds(
        JNIEnv* aJniEnv, jclass)
    {
        MSwtDisplay& display = CSwtDisplay::Current();
        TRect res(TRect::EUninitialized);
        CallMethod(res, &display.UiUtils(), &MSwtUiUtils::TaskTipRect);
        return NewJavaRectangle(aJniEnv, res);
    }


    /*
     * Class TextExtension
     */
    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_TextExtension_1New(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jint aParent,
        jint aStyle)
    {
        jint handle = NewMobileWidget(aJniEnv, &MSwtFactory::NewTextExtensionL,
                                      aPeer, aParent, aStyle);
        INCREASE_INSTANCE_COUNT(handle, TextExtension);
        return handle;
    }

    JNIEXPORT jint JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_TextExtension_1TextHandle(
        JNIEnv*,
        jclass,
        jint aHandle)
    {
        MSwtTextExtension* textExtension = reinterpret_cast<MSwtTextExtension*>(aHandle);
        MSwtText* result;
        CallMethod(result, textExtension, &MSwtTextExtension::Text);
        return reinterpret_cast<jint>(result);
    }

    static void TextExtension_SetTypeStyleL(TInt aHandle, TInt aStyle)
    {
        MSwtTextExtension* textExtension = reinterpret_cast<MSwtTextExtension*>(aHandle);
        CallMethodL(textExtension, &MSwtTextExtension::SetTypeStyleL, aStyle);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_TextExtension_1SetTypeStyle(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aStyle)
    {
        TRAPD(error, TextExtension_SetTypeStyleL(aHandle, aStyle));
        ThrowIfError(error, aJniEnv);
    }

    static void TextExtension_SetInitialInputModeL(
        JNIEnv* aJniEnv,
        TInt aHandle,
        TInt aCasingModifier,
        const jstring aMode)
    {
        HBufC* mode = ConvertStringLC(aJniEnv, aMode);

        MSwtTextExtension* textExtension = reinterpret_cast<MSwtTextExtension*>(aHandle);
        MSwtTextExtension::TCasingModifier modifier = static_cast<MSwtTextExtension::TCasingModifier>(
                    aCasingModifier);
        CallMethodL(textExtension, &MSwtTextExtension::SetInitialInputModeL, modifier, mode);

        if (mode != NULL)
            CleanupStack::PopAndDestroy(mode);
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_TextExtension_1SetInitialInputMode(
        JNIEnv* aJniEnv,
        jclass,
        jint aHandle,
        jint aCasingModifier,
        jstring aMode)
    {
        TRAPD(error, TextExtension_SetInitialInputModeL(aJniEnv, aHandle, aCasingModifier, aMode));
        ThrowIfError(error, aJniEnv);
    }


    /*
     * Class TimedMessageBox
     */
    static void TimedMessageBox_OpenL(JNIEnv* aJniEnv,
                                      TSwtPeer aPeer,
                                      const jstring aTitle,
                                      const jstring aMessage,
                                      TInt aStyle,
                                      TInt aImageHandle)
    {
        CSwtDisplay& display = CSwtDisplay::Current();

        HBufC* message = (aMessage!=NULL) ? ConvertStringLC(aJniEnv, aMessage) : HBufC::NewLC(0);
        HBufC* title   = (aTitle != NULL) ? ConvertStringLC(aJniEnv, aTitle)   : HBufC::NewLC(0);

        display.RequestRunDialogL(aPeer, CSwtDialogBroker::ETimedMessageBox, aStyle, *title, *message, KNullDesC(), aImageHandle);

        CleanupStack::PopAndDestroy(2, message);   // title, message
    }

    JNIEXPORT void JNICALL Java_org_eclipse_ercp_swt_mobile_internal_OS_TimedMessageBox_1Open(
        JNIEnv* aJniEnv,
        jclass,
        jobject aPeer,
        jstring aTitle,
        jstring aMessage,
        jint aStyle,
        jint aImageHandle)
    {
        TRAPD(error, TimedMessageBox_OpenL(aJniEnv, reinterpret_cast< TSwtPeer >(aPeer), aTitle, aMessage, aStyle, aImageHandle));
        ThrowIfError(error, aJniEnv);
    }

} // extern "C"
