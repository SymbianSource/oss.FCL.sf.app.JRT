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
* Description:  Contact list implementation
 *
*/


// PACKAGE
package com.nokia.mj.impl.pim;

// IMPORTS
import javax.microedition.pim.Contact;
import javax.microedition.pim.ContactList;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMItem;
import com.nokia.mj.impl.pim.ErrorString;

// CLASS DEFINITION
/**
 * Contact list implementation.
 */
public final class ContactListImpl extends PIMListImpl implements ContactList
{
    // Constructors

    ContactListImpl(int aNativeContactListHandle, int aMode)
    {
        super(aNativeContactListHandle, aMode);
    }

    // Methods from ContactList

    public synchronized Contact createContact()
    {
        return (ContactImpl) createItem();
    }

    public synchronized Contact importContact(Contact aContact)
    {
        if (aContact == null)
        {
            throw new NullPointerException(ErrorString.IMPORTING_FAILED_COLON +
                                           ErrorString.IMPORT_ITEM_IS_NULL);
        }
        ContactImpl contactItem = (ContactImpl) createContact();
        contactItem.copyFrom(aContact);

        // Set REVISION field
        updateRevisionField(contactItem, Contact.REVISION, PIMItem.ATTR_NONE);

        return contactItem;
    }

    public synchronized void removeContact(Contact aContact)
    throws PIMException
    {
        checkModeMinimum(PIM.WRITE_ONLY);
        removeItem(aContact);
    }

    // Methods from PIMListImpl

    int getListType()
    {
        return PIM.CONTACT_LIST;
    }

    protected PIMItemImpl createAssociatedItem(int aItemHandle)
    {
        return new ContactImpl(aItemHandle, this);
    }

}