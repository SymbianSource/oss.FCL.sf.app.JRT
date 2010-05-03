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
* Description:  Contact list interface.
 *
*/


// PACKAGE
package javax.microedition.pim;

// CLASS DEFINITION
/**
 * PIM API 1.0 compliant ContactList interface.
 */
public interface ContactList extends PIMList
{
    public Contact createContact();

    public Contact importContact(Contact aContact);

    public void removeContact(Contact aContact) throws PIMException;
}
