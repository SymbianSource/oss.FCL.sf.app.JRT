/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "javauseragent.h"
#ifdef RD_JAVA_S60_RELEASE_10_1_ONWARDS
#include <QWebpage.h>
#include "javaenvinfo.h"
#else
#include <cuseragent.h>
#endif

#ifdef RD_JAVA_S60_RELEASE_10_1_ONWARDS
class Page: public QWebPage
{
public:
    QString  userAgentForUrl ( const QUrl & url ) const;

};

QString  Page::userAgentForUrl ( const QUrl & url ) const
{
    return url.toString();
}
#endif

HBufC*  JavaUserAgent::GetUserAgentL()
{
#ifdef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    /*QApplication app();
    QUrl uri;
    Page page;
    QString webUserAgent = page.userAgentForUrl(uri);
    HBufC* agent = HBufC::NewMaxLC(webUserAgent.length());
    agent->Des().Copy(webUserAgent.utf16());
    agent->Des().Append(*(Java::JavaEnvInfo::GetUserAgentHeaderL()));
    return agent;
    */
    _LIT(KString,"null");
    HBufC* agent = HBufC::NewLC(5);
    *agent = KString;
    CleanupStack::Pop(agent);
    return agent;

#else

    CUserAgent* userAgent = CUserAgent::NewL();
    CleanupStack::PushL(userAgent);

    HBufC8* agent8 = userAgent->UserAgentL();
    CleanupStack::PushL(agent8);

    HBufC* agent = HBufC::NewMaxLC(agent8->Length());
    agent->Des().Copy(*agent8);

    CleanupStack::Pop(agent);
    CleanupStack::PopAndDestroy(agent8);
    CleanupStack::PopAndDestroy(userAgent);
   	return agent;
#endif
}
