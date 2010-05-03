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
import java.io.PrintStream;
import javax.microedition.lcdui.*;
import javax.microedition.rms.*;

public class Storage
{
    private static final String STORAGE_NAME = "rmsbenchmark";
    private static final int REPEAT = 100;
    private int LENGTH;

    private RecordStore recordStore;

    public Storage(int aLength)
    {
        LENGTH = aLength;
        try
        {
            recordStore = RecordStore.openRecordStore(STORAGE_NAME, true);
        }
        catch (RecordStoreException _ex)
        {
            System.out.println("RecordStoreException: " + _ex.toString());
        }
        Utils.initRandom();
    }

    public void clean()
    {
        try
        {
            recordStore.closeRecordStore();
            RecordStore.deleteRecordStore(STORAGE_NAME);
        }
        catch (RecordStoreException _ex)
        {
            System.out.println("RecordStoreException: " + _ex.toString());
        }
    }

    public String createRecord()
    {
        String score = "";
        try
        {
            recordStore.getSize(); // commits any pending transactions
            byte data[] = new byte[LENGTH];
            for (int i = 0; i < LENGTH; i++)
            {
                data[i] = (byte) Utils.random(0, 127);
            }
            Utils.startTiming();
            for (int i = 0; i < REPEAT; i++)
            {
                recordStore.addRecord(data, 0, data.length);
            }
            Utils.stopTiming();
            score = Utils.getTime();
        }
        catch (RecordStoreException _ex)
        {
            System.out.println("RecordStoreException: " + _ex.toString());
        }
        return score;
    }

    public String deleteRecord()
    {
        String score = "";
        try
        {
            recordStore.getSize(); // commits any pending transactions
            int recordCount = recordStore.getNumRecords();
            int recordID[] = new int[recordCount];
            RecordEnumeration recordEnum = recordStore.enumerateRecords(null, null, false);
            for (int i = 0; i < recordCount; i++)
            {
                recordID[i] = recordEnum.nextRecordId();
            }
            int j = recordCount - 1;
            int random[] = new int[REPEAT];
            for (int i = 0; i < REPEAT; i++)
            {
                int recordChosen = recordID[Utils.random(0, recordID.length - 1)];
                random[i] = recordChosen;
                int tempRecordID[] = new int[j--];
                int l = 0;
                for (int k = 0; k < recordID.length; k++)
                {
                    if (recordID[k] != recordChosen)
                    {
                        tempRecordID[l++] = recordID[k];
                    }
                }
                recordID = tempRecordID;
            }
            Utils.startTiming();
            for (int i = 0; i < REPEAT; i++)
            {
                recordStore.deleteRecord(random[i]);
            }
            Utils.stopTiming();
            score = Utils.getTime();
        }
        catch (RecordStoreException _ex)
        {
            Utils.stopTiming();
            System.out.println("RecordStoreException: " + _ex.toString());
        }
        return score;
    }

    public String readEnumRecord()
    {
        String score = "";
        try
        {
            RecordEnumeration recordEnum = recordStore.enumerateRecords(null, null, false);
            Utils.startTiming();
            for (int i = 0; i < REPEAT; i++)
            {
                byte data[] = recordEnum.nextRecord();
            }
            Utils.stopTiming();
            score = Utils.getTime();
        }
        catch (RecordStoreException _ex)
        {
            Utils.stopTiming();
            System.out.println("RecordStoreException: " + _ex.toString());
        }
        return score;
    }

    public String readRandomRecord()
    {
        String score = "";
        try
        {
            int recordCount = recordStore.getNumRecords();
            if (recordCount == 0)
            {
                Utils.startTiming();
                throw new RecordStoreException();
            }
            int recordID[] = new int[recordCount];
            RecordEnumeration recordEnum = recordStore.enumerateRecords(null, null, false);
            for (int i = 0; i < recordCount; i++)
            {
                recordID[i] = recordEnum.nextRecordId();
            }
            int random[] = new int[REPEAT];
            for (int i = 0; i < REPEAT; i++)
            {
                random[i] = recordID[Utils.random(0, recordCount - 1)];
            }
            Utils.startTiming();
            for (int i = 0; i < REPEAT; i++)
            {
                recordStore.getRecord(random[i]);
            }
            Utils.stopTiming();
            score = Utils.getTime();
        }
        catch (RecordStoreException _ex)
        {
            Utils.stopTiming();
            System.out.println("RecordStoreException: " + _ex.toString());
        }
        return score;
    }

    public String createEnum()
    {
        String score = "";
        try
        {
            RecordEnumeration recordEnums[] = new RecordEnumeration[REPEAT];
            Utils.startTiming();
            for (int i = 0; i < REPEAT; i++)
            {
                recordEnums[i] = recordStore.enumerateRecords(null, null, false);
            }
            Utils.stopTiming();
            score = Utils.getTime();
            for (int i = 0; i < REPEAT; i++)
            {
                recordEnums[i].destroy();
            }
        }
        catch (RecordStoreException _ex)
        {
            Utils.stopTiming();
            System.out.println("RecordStoreException: " + _ex.toString());
        }
        return score;
    }

    public String setRecord()
    {
        String score = "";
        try
        {
            recordStore.getSize(); // commits any pending transactions
            int recordCount = recordStore.getNumRecords();
            int recordID[] = new int[recordCount];
            RecordEnumeration recordEnum = recordStore.enumerateRecords(null, null, false);
            for (int i = 0; i < recordCount; i++)
            {
                recordID[i] = recordEnum.nextRecordId();
            }
            int j = recordCount - 1;
            int random[] = new int[REPEAT];
            for (int i = 0; i < REPEAT; i++)
            {
                int recordChosen = recordID[Utils.random(0, recordID.length - 1)];
                random[i] = recordChosen;
                int tempRecordID[] = new int[j--];
                int l = 0;
                for (int k = 0; k < recordID.length; k++)
                {
                    if (recordID[k] != recordChosen)
                    {
                        tempRecordID[l++] = recordID[k];
                    }
                }
                recordID = tempRecordID;
            }

            byte data[] = new byte[LENGTH];
            for (int i = 0; i < LENGTH; i++)
            {
                data[i] = (byte) Utils.random(0, 127);
            }

            Utils.startTiming();
            for (int i = 0; i < REPEAT; i++)
            {
                recordStore.setRecord(random[i], data, 0, data.length);
            }
            Utils.stopTiming();
            score = Utils.getTime();
        }
        catch (RecordStoreException _ex)
        {
            Utils.stopTiming();
            System.out.println("RecordStoreException: " + _ex.toString());
        }
        return score;
    }

    public String openStore()
    {
        String score = "";
        try
        {
            recordStore.closeRecordStore();
            try
            {
                long openTotalTime = 0;
                for (int i = 0; i < REPEAT; i++)
                {
                    Utils.startTiming();
                    recordStore = RecordStore.openRecordStore(STORAGE_NAME, true);
                    Utils.stopTiming();
                    openTotalTime += Utils.getDiff();

                    recordStore.closeRecordStore();
                }
                score = Utils.getTime(openTotalTime);
            }
            catch (RecordStoreException _ex)
            {
                System.out.println("RecordStoreException: " + _ex.toString());
            }
            recordStore = RecordStore.openRecordStore(STORAGE_NAME, true);
        }
        catch (RecordStoreException _ex)
        {
            System.out.println("RecordStoreException: " + _ex.toString());
        }
        return score;
    }

    public String closeStore()
    {
        String score = "";
        try
        {
            recordStore.closeRecordStore();
            try
            {
                long closeTotalTime = 0;
                for (int i = 0; i < REPEAT; i++)
                {
                    recordStore = RecordStore.openRecordStore(STORAGE_NAME, true);

                    Utils.startTiming();
                    recordStore.closeRecordStore();
                    Utils.stopTiming();
                    closeTotalTime += Utils.getDiff();
                }
                score = Utils.getTime(closeTotalTime);
            }
            catch (RecordStoreException _ex)
            {
                System.out.println("RecordStoreException: " + _ex.toString());
            }
            recordStore = RecordStore.openRecordStore(STORAGE_NAME, true);
        }
        catch (RecordStoreException _ex)
        {
            System.out.println("RecordStoreException: " + _ex.toString());
        }
        return score;
    }

}
