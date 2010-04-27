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

import java.io.*;
import java.util.Enumeration;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.io.file.FileSystemRegistry;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;

public class Printer
{
    private static final String FILENAME = "rmsbenchmark.csv";

    public Printer()
    {
    }

    public void print()
    {
        System.out.println("Printer.print() called");
        boolean mmcFound = false;
        boolean s60environment = false;
        Enumeration enumeration = FileSystemRegistry.listRoots();
        String root = "";
        String temp = "";
        while (enumeration.hasMoreElements())
        {
            root = (String) enumeration.nextElement();
            if (root.equals("root1/"))
            {
                temp = root; // WTK
            }
            else if (root.equals("E:/"))
            {
                mmcFound = true;
                s60environment = true;
            }
            else if (root.equals("C:/"))
            {
                s60environment = true;
            }
        }
        if (s60environment &&  mmcFound)
        {
            root = "E:/";
        }
        else if (s60environment)
        {
            root = "C:/Data/Others/";
        }
        else
        {
            root = temp;
        }

        try
        {
            String url = "file:///" + root + FILENAME;
            System.out.println("Printer.print() file system url: " + url);
            FileConnection fileconnection = (FileConnection) Connector.open(url, Connector.READ_WRITE);
            OutputStream outputstream;
            boolean fileExistsAlready = true;
            if (!fileconnection.exists())
            {
                fileconnection.create();
                fileExistsAlready = false;
                outputstream = fileconnection.openOutputStream(); // write in a newly created file
            }
            else
            {
                outputstream = fileconnection.openOutputStream(fileconnection.fileSize());   // append
            }

            OutputStreamWriter writer = new OutputStreamWriter(outputstream);
            if (!fileExistsAlready)
            {
                writer.write(BenchmarkRunner.TESTNAMES, 0, BenchmarkRunner.TESTNAMES.length());
                writer.write(BenchmarkRunner.TESTS, 0, BenchmarkRunner.TESTS.length());
            }
            writer.write(BenchmarkRunner.SCORES.toString(), 0, BenchmarkRunner.SCORES.length());
            writer.flush();
            System.out.println("SCORES: " + BenchmarkRunner.SCORES.toString());
            if (writer != null) writer.close();
            if (outputstream != null) outputstream.close();
            if (fileconnection != null) fileconnection.close();
        }
        catch (IOException ioexception)
        {
            System.out.println(ioexception.toString() + ": " + ioexception.getMessage());
        }
        catch (Exception exception)
        {
            System.out.println(exception.toString() + ": " + exception.getMessage());
        }
        System.out.println("Printer.print() returns");
    }
}
