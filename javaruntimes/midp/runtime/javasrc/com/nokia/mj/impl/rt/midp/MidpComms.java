/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.rt.midp;


import com.nokia.mj.impl.comms.CommsEndpoint;
import com.nokia.mj.impl.comms.CommsListener;
import com.nokia.mj.impl.comms.CommsMessage;

import com.nokia.mj.impl.utils.Uid;

/**
 * A class for Comms connection towards Java Captain. This class is used to
 * send and receive messages to/from Java Captain.
 */

final class MidpComms implements CommsListener
{
    /**
     * RTC plugin id of the Java Captain. Must be in sync with the constant
     * PLUGIN_ID_RTC_C in comms.h
     */
    private static final int PLUGIN_ID_RTC_C = 1;

    /**
     * RTC message ids Must be in sync with the constants in rtcmessages.h
     */
    private static final int RTC_MSG_ID_LAUNCH_APPLICATION_REQ       = 101;
    private static final int RTC_MSG_ID_TERMINATE_APPLICATION_REQ    = 102;
    private static final int RTC_MSG_ID_APPLICATION_RUNNING_IND      = 103;
    private static final int RTC_MSG_ID_APPLICATION_TERMINATED_IND   = 104;

    /**
     * Flag identifying background start in options field in
     * RTC_MSG_ID_LAUNCH_APPLICATION_REQ message.
     */
    private static final int RTC_LAUNCH_TYPE_BACKGROUND_C           = 0x4;

    /**
     * PMC plugin id of the Java Captain. Must be in sync with the constant
     * PLUGIN_ID_PMC_C in comms.h
     */
    private static final int PLUGIN_ID_PMC_C = 9;

    /**
     * Message for informing captain that the process has been initialized.
     * This is used when entering pre-warmed state. This must be in sync
     * with the constant PMC_MSG_ID_PROCESS_INITIALISED_IND in pmcmessages.h
     */
    private static final int PMC_MSG_ID_PROCESS_INITIALISED_IND      = 121;

    /**
     * UID of the MIDlet. This needs to be sent to Java Captain for identying
     * purposes.
     */
    private Uid             mMidletUid;

    /**
     * The Comms connection towards Java Captain.
     */
    private CommsEndpoint   mComms;


    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * The constructor of MidpComms. This is used when the UID is not yet
     * known - i.e. in a case where the MIDP runtime is started into pre-warmed
     * state. The constructor will register a listener to already existing
     * Comms connection.
     */
    MidpComms()
    {
        mComms = CommsEndpoint.find("javacaptain");
        mComms.registerListener(PLUGIN_ID_RTC_C, this);
    }
    /**
     * The constructor of MidpComms used when thw UID of the MIDlet is known.
     * @see MidpComms()
     */
    MidpComms(Uid midletUid)
    {
        this();
        mMidletUid = midletUid;
    }


    /**
     * Unregisters the Comms listener.
     */
    synchronized void close()
    {
        if (mComms != null)
        {
            if (Log.mOn) Log.logI("Closing comms (Java).");
            mComms.unregisterListener(PLUGIN_ID_RTC_C);
            mComms.destroy();
        }
        mComms = null;
    }

    /**
     * Sends the running indication message to the Java Captain. This is used
     * to inform Java Captain that the MIDP runtime was initialized correctly
     * and it ready to start the application.
     * @param status A status of the start up initializations.
     */
    synchronized void sendRunningIndication(int status)
    {
        if (Log.mOn) Log.logI("Comms (Java), sendRunningIndication.");
        CommsMessage message = new CommsMessage();
        message.setModuleId(PLUGIN_ID_RTC_C);
        message.setMessageId(RTC_MSG_ID_APPLICATION_RUNNING_IND);
        message.write(mMidletUid.toString());
        message.write(status);
        mComms.send(message);
    }

    /**
     * Sends the process running indication message to the Java Captain. This
     * is used to inform Java captain that the MIDP runtime was initialized
     * to pre-warmed state. This is used when the MIDlet to be started is not
     * known yet. Java Captain needs the process id for identification
     * purposes.
     * @param pid A pid of the MIDP process.
     */
    synchronized void sendProcessRunningIndication(int pid)
    {
        if (Log.mOn) Log.logI("Comms (Java), sendProcessRunningIndication.");
        CommsMessage message = new CommsMessage();
        message.setModuleId(PLUGIN_ID_PMC_C);
        message.setMessageId(PMC_MSG_ID_PROCESS_INITIALISED_IND);
        message.write(pid);
        mComms.send(message);
    }

    /**
     * Sends the terminated running indication message to the Java Captain.
     * This is used to inform Java captain that the MIDP runtime has closed
     * the MIDlet and is ready for startin shut down procedures. Since it can
     * not be guaranteed that the MIDP runtime succeeds on closing the JVM,
     * Java Captain will ensure that the process is destroyed on failure cases.
     * @param status A status of the shut down procedures.
     */
    synchronized void sendTerminatedIndication(int status)
    {
        if (Log.mOn) Log.logI("Comms (Java), sendTerminatedIndication.");
        CommsMessage message = new CommsMessage();
        message.setModuleId(PLUGIN_ID_RTC_C);
        message.setMessageId(RTC_MSG_ID_APPLICATION_TERMINATED_IND);
        message.write(mMidletUid.toString());
        message.write(status);
        mComms.send(message);
    }

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Handles the messages sent by Java Captain.
     * @param message A message from Java Captain.
     */
    public void processMessage(CommsMessage message)
    {
        if (Log.mOn) Log.logI("Comms (Java), processMessage.");
        switch (message.getMessageId())
        {
            /**
             * Application start up message could be received either in
             * pre-warmed state or when the application is re-launched.
             * In normal start up use case where pre-warming is not used
             * this message should not be sent.
             */
        case RTC_MSG_ID_LAUNCH_APPLICATION_REQ:
        {
            String uidString = message.readString();
            Uid uid = Uid.createUid(uidString);
            int type         = message.readInt();
            int options      = message.readInt();
            String rlc       = message.readString();
            String appArgs   = message.readString();
            if (Log.mOn) Log.logI("Comms (launchReq), rlc=" + rlc +
                                      ", uid=" + uid +
                                      ", type=" + type +
                                      ", options=" + options +
                                      ", args=" + appArgs);

            // In prewarmed case the UID is not known beforehand. The
            // storing is done in here.
            if (mMidletUid == null)
            {
                mMidletUid = uid;
            }

            // Checking that the the message is sent to correct MIDP
            // runtime.
            if (mMidletUid.equals(uid))
            {
                boolean backGround = false;
                if (type ==  RTC_LAUNCH_TYPE_BACKGROUND_C)
                {
                    backGround = true;
                }

                MidletLifeCycle.getInstance().
                launchMidletRequest(uid, backGround, appArgs);
            }
            else
            {
                Log.logE("Comms (launchReq) with illegal UID, " +
                         "rlc=" + rlc +
                         ", uid(recv)=" + uid +
                         ", midletUid=" + mMidletUid +
                         ", type=" + type +
                         ", options=" + options);
            }
        }
        break;

        /**
         * Java Captain wants to stop the MIDlet.
         */
        case RTC_MSG_ID_TERMINATE_APPLICATION_REQ:
        {
            String uidString = message.readString();
            Uid uid = Uid.createUid(uidString);

            int options = message.readInt();
            if (Log.mOn) Log.logI("Comms (terminatreReq), " +
                                      ", uid=" + uid +
                                      ", options=" + options);

            // Checking that the the message is sent to correct MIDP
            // runtime.
            if (mMidletUid.equals(uid))
            {
                MidletLifeCycle.getInstance().terminateMidletRequest(false);
            }
            else
            {
                Log.logE("Comms (terminatreReq) with illegal UID, " +
                         ", uid(recv)=" + uid +
                         ", midletUid=" + mMidletUid +
                         ", options=" + options);
            }
        }
        break;

        default:
            Log.logE("Comms, Unknow message received (MIDP JAVA RT): " +
                     ", messageId=" + message.getMessageId() +
                     ", moduleId=" + message.getModuleId());
            break;
        }
    }
}
