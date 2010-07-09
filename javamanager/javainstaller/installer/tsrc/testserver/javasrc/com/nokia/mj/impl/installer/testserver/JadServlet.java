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

package com.nokia.mj.impl.installer.testserver;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileNotFoundException;
import java.io.IOException;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * Servlet implementation class JadServlet
 */
public class JadServlet extends HttpServlet
{
    private static final long serialVersionUID = 1L;

    protected void processRequest(HttpServletRequest request,
                                  HttpServletResponse response)
    throws ServletException, IOException
    {
        ServletContext context = getServletContext();
        String localPath = context.getRealPath(request.getServletPath());

        response.setContentType("text/vnd.sun.j2me.app-descriptor");

        BufferedReader in = null;
        ServletOutputStream out = null;

        try
        {
            in = new BufferedReader(new FileReader(localPath));
            out = response.getOutputStream();

            String line = in.readLine();
            while (line != null)
            {
                if (line.startsWith("MIDlet-Jar-URL:")
                        || line.startsWith("MIDlet-Install-Notify:")
                        || line.startsWith("MIDlet-Delete-Notify:"))
                {
                    line = line.replaceAll("<SERVER>", request.getServerName()
                                           + ":" + request.getServerPort());
                    line = line.replaceAll("<CONTEXT>",
                                           request.getContextPath());
                }
                out.println(line);
                line = in.readLine();
            }
        }
        catch (FileNotFoundException e)
        {
            response.sendError(HttpServletResponse.SC_NOT_FOUND,
                               request.getServletPath());
        }
        finally
        {
            if (out != null)
            {
                out.close();
            }
            if (in != null)
            {
                in.close();
            }
        }
    }

    protected void doGet(HttpServletRequest request,
                         HttpServletResponse response)
    throws ServletException, IOException
    {
        processRequest(request, response);
    }

    protected void doPost(HttpServletRequest request,
                          HttpServletResponse response)
    throws ServletException, IOException
    {
        processRequest(request, response);
    }

}
