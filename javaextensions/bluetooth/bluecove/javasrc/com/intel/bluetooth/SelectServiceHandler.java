/**
 *  BlueCove - Java library for Bluetooth
 *  Copyright (C) 2006-2008 Vlad Skarzhevskyy
 *
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 *
 *  @author vlads
 *  @version $Id: SelectServiceHandler.java 2476 2008-12-01 17:41:59Z skarzhevskyy $
 */
package com.intel.bluetooth;

import java.util.Hashtable;
import java.util.Vector;

import javax.bluetooth.BluetoothStateException;
import javax.bluetooth.DeviceClass;
import javax.bluetooth.DiscoveryAgent;
import javax.bluetooth.DiscoveryListener;
import javax.bluetooth.RemoteDevice;
import javax.bluetooth.ServiceRecord;
import javax.bluetooth.UUID;

/**
 * Implementation of DiscoveryAgent.selectService().
 *
 * <p>
 * <b><u>Your application should not use this class directly.</u></b>
 *
 */
public class SelectServiceHandler implements DiscoveryListener {

	private DiscoveryAgent agent;

	private Object inquiryCompletedEvent = new Object();

	private boolean inquiryCompleted;

	private Object serviceSearchCompletedEvent = new Object();

	private boolean serviceSearchCompleted;

	private Hashtable devicesProcessed = new Hashtable();

	private Vector serviceSearchDeviceQueue = new Vector();

	private ServiceRecord servRecordDiscovered;

	private static int threadNumber;

	private static synchronized int nextThreadNum() {
		return threadNumber++;
	}

	public SelectServiceHandler(DiscoveryAgent agent) {
		this.agent = agent;
	}

	/**
	 * Please refer JSR-82
	 */
	public String selectService(UUID uuid, int security, boolean master) throws BluetoothStateException {
		if (uuid == null) {
			throw new NullPointerException("uuid is null");
		}
		switch (security) {
		case ServiceRecord.NOAUTHENTICATE_NOENCRYPT:
		case ServiceRecord.AUTHENTICATE_NOENCRYPT:
		case ServiceRecord.AUTHENTICATE_ENCRYPT:
			break;
		default:
			throw new IllegalArgumentException();
		}

		RemoteDevice[] devs = agent.retrieveDevices(DiscoveryAgent.PREKNOWN);
		for (int i = 0; (devs != null) && (i < devs.length); i++) {
			ServiceRecord sr = findServiceOnDevice(uuid, devs[i]);
			if (sr != null) {
				return sr.getConnectionURL(security, master);
			}
		}
		devs = agent.retrieveDevices(DiscoveryAgent.CACHED);
		for (int i = 0; (devs != null) && (i < devs.length); i++) {
			ServiceRecord sr = findServiceOnDevice(uuid, devs[i]);
			if (sr != null) {
				return sr.getConnectionURL(security, master);
			}
		}
		ParallelSearchServicesThread t = new ParallelSearchServicesThread(uuid);
		t.start();

		synchronized (inquiryCompletedEvent) {
			if (!agent.startInquiry(DiscoveryAgent.GIAC, this)) {
				return null;
			}
			while (!inquiryCompleted) {
				try {
					inquiryCompletedEvent.wait();
				} catch (InterruptedException e) {
					return null;
				}
			}
			agent.cancelInquiry(this);
		}

		if ((servRecordDiscovered == null) && (!t.processedAll())) {
			synchronized (serviceSearchDeviceQueue) {
				serviceSearchDeviceQueue.notifyAll();
			}
			try {
				t.join();
			} catch (InterruptedException e) {
				return null;
			}
		}
		t.interrupt();

		if (servRecordDiscovered != null) {
			return servRecordDiscovered.getConnectionURL(security, master);
		}

		return null;
	}

	private class ParallelSearchServicesThread extends Thread {

		private boolean stoped = false;

		private int processedNext = 0;

		private int processedSize = 0;

		private UUID uuid;

		ParallelSearchServicesThread(UUID uuid) {
			super("SelectServiceThread-" + nextThreadNum());
			this.uuid = uuid;
		}

		boolean processedAll() {
			return (processedNext == serviceSearchDeviceQueue.size());
		}

		public void interrupt() {
			stoped = true;
			synchronized (serviceSearchDeviceQueue) {
				serviceSearchDeviceQueue.notifyAll();
			}
			super.interrupt();
		}

		public void run() {
			mainLoop: while ((!stoped) && (servRecordDiscovered == null)) {
				synchronized (serviceSearchDeviceQueue) {
					if ((inquiryCompleted) && (processedSize == serviceSearchDeviceQueue.size())) {
						return;
					}
					if (processedSize == serviceSearchDeviceQueue.size()) {
						try {
							serviceSearchDeviceQueue.wait();
						} catch (InterruptedException e) {
							return;
						}
					}
					processedSize = serviceSearchDeviceQueue.size();
				}

				for (int i = processedNext; i < processedSize; i++) {
					RemoteDevice btDevice = (RemoteDevice) serviceSearchDeviceQueue.elementAt(i);

					if (findServiceOnDevice(uuid, btDevice) != null) {
						break mainLoop;
					}
					processedNext += 1;
				}
//				processedNext = processedSize + 1;
			}
		}

	}

	private ServiceRecord findServiceOnDevice(UUID uuid, RemoteDevice device) {
		if (devicesProcessed.containsKey(device)) {
			return null;
		}
		devicesProcessed.put(device, device);
		DebugLog.debug("searchServices on ", device);
		synchronized (serviceSearchCompletedEvent) {
			try {
				serviceSearchCompleted = false;
				agent.searchServices(null, new UUID[] { uuid }, device, this);
			} catch (BluetoothStateException e) {
				DebugLog.error("searchServices", e);
				return null;
			}
			while (!serviceSearchCompleted) {
				try {
					serviceSearchCompletedEvent.wait();
				} catch (InterruptedException e) {
					return null;
				}
			}
		}
		return servRecordDiscovered;
	}

	public void deviceDiscovered(final RemoteDevice btDevice, DeviceClass cod) {
		if (devicesProcessed.containsKey(btDevice)) {
			return;
		}
		synchronized (serviceSearchDeviceQueue) {
			serviceSearchDeviceQueue.addElement(btDevice);
			serviceSearchDeviceQueue.notifyAll();
		}
	}

	public void inquiryCompleted(int discType) {
		synchronized (inquiryCompletedEvent) {
			inquiryCompleted = true;
			inquiryCompletedEvent.notifyAll();
		}
	}

	public void serviceSearchCompleted(int transID, int respCode) {
		synchronized (serviceSearchCompletedEvent) {
			serviceSearchCompleted = true;
			serviceSearchCompletedEvent.notifyAll();
		}
	}

	public void servicesDiscovered(int transID, ServiceRecord[] servRecord) {
		if ((servRecord.length > 0) && (servRecordDiscovered == null)) {
			servRecordDiscovered = servRecord[0];
			synchronized (serviceSearchCompletedEvent) {
				serviceSearchCompleted = true;
				serviceSearchCompletedEvent.notifyAll();
			}
			synchronized (inquiryCompletedEvent) {
				inquiryCompleted = true;
				inquiryCompletedEvent.notifyAll();
			}
		}
	}
}
