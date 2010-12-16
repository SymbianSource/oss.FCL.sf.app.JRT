/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implementation of CMIDUIManager
*
*/

#include <bldvariant.hrh>

// CDictionaryFileStore API to open ini file
#include <s32file.h>
#include <bautils.h>
#include <eikenv.h>
#include <eikappui.h>
#include <j2me/jdebug.h>

#include "CMIDUIManager.h"
// CMIDTickerController API to obtain instance
#include "CMIDTicker.h"
#include "CMIDDisplayable.h"
#include "CMIDCanvas.h"
#include "CMIDKeyDecoder.h"
#include "CMIDScaler.h"
#include "CMIDCanvasKeypad.h"
#include "CMIDDefaultBackground.h"

/** This macro is executed each time a trapped call returns an error code different than KErrNone */
#undef  TRAP_INSTRUMENTATION_LEAVE
#define TRAP_INSTRUMENTATION_LEAVE(aResult) DEBUG_INT2("In CMIDDisplayable.cpp, trapped method was called at line %D and got exception %D", __LINE__, aResult);

#ifdef CANVAS_ZOOM_SUPPORT
const TUid KUidZoomIndex={0x10003EE0};
#endif

CMIDUIManager* CMIDUIManager::NewL(MMIDEnv& aEnv)
{
    CMIDUIManager* uiManager = new(ELeave)CMIDUIManager(aEnv);
    CleanupStack::PushL(uiManager);
    uiManager->ConstructL();
    CleanupStack::Pop(uiManager);
    return uiManager;
}

void CMIDUIManager::ConstructL()
{
    iEnv.AddObserverL(*this);
    RestoreIniL();
    iDefDispDeactivated = EFalse;
}

CMIDUIManager::CMIDUIManager(MMIDEnv& aEnv)
        : iEnv(aEnv)
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDUIManager::~CMIDUIManager()
{
    CloseDefaultDisplayable();
    iEnv.RemoveObserver(*this);
    TRAP_IGNORE(StoreIniL());
    delete iMenuHandler;
    delete iNaviPaneController;
    delete iKeyDecoder;
    delete iScaler;
    delete iCanvasKeypad;
}

void CMIDUIManager::ZoomIn()
{
#ifdef CANVAS_ZOOM_SUPPORT
    TInt zoomState = iZoomState;
    if (++ zoomState > EZoomMax)
    {
        zoomState=EZoomMin;
    }
    SetZoomState(zoomState);
#endif
}

void CMIDUIManager::ZoomOut()
{
#ifdef CANVAS_ZOOM_SUPPORT
    TInt zoomState = iZoomState;
    if (-- zoomState < EZoomMin)
    {
        zoomState = EZoomMax;
    }
    SetZoomState(zoomState);
#endif
}

#ifdef CANVAS_ZOOM_SUPPORT
void CMIDUIManager::SetZoomState(TInt aZoomState)
{
    if (iZoomState != aZoomState)
    {
        iZoomState = aZoomState;

        MMIDDisplayable* displayable = iEnv.Current();
        MMIDComponent* content = (displayable ? displayable->Component() : NULL);
        if (content)
        {
            if (content->Type() == MMIDComponent::ECanvas)
            {
                CMIDCanvas* canvas = static_cast<CMIDCanvas*>(content);
                canvas->SetZoomState(iZoomState);
            }
        }
    }
}

/**
 *@return the current canvas zoom state.
 */
TInt CMIDUIManager::ZoomState() const
{
    return iZoomState;
}
#endif

void CMIDUIManager::StoreIniL()
{
#ifdef CANVAS_ZOOM_SUPPORT
    CDictionaryStore* iniFile = OpenIniFileLC(CCoeEnv::Static()->FsSession());
    RDictionaryWriteStream writeStream;
    writeStream.AssignLC(*iniFile,KUidZoomIndex);
    writeStream.WriteInt32L(iZoomState);
    writeStream.CommitL();
    iniFile->CommitL();
    CleanupStack::PopAndDestroy(2); //writeStream, iniFile
#endif
}

void CMIDUIManager::RestoreIniL()
{
#ifdef CANVAS_ZOOM_SUPPORT
    TInt zoomState;

    CDictionaryStore* iniFile=OpenIniFileLC(CCoeEnv::Static()->FsSession());
    if (!iniFile->IsNullL())
    {
        RDictionaryReadStream readStream;
        readStream.OpenLC(*iniFile,KUidZoomIndex);
        zoomState = readStream.ReadInt32L();
        CleanupStack::PopAndDestroy(); //readStream
    }
    else
    {
        zoomState = EZoomMin;
    }
    CleanupStack::PopAndDestroy(iniFile);

    SetZoomState(zoomState);
#endif
}

//
// Opens the ini file if it exists, otherwise creates a new one
//
CDictionaryStore* CMIDUIManager::OpenIniFileLC(RFs& aFs) const
{
    TBuf<KMaxFileName> iniName;
    iniName.Append(iEnv.MidletHome());
    _LIT(KSeparator,"\\");
    iniName.Append(KSeparator);
    iniName.Append(iEnv.MidletName());
    _LIT(KIniExtension,".ini");
    iniName.Append(KIniExtension);
    //
    // ensure that all directories in the path exist
    //
    aFs.MkDirAll(iniName); // ignore the error
    //
    CDictionaryStore* inifile=NULL;
    //
    // if first attempt to open ini fails because of corrupt file, delete it and try again.
    //
    TRAPD(err,inifile=CDictionaryFileStore::OpenL(aFs,iniName,iEnv.MidletUid()));
    if (err==KErrNone)
        CleanupStack::PushL(inifile);
    else if (err==KErrEof || err==KErrCorrupt)
    {
        User::LeaveIfError(aFs.Delete(iniName));
        inifile=CDictionaryFileStore::OpenLC(aFs,iniName,iEnv.MidletUid());
        err=KErrNone;
    }
    User::LeaveIfError(err);
    return inifile;
}

void CMIDUIManager::HandleResourceChangeL(TInt /*aType*/)
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDUIManager::HandleSwitchOnL(TBool aSwitchOn)
{
    OpenNaviPaneControllerL()->ShowTickerL(aSwitchOn);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMIDUIManager::HandleForegroundL(TBool aForeground)
{
    if (iNaviPaneController)
    {
        GetNaviPaneController()->ShowTickerL(aForeground);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMIDMenuHandler* CMIDUIManager::OpenMenuHandlerL()
{
    if (!iMenuHandler)
    {
        iMenuHandler = CMIDMenuHandler::NewL(iEnv);
    }
    return iMenuHandler;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMIDMenuHandler* CMIDUIManager::GetMenuHandler()
{
    return iMenuHandler;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMIDNaviPaneController* CMIDUIManager::OpenNaviPaneControllerL()
{
    if (!iNaviPaneController)
    {
        iNaviPaneController = CMIDNaviPaneController::NewL();
    }
    return iNaviPaneController;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMIDNaviPaneController* CMIDUIManager::GetNaviPaneController()
{
    return iNaviPaneController;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMIDKeyDecoder* CMIDUIManager::OpenKeyDecoderL()
{

    if (!iKeyDecoder)
    {
        iKeyDecoder = CMIDKeyDecoder::NewL(iEnv);
    }
    return iKeyDecoder;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMIDKeyDecoder* CMIDUIManager::GetKeyDecoder()
{
    return iKeyDecoder;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMIDScaler* CMIDUIManager::OpenScalerL()
{
    if (!iScaler)
    {
        iScaler = CMIDScaler::NewL();
    }
    iScaler->iRefC++;
    return iScaler;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMIDScaler* CMIDUIManager::GetScaler(TBool aIsOwner)
{
    if (iScaler && aIsOwner)
    {
        iScaler->iRefC++;
    }
    return iScaler;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMIDUIManager::CloseScaler(CMIDScaler* /*aScaler*/)
{
    if (iScaler)
    {
        if (iScaler->iRefC > 0)
        {
            iScaler->iRefC--;
        }

        if (iScaler->iRefC <= 0)
        {
            delete iScaler;
            iScaler = NULL;
        }
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMIDCanvasKeypad* CMIDUIManager::OpenCanvasKeypadL(MMIDDisplayable* aDisplayable)
{
    if (aDisplayable)
    {
        if (!iCanvasKeypad)
        {
            iCanvasKeypad = CMIDCanvasKeypad::NewL(aDisplayable);
        }
        iCanvasKeypad->iRefC++;
    }
    return iCanvasKeypad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMIDCanvasKeypad* CMIDUIManager::GetCanvasKeypad(TBool aIsOwner)
{
    if (iCanvasKeypad && aIsOwner)
    {
        iCanvasKeypad->iRefC++;
    }
    return iCanvasKeypad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMIDUIManager::CloseCanvasKeypad(CMIDCanvasKeypad* aCanvasKeypad)
{
    if (iCanvasKeypad && iCanvasKeypad == aCanvasKeypad)
    {
        if (iCanvasKeypad->iRefC > 0)
        {
            iCanvasKeypad->iRefC--;
        }

        if (iCanvasKeypad->iRefC <= 0)
        {
            delete iCanvasKeypad;
            iCanvasKeypad = NULL;
        }
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
// parameter aVisible is for DafaultDisplayable visibility.
CMIDDisplayable* CMIDUIManager::OpenDefaultDisplayableL(TBool aVisible)
{
    if (!iDefDispDeactivated)
    {
        if (!iDefaultDisplayable)
        {
            // When a default displayable is not create, then it will be created.
            iDefaultDisplayable = CMIDDisplayable::NewL(iEnv, *this);
            CCoeControl& window = static_cast< CMIDDisplayable& >(*iDefaultDisplayable).ContentWindow();
            iDefaultBackground = CMIDDefaultBackground::NewL(window);
            iDefaultDisplayable->SetComponentL(*iDefaultBackground);
            // call HandleCurrent with TRUE for store pointer of Displayable
            iDefaultDisplayable->HandleCurrentL(ETrue);
            // Default displayable is not currnet in this moment
            iDefaultDisplayable->HandleCurrentL(EFalse);
            iDefaultDisplayable->MakeVisible(aVisible);
            CEikonEnv::Static()->EikAppUi()->RemoveFromStack(iDefaultBackground);
        }
        else if (aVisible)
        {
            // This makes active default displayable visible.
            iDefaultDisplayable->MakeVisible(ETrue);
        }
    }
    return iDefaultDisplayable;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMIDDisplayable* CMIDUIManager::GetDefaultDisplayable()
{
    return iDefaultDisplayable;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMIDUIManager::CloseDefaultDisplayable()
{
    if (iDefaultDisplayable)
    {
        // HandleCurrentL cannot leave when called with argument EFalse,
        // but because of naming convention we cannot trust that is the
        // case in the future so trap it anyway.
        TInt err = 0;
        DEBUG("CMIDMenuHandler::RemoveDefaultDisplayable - calling iDefaultDisplayable->HandleCurrentL");
        TRAP(err, iDefaultDisplayable->HandleCurrentL(EFalse));
        if (err != KErrNone)
        {
            DEBUG_INT("CMIDMenuHandler::RemoveDefaultDisplayable - Exception from HandleCurrentL. Error = %d", err);
        }

        delete iDefaultBackground;
        iDefaultBackground = NULL;

        iDefaultDisplayable->Dispose();
        iDefaultDisplayable = NULL;
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TSize CMIDUIManager::ReadPointerEventSuppressorValues()
{
    TSize values;
    TPtrC suppressorValues;
    //Read JAD attribute value
    if (iEnv.MidletAttribute(LcduiMidletAttributes::KAttribPointerEventSuppressorValues,
                             suppressorValues) == KErrNone)
    {
        //JAD attribute found.
        //Attributes are parsed to TSize format + validity check performed.
        //TSize is used as return value for ease of use.
        //TSize.iWidth=first value, TSize.iHeight=second value
        values = TwoDesValuesToTSize(suppressorValues);
    }
    else
    {
        //No JAD attribute found. Error value returned->Use default values.
        values = TSize(KPESErrorValue, KPESErrorValue);
    }

    return values;
}

// ---------------------------------------------------------------------------
// CMIDUIManager::TwoDesValuesToTSize
// Utility to parse TPtrC to TSize (two values separated with comma).
// @param TPtrC
// @return TSize
// ---------------------------------------------------------------------------
//
TSize CMIDUIManager::TwoDesValuesToTSize(TPtrC values)
{
    TSize newValues;
    TSize errorValues = TSize(KPESErrorValue, KPESErrorValue);
    TInt newIntValue1;
    TInt newIntValue2;
    // Assumed max. value is 9999.
    TBuf<KPESParameterLength> value1;
    // Assumed max. value is 9999.
    TBuf<KPESParameterLength> value2;
    TChar tmpChar;
    _LIT(lComma, ",");
    TChar comma(',');

    // value1 and value2 must be divided by comma
    if (values.Find(lComma) != KErrNotFound)
    {
        TLex16 lex(values);
        while (lex.Peek() != comma)
        {
            tmpChar = lex.Get();
            if (tmpChar.IsDigit() && value1.Length() < KPESParameterLength)
            {
                value1.Append(tmpChar);
            }
            else if (!tmpChar.IsSpace())
            {
                // If character is not digit or space
                return errorValues;
            }
        }
        lex.Get();
        while (!lex.Eos())
        {
            tmpChar = lex.Get();
            if (tmpChar.IsDigit() && value2.Length() < KPESParameterLength)
            {
                value2.Append(tmpChar);
            }
            else if (!tmpChar.IsSpace())
            {
                // If character is not digit or space
                return errorValues;
            }
        }

        // Width and height must have at least one digit each
        if (value1.Length() > 0 && value2.Length() > 0)
        {
            TLex16 widthLex(value1);
            widthLex.Val(newIntValue1);
            TLex16 heightLex(value2);
            heightLex.Val(newIntValue2);
            newValues.SetSize(TInt(newIntValue1), TInt(newIntValue2));
            return newValues;
        }
    }
    // If there is no comma between the digits or there is no digits in value1 or value2
    return errorValues;
}

// End of File
