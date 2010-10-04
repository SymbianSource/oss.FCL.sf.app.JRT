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

package com.nokia.mj.impl.nokiauiapi;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.OutputStream;

import org.eclipse.swt.internal.extension.DisplayExtension;
import org.eclipse.swt.widgets.Internal_PackageSupport;

import com.nokia.mj.impl.nokiauiapi.OS;
import com.nokia.mid.ui.SoftNotificationException;
import com.nokia.mid.ui.SoftNotificationListener;
import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

/**
 * SoftNotificationImpl implements the functionality specified in
 * com.nokia.mid.ui.SoftNotification class.
 */
public final class SoftNotificationImpl extends com.nokia.mid.ui.SoftNotification {

	// Native CSoftNotification handle
	private int iSoftNotificationHandle = 0;

	// Observer of this class set by the API user. An array is used to allow
	// listener synchronization.
	private SoftNotificationListener[] iListener;

	// Listener event when user accepted notification
	private final static int EVENT_ACCEPT = 1;

	// Listener event when user dismissed notification
	private final static int EVENT_DISMISS = 2;

	// Error message prefix for exceptions.
	private final static String ERROR_MESSAGE_PREFIX = "Failed to ";

	// The class will have native resources so register it for
	// finalization.
	private Finalizer mFinalizer;

	private int err;

	private int id;
	
	private int midletUid;

	/**
	 * Constructor. New instance with old identifier.
	 * 
	 * @param aNotificationId
	 *            Identification of previous soft notification.
	 */
	public SoftNotificationImpl(int aNotificationId) {
		initialize(aNotificationId);
	}

	/**
	 * Constructor. New instance.
	 */
	public SoftNotificationImpl() {
		initialize(0);
	}

	/**
	 * Initializes the instance.
	 * 
	 * @param aNotificationId
	 *            Id of the soft notification.
	 */
	protected void initialize(int aNotificationId) {
		final int iNotificationId = aNotificationId;
		iListener = new SoftNotificationListener[1];

		Runnable r = new Runnable() {
			public void run() {
				String uidString = ApplicationInfo.getInstance().getUid().toString();
				// removing "[" and "]" brackets
				uidString = uidString.substring(1,uidString.length()-1);
				// converting to decimal
				midletUid =(int)Long.parseLong(uidString,16);
				iSoftNotificationHandle = OS.createNativePeer(midletUid, iNotificationId,
						SoftNotificationImpl.this);
			}
		};
		// if display is created already, execute it in UI thread no matter
		// what thread it is called from. Otherwise assume LCDUI application is
		// called
		// and create a display. eSWT is not allowed to call before display
		// creation
		if (DisplayExtension.getDisplayInstance() != null) {
			DisplayExtension.getDisplayInstance().syncExec(r);
		} else {
			com.nokia.mj.impl.nokialcdui.LCDUIInvoker
					.eSWTUIThreadRunnerSyncExec(r);
		}

		if (iSoftNotificationHandle <= 0) {
			throw new OutOfMemoryError();
		}

		mFinalizer = ((mFinalizer != null) ? mFinalizer : new Finalizer() {
			public void finalizeImpl() {
				close();
			}
		});
	}

	/**
	 * Called when the object is finalized by the garbage collector.
	 */
	public void close() {
		if (DisplayExtension.getDisplayInstance() != null) {
			Internal_PackageSupport.getDisplayInstance().syncExec(new Runnable() {
				public void run() {
					if (mFinalizer != null) {
						OS.destroy(iSoftNotificationHandle);
					}
					mFinalizer = null;
				}
			});
		}
	}

	/**
	 * Notification callback, called from the native side.
	 * 
	 * @param aEventArg
	 *            Occurred event.
	 */
	private void notificationCallback(int aEventArg) {
		// Synchronize the listener usage since the user may want to set it
		// to null during execution of this function.
		synchronized (iListener) {
			SoftNotificationListener listener = iListener[0];

			if (listener != null) {
				if (aEventArg == EVENT_ACCEPT) {
					listener.notificationSelected(this);
				} else if (aEventArg == EVENT_DISMISS) {
					listener.notificationDismissed(this);
				}
			}
		}
	}

	/**
	 * Checks the given error value. Throws SoftNotificationException if the
	 * given error value is other than NativeError.KErrNone.
	 * 
	 * @param aError
	 *            Error value to be checked.
	 * @param aErrorMessage
	 *            Message to be included in the exception.
	 */
	private final void checkError(int aError, String aErrorMessage)
			throws SoftNotificationException {
		if (aError != 0) {
			throw new SoftNotificationException(ERROR_MESSAGE_PREFIX
					+ aErrorMessage, aError);
		}
	}

	// Functions from the base class.

	/**
	 * See class SoftNotification for comments
	 */
	public int getId() {
		if (DisplayExtension.getDisplayInstance() != null) {
			Internal_PackageSupport.getDisplayInstance().syncExec(
					new Runnable() {
						public void run() {
							id = 0;
							id = OS.getId(iSoftNotificationHandle);
						}
					});
		}
		return id;
	}

	/**
	 * See class SoftNotification for comments
	 */
	public void post() throws SoftNotificationException {
		if (DisplayExtension.getDisplayInstance() != null) {
			Internal_PackageSupport.getDisplayInstance().syncExec(
					new Runnable() {
						public void run() {
							err = 0;
							err = OS
									.showSoftNotification(iSoftNotificationHandle);
						}
					});
		}
		checkError(err, "add soft notification");
	}

	/**
	 * See class SoftNotification for comments
	 */
	public void remove() throws SoftNotificationException {
		if (DisplayExtension.getDisplayInstance() != null) {
			Internal_PackageSupport.getDisplayInstance().syncExec(
					new Runnable() {
						public void run() {
							err = 0;
							err = OS
									.removeSoftNotification(iSoftNotificationHandle);
						}
					});
		}
		checkError(err, "remove notification");
	}

	/**
	 * See class SoftNotification for comments
	 */
	public void setListener(SoftNotificationListener aListener) {
		// Synchronize the listener setting since the user may set it
		// to null during it is used elsewhere in this class.
		synchronized (iListener) {
			iListener[0] = aListener;
		}
	}

	/**
	 * See class SoftNotification for comments
	 */
	public void setText(String aPrimaryText, String aSecondaryText)
			throws SoftNotificationException {
		final String iPrimaryText = aPrimaryText;
		final String iSecondaryText = aSecondaryText;

		if (DisplayExtension.getDisplayInstance() != null) {
			Internal_PackageSupport.getDisplayInstance().syncExec(
					new Runnable() {
						public void run() {
							err = 0;
							err = OS.setText(iSoftNotificationHandle,
									iPrimaryText == null ? "" : iPrimaryText,
									iSecondaryText == null ? ""
											: iSecondaryText);
						}
					});
		}
		checkError(err, "set note text");
	}

	/**
	 * See class SoftNotification for comments
	 */
	public void setSoftkeyLabels(String aSoftkey1Label, String aSoftkey2Label)
			throws SoftNotificationException {
		int err = 0;
		checkError(err, "set softkeys");
	}

	private String stringReplace(String source, String searchStr,
			String replacementStr) {
		StringBuffer strBuffer = new StringBuffer();
		int pos = source.indexOf(searchStr);

		while (pos != -1) {
			strBuffer.append(source.substring(0, pos)).append(replacementStr);
			source = source.substring(pos + searchStr.length());
			pos = source.indexOf(searchStr);
		}
		strBuffer.append(source);
		return strBuffer.toString();
	}

	/**
	 * See class SoftNotification for comments
	 */
	public void setImage(byte[] aImageData) throws SoftNotificationException {
		if (aImageData != null) {
			final byte[] iImageData = aImageData;
			if (DisplayExtension.getDisplayInstance() != null) {
				Internal_PackageSupport.getDisplayInstance().syncExec(
						new Runnable() {
							public void run() {
								err = 0;
								String imagePath = "";
								try {
									String directoryPath = ApplicationInfo
											.getInstance().getRootPath();
									// Replace "\private\102033e6\apps" with
									// "resource\apps\java"
									directoryPath = stringReplace(
											directoryPath, "private", "public");
									directoryPath = directoryPath
											+ "softnotification\\"
											+ midletUid + "_"
											+ getId();
									String imageName = midletUid
											+ "_" + getId();
									FileUtility target = new FileUtility(
											directoryPath);
									if (!target.exists()) {
										target.mkdirs();
									}
									target = new FileUtility(directoryPath
											+ "\\" + imageName);
									if (!target.exists()) {
										target.createNewFile();
									}
									OutputStream fos = target
											.openOutputStream();
									ByteArrayInputStream in = new ByteArrayInputStream(
											iImageData);
									byte[] buf = new byte[1024];
									int len;
									while ((len = in.read(buf)) > 0) {
										fos.write(buf, 0, len);
									}
									in.close();
									fos.close();
									target = null;
									imagePath = directoryPath + "\\"
											+ imageName;
								} catch (IOException ex) {
									ex.printStackTrace();
								} catch (Throwable t) {
									t.printStackTrace();
								}
								err = OS.setImagePath(iSoftNotificationHandle,
										imagePath);
							}
						});
			}
		}
		checkError(err, "set image");
	}
}
