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
* Description:  Contact item implementation.
 *
*/


// PACKAGE
package com.nokia.mj.impl.pim;

// IMPORTS
import javax.microedition.pim.Contact;
import javax.microedition.pim.PIM;
import com.nokia.mj.impl.pim.GenericException;
import com.nokia.mj.impl.pim.utils.NativeError;

// CLASS DEFINITION
/**
 * Contact item implementation.
 */
public final class ContactImpl extends PIMItemImpl implements Contact
{
    // Constructors

    /**
     * Creates new ContactImpl and associates it with given native peer object.
     */
    ContactImpl(int aItemHandle)
    {
        super(aItemHandle);
        iItemType = PIM.CONTACT_LIST;
    }

    /**
     * Creates new ContactImpl and associates it with given native peer object
     * and given ContactList.
     */
    ContactImpl(int aItemHandle, ContactListImpl aContactList)
    {
        super(aItemHandle, aContactList);
        iItemType = PIM.CONTACT_LIST;
    }

    /**
     * Creates new ContactImpl and its native side peer object. The new
     * ContactImpl object is not associated with a list.
     */
    ContactImpl()
    {
        super();

        iItemType = PIM.CONTACT_LIST;
        int[] error = new int[1];
        iItemHandle = _createNativePeer(

                          ((PIMManager) PIMManager.getInstance()).managerHandle(), error);

        if (!NativeError.checkSuccess(error[0]))
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + error[0]);
        }
    }

    //  Methods from PIMItemImpl

    /**
     * Provides the essential info about this Contact so that the user
     * can distinguish it among other Contacts.
     *
     * One of these values is returned (in preferred order):
     * @li Formatted name
     * @li Family Name and Given name separated with a space
     * @li Family Name only
     * @li Given Name only
     * @li Organization (Company)
     * @li \c null
     */
    String getShortInfo()
    {
        String info = null;

        // If the item does not belong to a list, all fields are supported;
        // otherwise we must consult the list for permission to avoid
        // unexpected UnsupportedFieldException.

        if (isFieldSupportedAndPresent(Contact.FORMATTED_NAME))
        {
            info = getString(Contact.FORMATTED_NAME, 0);
        }
        else if (isFieldSupportedAndPresent(Contact.NAME))
        {
            // An existing string array should never be completely empty.
            String[] name = getStringArray(Contact.NAME, 0);
            String familyName = name[ NAME_FAMILY ];
            String givenName = name[ NAME_GIVEN ];
            if (familyName != null)
            {
                info = familyName;
                if (givenName != null)
                {
                    info = info + " " + givenName;
                }
            }
            else if (givenName != null)
            {
                info = givenName;
            }
        }

        // The NAME string array (for some reason) did not have either
        // the NAME_FAMILY or NAME_GIVEN elements present, the info
        // might still be null at this point.

        if (info == null && isFieldSupportedAndPresent(Contact.ORG))
        {
            info = getString(Contact.ORG, 0);
        }

        // else leave info null

        return info;
    }

    // Methods from Contact
    public synchronized int getPreferredIndex(int aField)
    {
        int[] error = new int[1];

        int preferredIndex = _getPreferredIndex(iItemHandle, aField, error);

        NativeError.handleFieldHandlingError(error[0], aField);
        return preferredIndex;
    }

    // New methods

    /**
     * Copies the contents of given Contact-implementing object. New data is
     * appended to the existing data.
     */
    void copyFrom(Contact aContact)
    {
        baseCopyFrom(aContact);
    }

    // Native operations

    /**
     * @return Handle to the native side peer object or < 0 in error.
     */
    private native int _createNativePeer(int aManagerHandle, int[] aError);

    /**
     * Errors are reported through an int array because the operation may return
     * negative value.
     *
     * @return -1 for No preferred index set, >= 0 if set.
     */
    private native int _getPreferredIndex(int aContactItemHandle, int aField,
                                          int[] aError);
}
