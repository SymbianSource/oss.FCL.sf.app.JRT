/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


package javax.microedition.pki;

public interface Certificate
{
    public String getSubject();

    public String getIssuer();

    public String getType();

    public String getVersion();

    public String getSigAlgName();

    public long getNotBefore();

    public long getNotAfter();

    public String getSerialNumber();
}

