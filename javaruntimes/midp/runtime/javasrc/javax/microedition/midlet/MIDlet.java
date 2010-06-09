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


package javax.microedition.midlet;

import java.util.Hashtable;
import javax.microedition.io.ConnectionNotFoundException;

import com.nokia.mj.impl.rt.ui.ConfirmData;
import com.nokia.mj.impl.rt.ui.RuntimeUi;
import com.nokia.mj.impl.rt.ui.RuntimeUiFactory;

import com.nokia.mj.impl.rt.support.ApplicationInfo;

import com.nokia.mj.impl.security.midp.authorization.AccessControllerFactoryImpl;
import com.nokia.mj.impl.security.midp.authorization.AccessControllerImpl;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;

import com.nokia.mj.impl.utils.Logger;

/**
 * A class to be extended by the MIDlet applcation. See MIDP spec for
 * further details.
 */
public abstract class MIDlet
{

    /**
     * MidletApplication object is used to delegate and propagate the
     * life cycle tasks between the MIDlet application and the life cycle
     * of the MIDP runtime.
     */
    private MidletApplication mMidletApplication;

    private static Hashtable mNetworkPromptSchemes = null;
    private static Hashtable mNoPromptSchemes = null;

    /**
     * For allowing of MIDlet constructor to be called only once.
     */
    private static boolean mConstructionAllowed = true;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * For checking if MIDlet is allowed to do some protected action.
     * @see MIDP spec for further details.
     * @param permission The permsission to be checked.
     * @return 0 if denied, 1 if allowed, -1 if unknown.
     */
    public final int checkPermission(String permission)
    {
        try
        {
            ApplicationInfo appInfo = ApplicationInfo.getInstance();
            AccessControllerImpl accessControllerImpl =
                AccessControllerFactoryImpl.
                getAccessController(appInfo.getSuiteUid(),
                                    appInfo.getName());
            return accessControllerImpl.checkPermission(permission);
        }
        catch (Throwable t)
        {
        }
        return 0;
    }

    /**
     * For informing the life cycle that the MIDlet wants to close itself.
     * @see MIDP spec for further details.
     */
    public final void notifyDestroyed()
    {
        mMidletApplication.notifyDestroyed();
    }

    /**
     * For informing the life cycle that the MIDlet wants to enter into
     * paused state.
     * @see MIDP spec for further details.
     */
    public final void notifyPaused()
    {
        mMidletApplication.notifyPaused();
    }

    /**
     * For informing the life cycle that the MIDlet resume from paused state
     * into active state.
     * @see MIDP spec for further details.
     */
    public final void resumeRequest()
    {
        mMidletApplication.resumeRequest();
    }

    /**
     * For getting application property defined in the manifest file and
     * in the JAD file (if present).
     * @see MIDP spec for further details.
     * @param key The name of the attribute.
     * @return The value of the attribute if found, otherwise null.
     */
    public final String getAppProperty(String key)
    {
        if (key == null)
        {
            throw new NullPointerException(
                "MIDlet.getAppProperty() called with null key");
        }
        return mMidletApplication.getAppProperty(key);
    }

    /**
     * For doing the platform reguest. The implementation doesn't support
     * cancelling the pending requests.
     * @see MIDP spec for further details.
     * @param url The url for the platform request.
     * @return Always false, since there is no need to close the suite.
     */
    public final boolean platformRequest(String url)
    throws ConnectionNotFoundException
    {
        Logger.PLOG(Logger.EJavaRuntime,
                    "MIDlet.platformRequest(): " + url);
        if (null == url)
        {
            throw new NullPointerException(
                "URL is null");
        }
        url = url.trim();

        // Empty string means cancel request.
        // Because we don't queue requests, no action taken here.
        if (true == url.equals(""))
        {
            return false;
        }

        // If the platform request is used to start arbitrary native application,
        // check that MIDlet is in manufacturer or operator domain
        if (startsArbitraryNativeApp(url))
        {
            ApplicationInfo appInfo = ApplicationInfo.getInstance();
            String domain = appInfo.getProtectionDomain();
            if ((ApplicationInfo.MANUFACTURER_DOMAIN.equals(domain) != true) &&
                    (ApplicationInfo.OPERATOR_DOMAIN.equals(domain) != true))
            {
                Logger.WLOG(Logger.EJavaRuntime,
                            "Only manufacturer or operator domain MIDlets can start arbitrary native apps.");

                throw new ConnectionNotFoundException(
                    "Request allowed only for manufacturer or operator MIDlets");
            }
        }


        Logger.ILOG(Logger.EJavaRuntime,
                    "Before handleConfirmationNote()");

        // Ask from the user if it is allowed to do the request.
        boolean acceptByUser = handleConfirmationNote(url);
        Logger.ILOG(Logger.EJavaRuntime,
                    "After handleConfirmationNote(): " + acceptByUser);
        if (false == acceptByUser)
        {
            throw new ConnectionNotFoundException("Request was cancelled by user");
        }

        // Do the request.
        _managePlatformRequest(url);
        return false;
    }


    /*** ---------------------------- PROTECTED --------------------------- */

    /**
     * The constructor of the MIDlet. It is possible to call this method only
     * by the MIDP life cycle and it can be done only once.
     * If the constructor of the subclass throws an exception, it will lead to
     * application start error dialog.
     * @see MIDP spec for further details.
     */
    protected MIDlet()
    {
        // Allow constructing of the MIDlet class only once.
        if (!mConstructionAllowed)
        {
            throw new SecurityException("Only application management "+
                                        "software is allowed to construct MIDlet");
        }
        mConstructionAllowed = false;

        // For communication between MIDlet and the MIDP life cycle.
        mMidletApplication = new MidletApplication(this);
        Logger.ILOG(Logger.EJavaRuntime,
                    "MIDlet created: " + this);
    }

    /**
     * The MIDP life cycle informs that it wants to close the MIDlet.
     * The unconditional is always true and any thrown exceptions will
     * be ignored.
     * @see MIDP spec for further details.
     * @param unconditional Always true.
     */
    protected abstract void destroyApp(boolean unconditional)
    throws MIDletStateChangeException;

    /**
     * The MIDP life cycle informs that it wants to start the MIDlet.
     * Any thrown exception will lead to application start error dialog.
     * @see MIDP spec for further details.
     */
    protected abstract void startApp() throws MIDletStateChangeException;

    /**
     * The MIDP life cycle informs that it wants to start the MIDlet.
     * This is never called by the implementation.
     * @see MIDP spec for further details.
     */
    protected abstract void pauseApp();

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * @return 'true' is returned in following situations:
     *         - confirmation query does not need to show or if user.
     *         - confirmation query has been shown to the user and user
     *           has selected 'Allow'.
     */
    private boolean handleConfirmationNote(String url)
    throws ConnectionNotFoundException
    {
        ApplicationInfo appInfo = ApplicationInfo.getInstance();
        String domain = appInfo.getProtectionDomain();
        if ((ApplicationInfo.MANUFACTURER_DOMAIN.equals(domain) == true) ||
                (ApplicationInfo.OPERATOR_DOMAIN.equals(domain) == true))
        {
            return true;
        }

        if ((mNetworkPromptSchemes == null) ||
                (mNoPromptSchemes == null))
        {
            fillSchemes();
        }

        String scheme = getScheme(url);
        String appName = appInfo.getName();
        String namedPermission = (String)mNetworkPromptSchemes.get(scheme);
        if (namedPermission != null)
        {
            AccessControllerImpl accessControllerImpl =
                AccessControllerFactoryImpl.
                getAccessController(appInfo.getSuiteUid(),
                                    appInfo.getName());
            return accessControllerImpl.isPermissionAllowed(namedPermission);
        }
        if (mNoPromptSchemes.containsKey(scheme) == true)
        {
            return true;
        }
        return showNote(appName,
                        SecurityPromptMessage.QUESTION_ID_APP_LAUNCH_VIA_PLAT_REQ);
    }


    private boolean showNote(String aAppName,String aConfirmationQueryId)
    {
        SecurityPromptMessage msg = SecurityPromptMessage.getInstance();
        String queryTxt = msg.getText(aConfirmationQueryId,null);
        String[] buttons = new String[2];
        buttons[0] = msg.getText(SecurityPromptMessage.ANSWER_ID_ALLOW,null);
        buttons[1] = msg.getText(SecurityPromptMessage.ANSWER_ID_DENY,null);
        ConfirmData dataObj = new ConfirmData(queryTxt,buttons,0);

        RuntimeUi uiObj = RuntimeUiFactory.getRuntimeUi();
        boolean isUserAnswered = uiObj.confirm(aAppName,dataObj);
        if (isUserAnswered == false)
        {
            return false;
        }
        int indexToChosenButton = dataObj.getAnswer();
        if (indexToChosenButton == 0)
        {
            return true;
        }
        return false;
    }

    private void fillSchemes()
    {
        if (mNetworkPromptSchemes == null)
        {
            mNetworkPromptSchemes = new Hashtable();
            mNetworkPromptSchemes.put("http:","javax.microedition.io.Connector.http");
            mNetworkPromptSchemes.put("https:","javax.microedition.io.Connector.https");
            mNetworkPromptSchemes.put("rtsp:","javax.microedition.io.Connector.rtsp");
            mNetworkPromptSchemes.put("mms:","javax.microedition.io.Connector.rtsp");
        }
        if (mNoPromptSchemes == null)
        {
            // These schemes start applications that ask user for
            // confirmation before any real action is done.
            mNoPromptSchemes = new Hashtable();
            mNoPromptSchemes.put("cti:","");
            mNoPromptSchemes.put("mailto:","");
            mNoPromptSchemes.put("mms:","");
            mNoPromptSchemes.put("mmsto:","");
            mNoPromptSchemes.put("rtsp:","");
            mNoPromptSchemes.put("sms:","");
            mNoPromptSchemes.put("tel:","");
        }
    }

    private String getScheme(String uri)
    throws ConnectionNotFoundException
    {
        int index = uri.indexOf(':');
        if (index == -1)
        {
            throw new ConnectionNotFoundException("Incorrect URI: " + uri);
        }
        index++;
        if (uri.length() <= index)
        {
            throw new ConnectionNotFoundException("Incorrect URI: " + uri);
        }
        return uri.substring(0,index);
    }

    private boolean startsArbitraryNativeApp(String uri)
    {
        // schemes are case sensitive
        if (uri.startsWith("nativeapp:"))
        {
            return true;
        }

        if (uri.startsWith("localapp:native/launch?") ||
                uri.startsWith("localapp://native/launch?"))
        {
            return true;
        }

        return false;
    }

    /*** ----------------------------- NATIVE ----------------------------- */

    private native void _managePlatformRequest(String url);
}
