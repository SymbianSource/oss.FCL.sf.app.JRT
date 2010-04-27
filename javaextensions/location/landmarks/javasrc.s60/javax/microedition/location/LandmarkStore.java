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
* Description:  JSR-179 Location API version 1.0.1 LandmarkStore class
 *
*/


// PACKAGE
package javax.microedition.location;

// IMPORTS
import java.util.Enumeration;
import java.util.Vector;
import java.io.IOException;

import com.nokia.mj.impl.location.LAPIManager;
import com.nokia.mj.impl.location.Locator;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.location.NativeError;
import com.nokia.mj.impl.location.LocationPermission;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.location.LandmarkStoreManager;
import com.nokia.mj.impl.utils.Logger;

// CLASS DESCRIPTION
/**
 * JSR-179 Location API version 1.0.1 LandmarkStore class
 *
 * Please refer JSR-179 spec
 *
 */
public class LandmarkStore
{

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javalocation");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaLocation,
                        "Unable to load javalocation dll.");
        }

    }

    // Error messages
    // Store name argument is null
    private static final String LANDMARK_STORE_NAME_IS_NULL_ERROR
    = "Landmark store name is null";

    // General error when listing categories
    private static final String UNABLE_TO_LIST_CATEGORIES_ERROR
    = "Unable to list categories";

    // Category argument is null
    private static final String CATEGORY_NAME_IS_NULL_ERROR
    = "Category name is null";

    // Adding new categories is not supported
    private static final String ADDING_CATEGORIES_NOT_SUPPORTED_ERROR
    = "Adding new categories to this store is not supported";

    // Deleting categories is not supported
    private static final String DELETING_CATEGORIES_NOT_SUPPORTED_ERROR
    = "Deleting categories from this store is not supported";

    // Store has been closed
    private static final String LANDMARK_STORE_IS_CLOSED_ERROR
    = "Landmark store is closed";

    // Landmark object passed to the method is null
    private static final String LANDMARK_NAME_IS_NULL
    = "Landmark name is null";

    // Client has asked to delete a landmark which does not belong to this store
    private static final String LANDMARK_DOES_NOT_BELONG_TO_STORE
    = "Landmark does not belong to this store";

    // Client has specified incorrect search area
    private static final String INCORRECT_SEARCH_AREA_ARGUMENTS
    = "Area arguments are not valid: ";

    private static final String NON_SUPPORTED_UNICODE_CHARACTER
    = "Non supported unicode characters are not allowed: ";

    private static final String LANDMARK_NAME_LENGTH_EXCEEDS
    = "Landmark name length exceeds the supported length";

    private static final String CREATE_LANDMARK_STORE_ERROR
    = "Landmark store name is too long or a landmark store with the specified name already exists";

    private static final String ADD_CATEGORY_ARGUMENT_ERROR
    = "Category name already exists or it is illegal";

    private static final String CREATE_LANDMARK_STORE_OPERATION_ERROR
    = "unable to create landmark store: ";

    private static final String DELETE_LANDMARK_ERROR
    = "Unable to delete landmark: ";

    private static final String ADD_LANDMARK_TO_CATEGORY_ERROR
    = "Unable to add landmark to category: ";

    private static final String REMOVE_LANDMARK_FROM_CATEGORY_ERROR
    = "Unable to remove landmark from category: ";

    private static final String UPDATE_LANDMARK_ERROR
    = "Unable to update landmark: ";

    private static final String DELETE_LANDMARK_STORE_ERROR
    = "Unable to delete landmark store: ";

    private static final String ADD_CATEGORY_TO_LANDMARK_STORE_ERROR
    = "Unable to add category to this landmark store: ";

    private static final String DELETE_CATEGORY_FROM_LANDMARK_STORE_ERROR
    = "Unable to delete category from this landmark store: ";

    private static final String OBTAIN_LANDMARK_ERROR
    = "Unable to obtain landmarks: ";

    // These error code is not defined in NativeError class
    private static final int KErrAlreadyExists = -11;

    private static final int KErrSessionClosed = -45;

    // Landmark Name Length
    private static final int KLandmarkNameLength = 255;

    // Handle to the native side peer object
    private int iStoreHandle;

    // Handle to the Location API native event source
    private int iEventSourceHandle;

    // The URI of the store. Globally unique
    private String iStoreUri;

    private Finalizer mFinalizer;

    /**
     * Hidden constructor. The constructor is not part of the Location API so it
     * must be hidden. This is used internally to create a new landmark store
     * instance by LandmarkStoreManager
     *
     * @param aStoreHandle
     *            Handle to the native store
     */
    LandmarkStore(int aStoreHandle, String aStoreUri)
    {
        // Make a globally unique uri which can be used to identify this store
        iStoreUri = aStoreUri.toLowerCase().intern();
        // Register this class for finalization
        mFinalizer = registerForFinalization();
        iStoreHandle = aStoreHandle;
        // Get Function source handle for this landmark store
        iEventSourceHandle = LAPIManager.getInstance().getFunctionSourceHandle();
    }

    Finalizer registerForFinalization()
    {
        return new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };

    }

    /**
     * Disposes the LandmarkStore native peer, if the handles are valid. Invalid
     * (negative) handles indicate that their creation failed in the first
     * place.
     */
    final void doFinalize()
    {
        // Remove the native store from the landmark store manager
        LandmarkStoreManager.getInstance().removeStore(iStoreUri, iStoreHandle);
        // The object does not belong to any other object so it can be disposed
        _dispose(iEventSourceHandle, iStoreHandle);
    }

    /**
     * Please refer JSR-179 spec
     */
    public static LandmarkStore getInstance(String aStoreName)
    {
        String arguments[] = { "" };
        // Ensure landmark read permission from Java security
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        LocationPermission permission = new LocationPermission("landmarks://*",
                "read");
        appUtils.checkPermission(permission);

        // Create a locator which points where the database is. null opens
        // the default landmark store
        Locator locator = null;
        if (aStoreName != null)
        {
            locator = Locator.createLocator(aStoreName);
        }

        // Try get the store from the landmark store manager. If null is
        // returned,
        // it indicates that the store has not been opened yet. This quarantees
        // that only one instance from each store can be opened at a time
        LandmarkStore store = LandmarkStoreManager.getInstance().getStore(
                                  locator);

        // Store was null so try to open it
        if (store == null)
        {
            // Get handle for the native peer object from the Landmark store
            // manager
            // Note that this class takes the ownership of the native database
            int handle = LandmarkStoreManager.getInstance().openStore(locator);
            if (handle <= NativeError.KErrNone)
            {
                // The requested landmark store was not found or the
                // arguments were incorrect, so return null
                return null;
            }

            String storeUri = (aStoreName == null ? "" : locator.getStoreURI());
            // Create a new instance from the native peer handle
            store = new LandmarkStore(handle, storeUri);
            // Register the opened store to the manager
            LandmarkStoreManager.getInstance().registerStore(store, storeUri);
        }

        return store;
    }

    /**
     * Creates a new landmark store with a specified name. The newly created
     * landmark store does not have any landmarks or categories.
     *
     * @param aStoreName
     *            The name of the store which will be created
     */
    public static void createLandmarkStore(String aStoreName)
    throws IOException, LandmarkException
    {
        String arguments[] = { "" };
        // Ensure landmark management permission from Java security
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        LocationPermission permission = new LocationPermission("landmarks://*",
                "management");
        appUtils.checkPermission(permission);
        checkNullArgument(aStoreName, LANDMARK_STORE_NAME_IS_NULL_ERROR);

        // All unicode characters are supported except the below the below list.
        /*
         * •0x0000...0x001F control characters •0x005C '\' •0x002F '/' •0x003A
         * ':' •0x002A '*' •0x003F '?' •0x0022 '“' •0x003C '<' •0x003E '>'
         * •0x007C '|' •0x007F...0x009F control characters •0xFEFF
         * Byte-order-mark •0xFFF0...0xFFFF
         */

        // 0x0000...0x001F control characters are not allowed
        for (int index = 0x0000; index <= 0x001F; index++)
        {
            if (aStoreName.indexOf(index) > 0)
            {
                throw new IllegalArgumentException(
                    NON_SUPPORTED_UNICODE_CHARACTER + aStoreName);
            }
        }

        // 0x005C is equivalent ascii value for character '\'
        if ((aStoreName.indexOf(0x005C) > 0) || (aStoreName.indexOf('/') > 0)
                || (aStoreName.indexOf(':') > 0)
                || (aStoreName.indexOf('*') > 0)
                || (aStoreName.indexOf('?') > 0)
                || (aStoreName.indexOf('"') > 0)
                || (aStoreName.indexOf('<') > 0)
                || (aStoreName.indexOf('>') > 0)
                || (aStoreName.indexOf('|') > 0))
        {
            throw new IllegalArgumentException(
                NON_SUPPORTED_UNICODE_CHARACTER + aStoreName);
        }

        // 0x007F...0x009F control characters are not allowed
        for (int index = 0x007F; index <= 0x009F; index++)
        {
            if (aStoreName.indexOf(index) > 0)
            {
                throw new IllegalArgumentException(
                    NON_SUPPORTED_UNICODE_CHARACTER + aStoreName);
            }
        }

        // 0xFEFF Byte-order-mark character is not allowed
        if (aStoreName.indexOf(0xFEFF) > 0)
        {
            throw new IllegalArgumentException(
                NON_SUPPORTED_UNICODE_CHARACTER + aStoreName);
        }

        // 0xFFF0...0xFFFF are not allowed
        for (int index = 0xFFF0; index <= 0xFFFF; index++)
        {
            if (aStoreName.indexOf(index) > 0)
            {
                throw new IllegalArgumentException(
                    NON_SUPPORTED_UNICODE_CHARACTER + aStoreName);
            }
        }

        // Create landmark store on default drive
        Locator locator = Locator.createLocator(aStoreName);
        int error = LandmarkStoreManager.getInstance().createStore(locator);
        // Handle error correctly
        if (error == KErrAlreadyExists || error == NativeError.KErrBadName)
        {
            throw new IllegalArgumentException(CREATE_LANDMARK_STORE_ERROR);
        }

        // Check for IO errors
        NativeError.checkIO(error, CREATE_LANDMARK_STORE_OPERATION_ERROR);
    }

    /**
     * Please refer JSR-179 spec
     */
    public static void deleteLandmarkStore(String aStoreName)
    throws IOException, LandmarkException
    {
        String arguments[] = { "" };
        // Ensure landmark store management permission from Java security
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        LocationPermission permission = new LocationPermission("landmarks://*",
                "management");
        appUtils.checkPermission(permission);
        checkNullArgument(aStoreName, LANDMARK_STORE_NAME_IS_NULL_ERROR);
        // Remove landmark store from the specified location
        Locator locator = Locator.createLocator(aStoreName);
        int error = LandmarkStoreManager.getInstance().deleteStore(locator);
        // Silently fail if store does not exist but handle the error if the
        // database is still used by other clients
        if (error != NativeError.KErrArgument
                && error != NativeError.KErrNotReady
                && error != NativeError.KErrBadName)
        {
            NativeError.checkIO(error, DELETE_LANDMARK_STORE_ERROR);
        }
    }

    /**
     * Please refer JSR-179 spec
     */
    public static String[] listLandmarkStores() throws IOException
    {
        String arguments[] = { "" };
        // Ensure landmark store read permission from Java Security
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        LocationPermission permission = new LocationPermission("landmarks://*",
                "read");
        appUtils.checkPermission(permission);
        String[] stores = null;
        // Get the list of landmark from the landmark store manager
        Vector locators = LandmarkStoreManager.getInstance().listStores();
        if (locators != null)
        {
            // Create a new string array and initialize it with the store names
            int numStores = locators.size();
            stores = new String[numStores];
            for (int i = 0; i < numStores; i++)
            {
                // The stores must not be listed as locators. The name of the
                // stores must be returned to the client.
                stores[i] = ((Locator) locators.elementAt(i)).getName();
            }
        }

        // null is returned if there are no stores in the device
        return stores;
    }

    /**
     * Please refer JSR-179 spec
     */
    public void addLandmark(Landmark aLandmark, String aCategory)
    throws IOException
    {
        String arguments[] = { "" };
        // Ensure landmark store write permission from Java Security
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        LocationPermission permission = new LocationPermission("landmarks://*",
                "write");
        appUtils.checkPermission(permission);
        checkNullArgument(aLandmark, LANDMARK_NAME_IS_NULL);

        // Prepare the landmark for saving and add it to the native database
        aLandmark.prepareForSave();

        int landmarkHandle = aLandmark.getHandle();
        int error = _addLandmark(iEventSourceHandle, iStoreHandle,
                                 landmarkHandle, aCategory);
        // Ensure that the store is open and not closed externally
        ensureOpenStore(error);
        // Check if the addition did not succeed due to some other reason
        NativeError.checkIO(error, ADD_LANDMARK_TO_CATEGORY_ERROR);

        // The landmark was added successfully. Associate it to this store
        aLandmark.associateToStore(this);
    }

    /**
     * Please refer JSR-179 spec
     */
    public Enumeration getLandmarks(String aCategory, String aName)
    throws IOException
    {
        int[] error = new int[1];
        // Get the landmarks from the native landmark store. null
        // arguments are interpreted as wildcards
        int[] landmarkHandles = _getLandmarksFromCategory(iEventSourceHandle,
                                iStoreHandle, aCategory, aName, error);
        // Handle returned landmark handles and error
        return handleGetLandmarks(landmarkHandles, error[0]);
    }

    /**
     * Please refer JSR-179 spec
     */
    public Enumeration getLandmarks() throws IOException
    {
        int[] error = new int[1];
        // Get all landmarks from the native landmark store
        int[] landmarkHandles = _getLandmarks(iEventSourceHandle, iStoreHandle,
                                              error);
        // Handle returned landmark handles and error
        return handleGetLandmarks(landmarkHandles, error[0]);
    }

    /**
     * Please refer JSR-179 spec
     */
    public Enumeration getLandmarks(String aCategory, double aMinLatitude,
                                    double aMaxLatitude, double aMinLongitude, double aMaxLongitude)
    throws IOException
    {
        // Check that all values are within specified range
        if (aMinLatitude < Coordinates.MIN_LATITUDE
                || aMinLatitude > Coordinates.MAX_LATITUDE
                || aMaxLatitude < Coordinates.MIN_LATITUDE
                || aMaxLatitude > Coordinates.MAX_LATITUDE
                || aMinLongitude < Coordinates.MIN_LONGITUDE
                || aMinLongitude >= Coordinates.MAX_LONGITUDE
                || aMaxLongitude < Coordinates.MIN_LONGITUDE
                || aMaxLongitude >= Coordinates.MAX_LONGITUDE
                || aMinLatitude > aMaxLatitude)
        {
            throw new IllegalArgumentException(INCORRECT_SEARCH_AREA_ARGUMENTS
                                               + aMinLatitude + ", " + aMaxLatitude + ", "
                                               + aMinLongitude + " or " + aMaxLongitude);
        }

        int[] error = new int[1];
        // Get all landmarks from the native landmark store
        int[] landmarkHandles = _getLandmarksFromArea(iEventSourceHandle,
                                iStoreHandle, aCategory, aMinLatitude, aMaxLatitude,
                                aMinLongitude, aMaxLongitude, error);
        // Handle returned landmark handles and error
        return handleGetLandmarks(landmarkHandles, error[0]);
    }

    /**
     * Please refer JSR-179 spec
     */
    public void removeLandmarkFromCategory(Landmark aLandmark, String aCategory)
    throws IOException
    {
        String arguments[] = { "" };
        // Ensure landmark store write permission from Java Security
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        LocationPermission permission = new LocationPermission("landmarks://*",
                "write");
        appUtils.checkPermission(permission);
        // Neither category nor landmark argument must be null
        checkNullArgument(aCategory, CATEGORY_NAME_IS_NULL_ERROR);
        checkNullArgument(aLandmark, LANDMARK_NAME_IS_NULL);

        // Check that if the landmark does not belong this store. If not, then
        // the call is simply ignored
        LandmarkStore store = aLandmark.getAssociatedStore();
        if (store == null || !store.getURI().equals(iStoreUri))
        {
            return;
        }

        int error = _removeLandmarkFromCategory(iEventSourceHandle,
                                                iStoreHandle, aLandmark.getHandle(), aCategory);
        // If the category did not exist the call is silently ignored
        if (error != NativeError.KErrNone && error != NativeError.KErrNotFound
                && error != NativeError.KErrArgument)
        {
            NativeError.checkIO(error, REMOVE_LANDMARK_FROM_CATEGORY_ERROR);
        }
    }

    /**
     * Please refer JSR-179 spec
     */
    public void updateLandmark(Landmark aLandmark) throws IOException,
                LandmarkException
    {
        String arguments[] = { "" };
        // Ensure landmark store write permission from Java Security
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        LocationPermission permission = new LocationPermission("landmarks://*",
                "write");
        appUtils.checkPermission(permission);
        checkNullArgument(aLandmark, LANDMARK_NAME_IS_NULL);

        // Throw IllegalArgumentException if the Landmark name
        // is greater than 255 (KLandmarkNameLength) characters
        if (aLandmark.getName().length() > KLandmarkNameLength)
        {
            throw new IllegalArgumentException(LANDMARK_NAME_LENGTH_EXCEEDS);
        }

        LandmarkStore store = aLandmark.getAssociatedStore();

        // Check that if the landmark does not belong this store. The name
        // must be used to match because a store might have been closed during
        // the life-time of this landmark object
        if (store == null || !store.getURI().equals(iStoreUri))
        {
            throw new LandmarkException(LANDMARK_DOES_NOT_BELONG_TO_STORE);
        }

        // Prepare the landmark for saving and add it to the native database
        aLandmark.prepareForSave();

        int landmarkHandle = aLandmark.getHandle();
        int error = _updateLandmark(iEventSourceHandle, iStoreHandle,
                                    landmarkHandle);

        // Ensure that the store is open and not closed externally
        ensureOpenStore(error);
        if (error == NativeError.KErrNotFound)
        {
            throw new LandmarkException(LANDMARK_DOES_NOT_BELONG_TO_STORE);
        }

        // Check if the addition did not succeed due to some other reason
        NativeError.checkIO(error, UPDATE_LANDMARK_ERROR);
    }

    /**
     * Please refer JSR-179 spec
     */
    public void deleteLandmark(Landmark aLandmark) throws IOException,
                LandmarkException
    {
        String arguments[] = { "" };
        // Ensure landmark store write permission from Java Security
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        LocationPermission permission = new LocationPermission("landmarks://*",
                "write");
        appUtils.checkPermission(permission);
        checkNullArgument(aLandmark, LANDMARK_NAME_IS_NULL);
        LandmarkStore store = aLandmark.getAssociatedStore();
        // Check that if the landmark does not belong this store. The name
        // must be used to match because a store might have been closed during
        // the life-time of this landmark object
        if (store == null || !store.getURI().equals(iStoreUri))
        {
            throw new LandmarkException(LANDMARK_DOES_NOT_BELONG_TO_STORE);
        }

        int landmarkHandle = aLandmark.getHandle();
        int error = _deleteLandmark(iEventSourceHandle, iStoreHandle,
                                    landmarkHandle);
        // Ensure that the store is open and not closed externally
        ensureOpenStore(error);
        // Check if the addition did not succeed due to some other reason.
        NativeError.checkIO(error, DELETE_LANDMARK_ERROR);
        // Landmark was successfully removed. The association still needs to
        // be remained since the landmark is considered as "belong to this
        // store"
        // For more information, see the API specification of JSR-179
    }

    /**
     * Please refer JSR-179 spec
     */
    public Enumeration getCategories()
    {
        int[] error = new int[1];
        // Get the names of the categories from the native
        String[] categories = _listCategories(iEventSourceHandle, iStoreHandle,
                                              error);
        // Return null if the store has been closed. The case might be that
        // this store has been closed due to a call to deleteLandmarkStore()
        if (error[0] == KErrSessionClosed)
        {
            return null;
        }
        // Listing categories went wrong so the only solution is to throw OOME
        else if (categories == null || error[0] < NativeError.KErrNone)
        {
            throw new OutOfMemoryError(UNABLE_TO_LIST_CATEGORIES_ERROR);
        }

        // Create a Vector from the category names and return the elements from
        // it
        Vector categoryVector = new Vector(categories.length);
        for (int i = 0; i < categories.length; i++)
        {
            categoryVector.addElement(categories[i]);
        }

        return categoryVector.elements();
    }

    /**
     * Please refer JSR-179 spec
     */
    public void addCategory(String aCategory) throws LandmarkException,
                IOException
    {
        String arguments[] = { "" };
        // Ensure category management permission form the Java Security
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        LocationPermission permission = new LocationPermission("landmarks://*",
                "management");
        appUtils.checkPermission(permission);
        checkNullArgument(aCategory, CATEGORY_NAME_IS_NULL_ERROR);
        // Add new category to the native landmark database
        int error = _addCategory(iEventSourceHandle, iStoreHandle, aCategory);
        // Ensure that the store is open and not closed externally
        ensureOpenStore(error);
        // Check that if the category already exists in the store or if the
        // length of the category name does not meet the native API requirements
        if (error == KErrAlreadyExists || error == NativeError.KErrArgument)
        {
            throw new IllegalArgumentException(ADD_CATEGORY_ARGUMENT_ERROR);
        }
        else if (error == NativeError.KErrNotSupported)
        {
            throw new LandmarkException(ADDING_CATEGORIES_NOT_SUPPORTED_ERROR);
        }
        // Check the error for IO exception
        NativeError.checkIO(error, ADD_CATEGORY_TO_LANDMARK_STORE_ERROR);
    }

    public void deleteCategory(String aCategory) throws LandmarkException,
                IOException
    {
        String arguments[] = { "" };
        // Ensure category management permission form the Java Security
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        LocationPermission permission = new LocationPermission("landmarks://*",
                "management");
        appUtils.checkPermission(permission);
        checkNullArgument(aCategory, CATEGORY_NAME_IS_NULL_ERROR);
        // Delete the category from the native landmark database
        int error = _deleteCategory(iEventSourceHandle, iStoreHandle,
                                    aCategory);
        // Ensure that the store is open and not closed externally
        ensureOpenStore(error);
        if (error == NativeError.KErrNotSupported)
        {
            throw new LandmarkException(DELETING_CATEGORIES_NOT_SUPPORTED_ERROR);
        }
        // Check the error for IO exception
        NativeError.checkIO(error, DELETE_CATEGORY_FROM_LANDMARK_STORE_ERROR);
    }

    // Package private methods

    /**
     * Returns the uri of the store
     */
    String getURI()
    {
        return iStoreUri;
    }

    /**
     * Checks wether the store is open or not
     *
     * @return true if the store is open, false if not
     */
    private synchronized boolean isOpen()
    {
        return iStoreHandle != -1;
    }

    /**
     * Ensures that the store is open. If the store has been externally closed
     * this method throws an IOException with a description
     */
    private void ensureOpenStore(int aError) throws IOException
    {
        if (!isOpen() || aError == KErrSessionClosed)
        {
            throw new IOException(LANDMARK_STORE_IS_CLOSED_ERROR);
        }
    }

    /**
     * Handles the return values of getLandmarks() functions
     *
     * @param aLandmarkHandles
     *            An array holding native landmark peer object handles
     * @param aError
     *            An error which may have occured during the getLandmarks()
     *            function call
     * @return An enumeration of landmarks, null if there is no landmarks
     */
    private Enumeration handleGetLandmarks(int[] aLandmarkHandles, int aError)
    throws IOException
    {
        // Ensure that the store is still open and hasn't been closed
        // due to a call to deleteLandmarkStore()
        ensureOpenStore(aError);
        // Check for IO errors
        NativeError.checkIO(aError, OBTAIN_LANDMARK_ERROR);
        // Return null if there were no landmarks in the store
        if (aLandmarkHandles == null || aLandmarkHandles.length == 0)
        {
            return null;
        }

        // Create new landmarks from the returned native item handles
        Vector landmarks = new Vector(aLandmarkHandles.length);
        for (int i = 0; i < aLandmarkHandles.length; i++)
        {
            Landmark newLandmark = new Landmark(aLandmarkHandles[i],
                                                iEventSourceHandle);
            // The landmark was from the native store so associate it
            newLandmark.associateToStore(this);
            // Add the new landmark to the vector which elements are
            // returned to the caller
            landmarks.addElement(newLandmark);
        }
        // Return the elements from the vector
        return landmarks.elements();
    }

    /**
     * Checks if the argument is null and throws a null pointer exception
     *
     * @param aObject
     *            The objects which is compared against null
     * @param aError
     *            The error message which is used when a null pointer exception
     *            has occured
     */
    private static void checkNullArgument(Object aObject, String aError)
    {
        if (aObject == null)
        {
            throw new NullPointerException(aError);
        }
    }

    /**
     * Adds a new landmark to the native landmark store
     *
     * @param aFunctionSourceHandle
     *            Handle to the native event source
     * @param aStoreHandle
     *            Handle to the native side peer object
     * @param aLandmarkHandle
     *            Handle to the added landmark
     * @return NativeError.KErrNone if no errors occured. Otherwise one of the
     *         system wide error codes
     */
    private native int _addLandmark(int aFunctionSourceHandle, int aStoreHandle,
                                    int aLandmarkHandle, String aCategory);

    /**
     * Returns an array of landmark native side peer objects which are store
     * into the landmark store
     *
     * @param aFunctionSourceHandle
     *            Handle to the native event source
     * @param aStoreHandle
     *            Handle to the native side peer object
     * @param aError
     *            On return, contains an error code
     * @return An array of landmark native handles
     */
    private native int[] _getLandmarks(int aFunctionSourceHandle,
                                       int aStoreHandle, int[] aError);

    /**
     * Returns an array of landmark native side peer objects which are store
     * into the landmark store and match the given parameters
     *
     * @param aFunctionSourceHandle
     *            Handle to the native event source
     * @param aStoreHandle
     *            Handle to the native side peer object
     * @param aCategory
     *            The category from which the landmarks a searched null argument
     *            will match all categories
     * @param aName
     *            The name which is used for matching. null argument will match
     *            all possible names
     * @param aError
     *            On return, contains an error code
     * @return An array of landmark native handles
     */
    private native int[] _getLandmarksFromCategory(int aFunctionSourceHandle,
            int aStoreHandle, String aCategory, String aName, int[] aError);

    /**
     * Returns an array of landmark native side peer objects which are store
     * into the landmark store and match the given parameters
     *
     * @param aFunctionSourceHandle
     *            Handle to the native event source
     * @param aStoreHandle
     *            Handle to the native side peer object
     * @param aCategory
     *            The category from which the landmarks a searched null argument
     *            will match all categories
     * @param aMinLatitude
     *            The minimum latitude of the area.
     * @param aMaxLatitude
     *            The maximum latitude of the area.
     * @param aMinLongitude
     *            The minimum longitude of the area.
     * @param aMaxLongitude
     *            The maximum longitude of the area.
     * @param aError
     *            On return, contains an error code
     * @return An array of landmark native handles
     */
    private native int[] _getLandmarksFromArea(int aFunctionSourceHandle,
            int aStoreHandle, String aCategory, double aMinLatitude,
            double aMaxLatitude, double aMinLongitude, double aMaxLongitude,
            int[] aError);

    /**
     * Removes a landmark from a category
     *
     * @param aFunctionSourceHandle
     *            Handle to the native event source
     * @param aStoreHandle
     *            Handle to the native side peer object
     * @param aLandmarkHandle
     *            Handle to the native side landmark
     * @param aCategory
     *            The category from which the landmark should be removed
     * @return NativeError.KErrNone if no errors occured. Otherwise one of the
     *         system wide error codes
     */
    private native int _removeLandmarkFromCategory(int aFunctionSourceHandle,
            int aStoreHandle, int aLandmarkHandle, String aCategory);

    /**
     * Updates an existing landmark in the native landmark store
     *
     * @param aFunctionSourceHandle
     *            Handle to the native event source
     * @param aStoreHandle
     *            Handle to the native side peer object
     * @param aLandmarkHandle
     *            Handle to the updated landmark
     * @return NativeError.KErrNone if no errors occured. Otherwise one of the
     *         system wide error codes
     */
    private native int _updateLandmark(int aFunctionSourceHandle,
                                       int aStoreHandle, int aLandmarkHandle);

    /**
     * Removes an existing landmark from the native landmark store
     *
     * @param aFunctionSourceHandle
     *            Handle to the native event source
     * @param aStoreHandle
     *            Handle to the native side peer object
     * @param aLandmarkHandle
     *            Handle to the landmark which will be removed
     * @return NativeError.KErrNone if no errors occured. Otherwise one of the
     *         system wide error codes
     */
    private native int _deleteLandmark(int aFunctionSourceHandle,
                                       int aStoreHandle, int aLandmarkHandle);

    /**
     * Lists the categories which ara available in the store
     *
     * @param aFunctionSourceHandle
     *            Handle to the native event source
     * @param aStoreHandle
     *            Handle to the native side peer object
     * @param aError
     *            On return, contains an error code
     */
    private native String[] _listCategories(int aFunctionSourceHandle,
                                            int aStoreHandle, int[] aError);

    /**
     * Adds new category to the native landmark database The method returns with
     * an error if the name is incorrect
     *
     * @param aFunctionSourceHandle
     *            Handle to the native event source
     * @param aStoreHandle
     *            Handle to the native side peer object
     * @param aCategory
     *            The name of the new category
     * @return NativeError.KErrNone if no errors occured. Otherwise one of the
     *         system wide error codes
     */
    private native int _addCategory(int aFunctionSourceHandle, int aStoreHandle,
                                    String aCategory);

    /**
     * Removes an existing category from the native landmark database The method
     * returns with an error if the name is incorrect
     *
     * @param aFunctionSourceHandle
     *            Handle to the native event source
     * @param aStoreHandle
     *            Handle to the native side peer object
     * @param aCategory
     *            The name of the removed category
     * @return NativeError.KErrNone if no errors occured. Otherwise one of the
     *         system wide error codes
     */
    private native int _deleteCategory(int aFunctionSourceHandle,
                                       int aStoreHandle, String aCategory);

    /**
     * Disposes the native side peer object
     *
     * @param aFunctionSourceHandle
     *            Handle to the native event source
     * @param aStoreHandle
     *            Handle to the native side peer object
     */
    private native void _dispose(int aFunctionSourceHandle, int aStoreHandle);

}

// End of file
