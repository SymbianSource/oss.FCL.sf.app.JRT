/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTKEYMAPPER_H
#define SWTKEYMAPPER_H


#include "eswt.h"

#ifdef RD_INTELLIGENT_TEXT_INPUT
#include "PtiEngine.h"
#include <centralrepository.h>
#include "swtstickykeyshandler.h"

#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
#include "javaptivariation.h"
#endif //RD_JAVA_S60_RELEASE_5_0_IAD

#endif //RD_INTELLIGENT_TEXT_INPUT



/**
 * CSwtKeyMapper
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtKeyMapper)
        : public CBase
{
// Methods
public:
    static CSwtKeyMapper* NewL();
    ~CSwtKeyMapper();
    TSwtKeyEventData MapKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    static void GetSymbianCodes(TInt aSwtKeyCode, TInt& aSymbianScanCode,
                                TUint* aSymbianKeyCode = NULL);
    static TInt GetSymbianScanCode(TInt aSwtKeyCode);
    static TInt GetSwtKeyCode(const TKeyEvent& aKeyEvent);
    static TInt GetSwtStateMask(const TKeyEvent& aKeyEvent);
    static TUint GetSymbianModifier(const TSwtKeyEventData& aData);

private:
    inline CSwtKeyMapper();
    void ConstructL();

    static inline TInt MappingTableSize();
    static TBool IsRepeatKey(TInt aScanCode);

#ifdef RD_INTELLIGENT_TEXT_INPUT
    TBool RemapKeyCodeByPtiEngineL(
        TKeyEvent& aKeyEvent,
        TEventCode aType,
        TBool& aIsModifierHandledAsAKey);
    static TBool IsHalfQwertyKeyboard();

#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
    void CallToJavaPtiVariationL(TInt aType);
#endif //RD_JAVA_S60_RELEASE_5_0_IAD

#endif //RD_INTELLIGENT_TEXT_INPUT


// Data types
private:
    // Structure for mapping a Symbian scan code to an SWT key code
    struct TScanCodeMapping
    {
        TInt iScanCode; // Symbian TStdScanCode
        TInt iKeyCode;  // Symbian TKeyCode
        TInt iSwtCode;  // SWT key code
    };

    struct TKeyMapping
    {
        TInt  iScanCode;
        TInt  iKeyCode;
        TChar iCharacter;
        TInt  iCount;

        TKeyMapping()
        {}

        TKeyMapping(TInt aScanCode)
                : iScanCode(aScanCode)
                , iKeyCode(0)
                , iCharacter(0)
                , iCount(0)
        {}
    };

    typedef CArrayFixFlat<TKeyMapping> CKeyMappings;

// Static data
private:
    static const TScanCodeMapping KScanCodeTable[];

// Data
private:
    // Mappings between a scan code and a character, owned, cannot be NULL
    CKeyMappings* iKeyMappings;

#ifdef RD_INTELLIGENT_TEXT_INPUT
    CPtiEngine *iPtiEngine;
    CRepository *iRepository;
    CSwtStickyKeysHandler *iSwtStickyKeysHandler;

#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
    RLibrary iPtiSupportLib;
#endif //RD_JAVA_S60_RELEASE_5_0_IAD

#endif //RD_INTELLIGENT_TEXT_INPUT

};



#endif // SWTKEYMAPPER_H
