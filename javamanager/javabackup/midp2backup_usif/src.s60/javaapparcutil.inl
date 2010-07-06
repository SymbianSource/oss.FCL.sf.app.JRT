/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CJavaApparcUtil inline functions definition
*
*/


using namespace java::backup;

inline void CJavaApparcUtil::AppName(const TUid aAppUid, TDes& aAppName)
{
    _LIT(KPathSeperator, "\\");
    _LIT(KAppPostfix, ".fakeapp");
    aAppName.Copy(KPathSeperator);
    aAppName.AppendNum(aAppUid.iUid);
    aAppName.Append(KAppPostfix);
}

