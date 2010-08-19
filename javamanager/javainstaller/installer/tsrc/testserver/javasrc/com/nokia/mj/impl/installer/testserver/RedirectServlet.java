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

import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * Servlet implementation class RedirectServlet
 */
public class RedirectServlet extends HttpServlet
{
    private static final long serialVersionUID = 1L;

    protected void processRequest(HttpServletRequest request,
                                  HttpServletResponse response)
    throws ServletException, IOException
    {
        int redirectCount = 1;
        String redirectUrl = "http://" + request.getServerName() + ":"
                             + request.getServerPort()
                             + request.getContextPath();

        String countParam = request.getParameter("count");
        String nameParam = request.getParameter("name");

        if (countParam != null)
        {
            try
            {
                redirectCount = Integer.parseInt(countParam);
            }
            catch (NumberFormatException nfe)
            {
            }
        }

        if (redirectCount > 1)
        {
            redirectUrl += request.getServletPath() + "?count="
                           + (redirectCount - 1) + "&name=";
        }
        else
        {
            redirectUrl += "/";
        }

        redirectUrl += nameParam;
        response.sendRedirect(redirectUrl);
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
