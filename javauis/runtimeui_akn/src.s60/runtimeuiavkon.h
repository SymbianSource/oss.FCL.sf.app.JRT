/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef RUNTIMEUIAVKON_H
#define RUNTIMEUIAVKON_H

#include "javaosheaders.h"
#include <vector>

namespace java
{
namespace runtimeui
{

class ConfirmData
{
public:
    inline ConfirmData(const TDesC& aQuestion, std::vector<HBufC*> aAnswerOptions, int aAnswerSuggestion)
            : iQuestion(aQuestion), iAnswerOptions(aAnswerOptions), iAnswerSuggestion(aAnswerSuggestion), iAnswer(NO_ANSWER) {}

    // constant identifying that the answer is not available
    static const int NO_ANSWER = -1;

    // public members
    const TDesC& iQuestion;
    std::vector<HBufC*> iAnswerOptions;
    int iAnswerSuggestion;
    int iAnswer;
};

class RuntimeUiAvkon
{
public:
    OS_IMPORT static void error(const TDesC& aAppName, const TDesC& aShortMsg, const TDesC& aDetailedMsg);
    OS_IMPORT static bool confirm(const TDesC& aAppName, ConfirmData& aConfirmData, bool aIdentified);
};

} //end namespace runtimeui
} //end namespace java

#endif // RUNTIMEUIAVKON_H
