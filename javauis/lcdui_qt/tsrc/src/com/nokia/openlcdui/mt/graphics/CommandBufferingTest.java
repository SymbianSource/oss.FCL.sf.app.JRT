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
package com.nokia.openlcdui.mt.graphics;

import junit.framework.*;
import javax.microedition.lcdui.*;
import com.nokia.openlcdui.mt.SWTTestCase;

public class CommandBufferingTest extends SWTTestCase
{
    private Image testImage;
    private final int testImageWidth = 20;
    private final int testImageHeight = 20;
    private int[] pixelData;
    
    private final int GRAPHICS_SETTINGS_1 = 1;
    private final int GRAPHICS_SETTINGS_2 = 2;
    
    private final int WHITE = 0x00ffffff;
    private final int BLACK = 0x00000000;
    private final int RED =   0x00ff0000; 
    private final int GREEN = 0x0000ff00;
    private final int BLUE =  0x000000ff; 
       
    /**
     * Constructor.
     */
    public CommandBufferingTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public CommandBufferingTest(String sTestName)
    {
        super(sTestName);
    }

    protected void setUp() throws Exception
    {
        super.setUp();
        testImage = Image.createImage(testImageWidth, testImageHeight);
        pixelData = new int[testImageWidth*testImageHeight];
    }

    protected void tearDown() throws Exception
    {
        super.tearDown();
        testImage = null;
        pixelData = null;
    }

    /**
     * To create the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods.
     *
     * @return new testsuite.
     */
    public static Test suite()
    {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
        java.util.Enumeration e;

        // Add widget tests
        methodNames = CommandBufferingTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new CommandBufferingTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testDefaultValues");
        methodNames.addElement("testClip1");
        methodNames.addElement("testClip2");
        methodNames.addElement("testDrawArc");
        methodNames.addElement("testDrawImage");
        methodNames.addElement("testDrawLine");
        methodNames.addElement("testDrawRect");
        methodNames.addElement("testDrawRGB");
        methodNames.addElement("testDrawRoundRect");
        methodNames.addElement("testDrawString");
        methodNames.addElement("testFillArc");
        methodNames.addElement("testFillRect");
        methodNames.addElement("testFillRoundRect");
        methodNames.addElement("testFillTriangle");
        

        return methodNames;
    }

    protected void runTest() throws Throwable
    {
    	if(getName().equals("testDefaultValues")) testDefaultValues();
    	else if(getName().equals("testClip1")) testClip1();
    	else if(getName().equals("testClip2")) testClip2();
    	else if(getName().equals("testDrawArc")) testDrawArc();
    	else if(getName().equals("testDrawImage")) testDrawImage();
    	else if(getName().equals("testDrawLine")) testDrawLine();
    	else if(getName().equals("testDrawRect")) testDrawRect();
    	else if(getName().equals("testDrawRGB")) testDrawRGB();
    	else if(getName().equals("testDrawRoundRect")) testDrawRoundRect();
    	else if(getName().equals("testDrawString")) testDrawString();
    	else if(getName().equals("testFillArc")) testFillArc();
    	else if(getName().equals("testFillRect")) testFillRect();
    	else if(getName().equals("testFillRoundRect")) testFillRoundRect();
    	else if(getName().equals("testFillTriangle")) testFillTriangle();
        else super.runTest();
    }

    /**
     * Tests the default value of a newly created Graphics object
     */
    void testDefaultValues()
    {
    	boolean pass = true;
    	String errorMsg = "";
    	Graphics g = testImage.getGraphics();
    	
    	// Validate default color
    	final int color = g.getColor();
    	if(color != BLACK) 
    	{
    	    pass = false;
    	    errorMsg += "Default color was " + color + ", while expected " + BLACK+". ";
    	}
    	
    	// Validate default clip
    	final int clipX = g.getClipX();
    	final int clipY = g.getClipY();
    	final int clipW = g.getClipWidth();
    	final int clipH = g.getClipHeight();
        if((clipX != 0) || (clipY != 0) || 
           (clipW != testImageWidth) || (clipH != testImageHeight))
        {
        	pass = false;
        	errorMsg += "Default clip was ("+clipX+","+clipY+","+clipW+","+clipH+"), " +
        			"while expected (0,0,"+testImageWidth+","+testImageHeight+"). ";
        }
    	
        // Validate that origin is in the top left corner
        final int transX = g.getTranslateX();
        final int transY = g.getTranslateY();
        if((transX != 0) || (transY != 0))
        {
        	pass = false;
        	errorMsg += "Default origo was ("+transX+","+transY+"), while expected (0,0). ";
        }
        
        // Validate default font
        final Font font = g.getFont();
        final Font refFont = Font.getDefaultFont();
        if((font.getFace() != refFont.getFace()) || 
           (font.getHeight() != refFont.getHeight()) ||
           (font.getSize() != refFont.getSize()) ||
           (font.getStyle() != refFont.getStyle())) 
        {
        	pass = false;
        	errorMsg += "Default font from Graphics does not match with the font returned by Font.getDefaultFont()	. ";
        }
        
        // Validate stroke style
        final int style = g.getStrokeStyle();
        if(style != Graphics.SOLID)
        {
            pass = false;
            errorMsg += "Default stroke style was "+ style +", while expected (SOLID) "+Graphics.SOLID;
        }
    	
    	if(!pass) 
    	{
    		fail(errorMsg);
    	}
    }
    
    /**
     * Test default clip by the values returned from Graphics 
     * and by rendering dots in corners and validating them
     */
    void testClip1()
    {
    	boolean pass = true;
    	String errorMsg = "Pixel validation failed at: ";
    	Graphics g = testImage.getGraphics();
    	g.drawLine(0, 0, 0, 0); // top-left
    	g.drawLine(testImageWidth-1, 0, testImageWidth-1, 0); // top-right
    	g.drawLine(0, testImageHeight-1, 0, testImageHeight-1); // bottom-left
    	g.drawLine(testImageWidth-1, testImageHeight-1, testImageWidth-1, testImageHeight-1); // bottom-right
    	
    	readTestImagePixels();

    	if(!validatePixel(0, 0, BLACK))
    	{
    		pass = false;
    		errorMsg += "top-left, ";
    	}
    	if(!validatePixel(testImageWidth-1, 0, BLACK))
    	{
    		pass = false;
    		errorMsg += "top-right, ";
    	}
    	if(!validatePixel(0, testImageHeight-1, BLACK))
    	{
    		pass = false;
    		errorMsg += "bottom-left, ";
    	}
    	if(!validatePixel(testImageWidth-1, testImageHeight-1, BLACK))
    	{
    		pass = false;
    		errorMsg += "bottom-right.";
    	}
    	
    	if(!pass) 
    	{
    		fail(errorMsg);
    	}
    }
    
    /**
     * Test clip with two different graphics objects 
     */
    void testClip2()
    {	
    	boolean pass = true;
    	String errorMsg = "Pixel check failed at: ";
    	Graphics g1 = testImage.getGraphics();
    	Graphics g2 = testImage.getGraphics();
    	final int[] clip1 = {1,1,testImageWidth-2, testImageHeight-2};
    	final int[] clip2 = {4,4,testImageWidth-9, testImageHeight-9};
    	final int[] clip3 = {8,8,testImageWidth-17, testImageHeight-17};
    	
    	// Set clip one pixel smaller than the test image
    	// and fill whole image with red, using graphics object 1
    	g1.setClip(clip1[0], clip1[1], clip1[2], clip1[3]);
    	g1.setColor(RED);
    	g1.fillRect(0, 0, testImageWidth, testImageHeight);
    	
    	// Set clip four pixels smaller than the test image
    	// and fill whole image with green, using graphics object 2
    	g2.setClip(clip2[0], clip2[1], clip2[2], clip2[3]);
    	g2.setColor(GREEN);
    	g2.fillRect(0, 0, testImageWidth, testImageHeight);
    	
    	// Set clip eight pixels smaller than the test image
    	// and fill whole image with blue, using again graphics object 1
    	g1.setClip(clip3[0], clip3[1], clip3[2], clip3[3]);
    	g1.setColor(BLUE);
    	g1.fillRect(0, 0, testImageWidth, testImageHeight);
    	
     	// read image pixels to member array
    	readTestImagePixels();
    	
    	final int[] spotsToValidate = {
    			clip1[0]-1,        clip1[1],          WHITE,  // left border (outside) of clip 1, white
    			clip1[0]+clip1[2], clip1[1],          WHITE,  // right border (outside) of clip 1, white  
    			clip1[0]+1,        clip1[1]-1,        WHITE,  // top border (outside) of clip 1, white
    			clip1[0]+1,        clip1[1]+clip1[3], WHITE,  // bottom border (outside) of clip 1, white
    			
    			clip1[0],            clip1[1],            RED,  // left border (inside) of clip 1, red
    			clip1[0]+clip1[2]-1, clip1[1],            RED,  // right border (inside) of clip 1, red
    			clip1[0],            clip1[1]+1,          RED,  // top border (inside) of clip 1, red
    			clip1[0]+1,          clip1[1]+clip1[3]-1, RED,  // bottom border (inside) of clip 1, red
    			
    			clip2[0]-1,        clip2[1],          RED,  // left border (outside) of clip 2, red
    			clip2[0]+clip2[2], clip2[1],          RED,  // right border (outside) of clip 2, red  
    			clip2[0]+1,        clip2[1]-1,        RED,  // top border (outside) of clip 2, red
    			clip2[0]+1,        clip2[1]+clip2[3], RED,  // bottom border (outside) of clip 2, red
    			
    			clip2[0],            clip2[1],            GREEN,  // left border (inside) of clip 2, green
    			clip2[0]+clip2[2]-1, clip2[1],            GREEN,  // right border (inside) of clip 2, green
    			clip2[0],            clip2[1]+1,          GREEN,  // top border (inside) of clip 2, green
    			clip2[0]+1,          clip2[1]+clip2[3]-1, GREEN,  // bottom border (inside) of clip 2, green
    			
    			clip3[0]-1,        clip3[1],          GREEN,  // left border (outside) of clip 3, green
    			clip3[0]+clip3[2], clip3[1],          GREEN,  // right border (outside) of clip 3, green  
    			clip3[0]+1,        clip3[1]-1,        GREEN,  // top border (outside) of clip 3, green
    			clip3[0]+1,        clip3[1]+clip3[3], GREEN,  // bottom border (outside) of clip 3, green
    			
    			clip3[0],            clip3[1],            BLUE,  // left border (inside) of clip 3, blue
    			clip3[0]+clip3[2]-1, clip3[1],            BLUE,  // right border (inside) of clip 3, blue
    			clip3[0],            clip3[1]+1,          BLUE,  // top border (inside) of clip 3, blue
    			clip3[0]+1,          clip3[1]+clip3[3]-1, BLUE,  // bottom border (inside) of clip 3, blue
    	};
    	
    	// Validate test points against reference color
    	for(int i = 0; i < spotsToValidate.length ; i=i+3) 
    	{
    		if(!validatePixel(spotsToValidate[i], spotsToValidate[i+1], spotsToValidate[i+2]))
        	{
        		pass = false;
        		errorMsg += "("+spotsToValidate[i]+","+spotsToValidate[i+1]+"), expected "+colorToString(spotsToValidate[i+2])+
        		            ", got "+ colorToString(getPixelIgnoreAlpha(spotsToValidate[i], spotsToValidate[i+1]))+" : ";
        	}
    	}
    	
    	if(!pass) 
    	{
    		fail(errorMsg);
    	}
    }
    
    /**
     * Test drawArc with two graphics 
     */
    void testDrawArc()
    {
    	boolean pass = true;
    	String errorMsg = "Pixel check failed at: ";
    	Graphics g1 = testImage.getGraphics();
    	Graphics g2 = testImage.getGraphics();
    	
    	// run different settings in the graphics instances
    	applyGraphicsSettings(g1, GRAPHICS_SETTINGS_1);
    	applyGraphicsSettings(g2, GRAPHICS_SETTINGS_2);
    	
    	g1.drawArc(0, 0, 8, 8, 0, 180);
    	g2.drawArc(0, 0, 8, 8, 0, 180);
    	g1.drawArc(0, 8, 8, 8, 0, 180);
    	
     	// read image pixels to member array
    	readTestImagePixels();
    	
    	// Most of the points to validate are based on the values 
    	// set by applyGraphicsSettings() -method
    	final int[] spotsToValidate = {
    			1,  9, BLUE, // Starting point of the first arc drawn with g1. Y location is arc y + ((height/2)+1) as specified in lcdui spec
    			9,  9, BLUE, // end point
    			5,  5, BLUE, // tip of the arc 
    			10, 8, RED,  // start arc 2
    			18, 8, RED,  // end arc 2
    			14, 4, RED,  // tip of arc 2
    			1,  17, BLUE, // start arc 3
    			9,  17, BLUE, // end arc 3
    			5,  13, BLUE, // tip of arc 3		
    	};
    	
    	// Validate test points against reference color
    	for(int i = 0; i < spotsToValidate.length ; i=i+3) 
    	{
    		if(!validatePixel(spotsToValidate[i], spotsToValidate[i+1], spotsToValidate[i+2]))
        	{
        		pass = false;
        		errorMsg += "("+spotsToValidate[i]+","+spotsToValidate[i+1]+"), expected "+colorToString(spotsToValidate[i+2])+
        		            ", got "+ colorToString(getPixelIgnoreAlpha(spotsToValidate[i], spotsToValidate[i+1]))+" : ";
        	}
    	}
    	
    	if(!pass) 
    	{
    		fail(errorMsg);
    	}
    }
    
    /**
     * Test drawImage with two graphics
     */
	void testDrawImage()
	{
    	boolean pass = true;
    	String errorMsg = "Pixel check failed at: ";
    	
    	// prepare image to be drawn
    	Image img = Image.createImage(5,5);
    	Graphics ig = img.getGraphics();
    	ig.fillRect(0, 0, 5, 5);
    	
    	Graphics g1 = testImage.getGraphics();
    	Graphics g2 = testImage.getGraphics();
    	
    	// run different settings in the graphics instances
    	applyGraphicsSettings(g1, GRAPHICS_SETTINGS_1);
    	applyGraphicsSettings(g2, GRAPHICS_SETTINGS_2);
    	
    	g1.drawImage(img, 0, 0, 0);
    	g2.drawImage(img, 0, 0, 0);
    	g1.drawImage(img, 0, 8, 0);
    	
     	// read image pixels to member array
    	readTestImagePixels();
    	
    	// Most of the points to validate are based on the values 
    	// set by applyGraphicsSettings() -method
    	final int[] spotsToValidate = {
    			1, 5, BLACK, // top-left corner of image 1
    			5, 5, BLACK, // top-right corner of image 1
    			1, 9, BLACK, // bottom-left corner of image 1 
    		    5, 9, BLACK,  // bottom-right corner of image 1
    		    
    		    0, 7, WHITE, // Left border (outside) of image 1
    		    6, 7, WHITE, // Right border (outside) of image 1
    		    3, 4, WHITE, // Top border (outside) of image 1
    		    3, 10, WHITE, // Bottom border (outside) of image 1
    		    
    			10, 4, BLACK, // top-left corner of image 2
    			14, 4, BLACK, // top-right corner of image 2
    			10, 8, BLACK, // bottom-left corner of image 2 
    		    14, 8, BLACK,  // bottom-right corner of image 2
    		    
    		    9,  6, WHITE, // Left border (outside) of image 2
    		    15, 6, WHITE, // Right border (outside) of image 2
    		    12, 3, WHITE, // Top border (outside) of image 2
    		    12, 9, WHITE, // Bottom border (outside) of image 2
    		    
    			1, 13, BLACK, // top-left corner of image 3
    			5, 13, BLACK, // top-right corner of image 3
    			1, 17, BLACK, // bottom-left corner of image 3 
    		    5, 17, BLACK,  // bottom-right corner of image 3	
    		    
    		    0, 15, WHITE, // Left border (outside) of image 3
    		    6, 15, WHITE, // Right border (outside) of image 3
    		    3, 12, WHITE, // Top border (outside) of image 3
    		    3, 18, WHITE, // Bottom border (outside) of image 3
    	};
    	
    	// Validate test points against reference color
    	for(int i = 0; i < spotsToValidate.length ; i=i+3) 
    	{
    		if(!validatePixel(spotsToValidate[i], spotsToValidate[i+1], spotsToValidate[i+2]))
        	{
        		pass = false;
        		errorMsg += "("+spotsToValidate[i]+","+spotsToValidate[i+1]+"), expected "+colorToString(spotsToValidate[i+2])+
        		            ", got "+ colorToString(getPixelIgnoreAlpha(spotsToValidate[i], spotsToValidate[i+1]))+" : ";
        	}
    	}
    	
    	if(!pass) 
    	{
    		fail(errorMsg);
    	}
	}
	
	void testDrawLine()
	{
    	boolean pass = true;
    	String errorMsg = "Pixel check failed at: ";
    	
    	Graphics g1 = testImage.getGraphics();
    	Graphics g2 = testImage.getGraphics();
    	
    	// run different settings in the graphics instances
    	applyGraphicsSettings(g1, GRAPHICS_SETTINGS_1);
    	applyGraphicsSettings(g2, GRAPHICS_SETTINGS_2);
    	
    	g1.drawLine(0, 0, 0, 7);
    	g2.drawLine(0, 0, 0, 5);
    	g1.drawLine(0, 12, 7, 12);
    	
     	// read image pixels to member array
    	readTestImagePixels();
    	
    	// Most of the points to validate are based on the values 
    	// set by applyGraphicsSettings() -method
    	final int[] spotsToValidate = {
    			// Dotted blue line 1
    			1, 4, WHITE, // point before line 1 start 
    			1, 5, BLUE,  // start of line 1
    			1, 7, WHITE, // middle of line 1, verify that its dotted
    		    1, 11, BLUE,  // end of line 1
    		    1, 12, WHITE, // point after line 1 end
    		    
    		    // Solid red line 2
    			10, 3, WHITE, // point before line 2 start 
    			10, 4, RED,  // start of line 2
    		    10, 9, RED,  // end of line 2
    		    10, 10, WHITE, // point after line 2 end
    		    
    		    // Dotted blue line 3
    			0, 17, WHITE, // point before line 1 start 
    			1, 17, BLUE,  // start of line 1
    			3, 17, WHITE, // middle of line 1, verify that its dotted
    		    7, 17, BLUE,  // end of line 1
    		    8, 17, WHITE, // point after line 1 end
    	};
    	
    	// Validate test points against reference color
    	for(int i = 0; i < spotsToValidate.length ; i=i+3) 
    	{
    		if(!validatePixel(spotsToValidate[i], spotsToValidate[i+1], spotsToValidate[i+2]))
        	{
        		pass = false;
        		errorMsg += "("+spotsToValidate[i]+","+spotsToValidate[i+1]+"), expected "+colorToString(spotsToValidate[i+2])+
        		            ", got "+ colorToString(getPixelIgnoreAlpha(spotsToValidate[i], spotsToValidate[i+1]))+" : ";
        	}
    	}
    	
    	if(!pass) 
    	{
    		fail(errorMsg);
    	}
	}
	
	/**
	 * Test drawRect with two graphics
	 */
	void testDrawRect()	
	{
    	boolean pass = true;
    	String errorMsg = "Pixel check failed at: ";
    	
    	Graphics g1 = testImage.getGraphics();
    	Graphics g2 = testImage.getGraphics();
    	
    	// run different settings in the graphics instances
    	applyGraphicsSettings(g1, GRAPHICS_SETTINGS_1);
    	applyGraphicsSettings(g2, GRAPHICS_SETTINGS_2);
    	
    	g1.drawRect(0, 0, 6, 6);
    	g2.drawRect(0, 0, 6, 6);
    	g1.drawRect(0, 8, 3, 3);
    	
     	// read image pixels to member array
    	readTestImagePixels();
    	//print(pixelData, testImageWidth);
    	
    	// Most of the points to validate are based on the values 
    	// set by applyGraphicsSettings() -method
    	final int[] spotsToValidate = {
    			// Dotted rectangle
    			1, 5, BLUE,  // top-left corner of rectangle 1
    			7, 5, BLUE,  // top-right corner of rectangle 1
    			1, 11, BLUE, // bottom-left corner of rectangle 1 
    		    7, 11, BLUE, // bottom-right corner of rectangle 1
    		    3, 5, WHITE, // middle of one side, verify that its dotted
    		    
    		    0, 7, WHITE, // Left border (outside) of rectangle 1
    		    8, 7, WHITE, // Right border (outside) of rectangle 1
    		    3, 4, WHITE, // Top border (outside) of rectangle 1
    		    3, 12, WHITE, // Bottom border (outside) of rectangle 1
    		    
    		    // Solid rectangle
    			10, 4, RED,  // top-left corner of rectangle 2
    			16, 4, RED,  // top-right corner of rectangle 2
    			10, 10, RED, // bottom-left corner of rectangle 2 
    		    16, 10, RED, // bottom-right corner of rectangle 2
    		    
    		    9, 6, WHITE, // Left border (outside) of rectangle 2
    		    17, 6, WHITE, // Right border (outside) of rectangle 2
    		    12, 3, WHITE, // Top border (outside) of rectangle 2
    		    14, 11, WHITE, // Bottom border (outside) of rectangle 2
    		    
    		    // Dotted rectangle
    			1, 13, BLUE,  // top-left corner of rectangle 3
    			4, 13, BLUE,  // top-right corner of rectangle 3
    			1, 16, BLUE, // bottom-left corner of rectangle 3 
    		    4, 16, BLUE, // bottom-right corner of rectangle 3
    		    3, 13, WHITE, // middle of one side, verify that its dotted
    		    
    		    0, 14, WHITE, // Left border (outside) of rectangle 3
    		    5, 14, WHITE, // Right border (outside) of rectangle 3
    		    3, 12, WHITE, // Top border (outside) of rectangle 3
    		    3, 17, WHITE, // Bottom border (outside) of rectangle 3
    	};
    	
    	// Validate test points against reference color
    	for(int i = 0; i < spotsToValidate.length ; i=i+3) 
    	{
    		if(!validatePixel(spotsToValidate[i], spotsToValidate[i+1], spotsToValidate[i+2]))
        	{
        		pass = false;
        		errorMsg += "("+spotsToValidate[i]+","+spotsToValidate[i+1]+"), expected "+colorToString(spotsToValidate[i+2])+
        		            ", got "+ colorToString(getPixelIgnoreAlpha(spotsToValidate[i], spotsToValidate[i+1]))+" : ";
        	}
    	}
    	
    	if(!pass) 
    	{
    		fail(errorMsg);
    	}	
	}
	
	/**
	 * Test drawRGB with two graphics
	 */
	void testDrawRGB() 
	{
    	boolean pass = true;
    	String errorMsg = "Pixel check failed at: ";
    	
    	// prepare test rgbdata
    	final int r = 0xffff0000;
    	final int b = 0xff0000ff;
    	final int[] rgb1 = {
    			r,r,r,r,r,
    			r,r,r,r,r,
    			r,r,r,r,r,
    			r,r,r,r,r,
    			r,r,r,r,r
    	};
    	
    	final int[] rgb2 = {
    			b,b,b,b,b,
    			b,b,b,b,b,
    			b,b,b,b,b,
    			b,b,b,b,b,
    			b,b,b,b,b
    	};
    	
    	Graphics g1 = testImage.getGraphics();
    	Graphics g2 = testImage.getGraphics();
    	
    	// run different settings in the graphics instances
    	applyGraphicsSettings(g1, GRAPHICS_SETTINGS_1);
    	applyGraphicsSettings(g2, GRAPHICS_SETTINGS_2);
    	
    	g1.drawRGB(rgb1, 0, 5, 0, 0, 5, 5, false);
    	g2.drawRGB(rgb2, 0, 5, 0, 0, 5, 5, false);
    	g1.drawRGB(rgb1, 0, 5, 0, 8, 5, 5, false);
    	
     	// read image pixels to member array
    	readTestImagePixels();
    	//print(pixelData, testImageWidth);
    	
    	// Most of the points to validate are based on the values 
    	// set by applyGraphicsSettings() -method
    	final int[] spotsToValidate = {
    			1, 5, RED,  // top-left corner of rectangle 1
    			5, 5, RED,  // top-right corner of rectangle 1
    			1, 9, RED, // bottom-left corner of rectangle 1 
    		    5, 9, RED, // bottom-right corner of rectangle 1
    		    
    		    0, 7, WHITE, // Left border (outside) of rectangle 1
    		    6, 7, WHITE, // Right border (outside) of rectangle 1
    		    3, 4, WHITE, // Top border (outside) of rectangle 1
    		    3, 10, WHITE, // Bottom border (outside) of rectangle 1
    		    
    			10, 4, BLUE,  // top-left corner of rectangle 2
    			14, 4, BLUE,  // top-right corner of rectangle 2
    			10, 8, BLUE, // bottom-left corner of rectangle 2 
    		    14, 8, BLUE, // bottom-right corner of rectangle 2
    		    
    		    9,  6, WHITE, // Left border (outside) of rectangle 2
    		    15, 6, WHITE, // Right border (outside) of rectangle 2
    		    12, 3, WHITE, // Top border (outside) of rectangle 2
    		    12, 9, WHITE, // Bottom border (outside) of rectangle 2
    		    
    			1, 13, RED,  // top-left corner of rectangle 3
    			5, 13, RED,  // top-right corner of rectangle 3
    			1, 17, RED, // bottom-left corner of rectangle 3 
    		    5, 17, RED, // bottom-right corner of rectangle 3
    		    
    		    0, 14, WHITE, // Left border (outside) of rectangle 3
    		    6, 14, WHITE, // Right border (outside) of rectangle 3
    		    3, 12, WHITE, // Top border (outside) of rectangle 3
    		    3, 18, WHITE, // Bottom border (outside) of rectangle 3
    	};
    	
    	// Validate test points against reference color
    	for(int i = 0; i < spotsToValidate.length ; i=i+3) 
    	{
    		if(!validatePixel(spotsToValidate[i], spotsToValidate[i+1], spotsToValidate[i+2]))
        	{
        		pass = false;
        		errorMsg += "("+spotsToValidate[i]+","+spotsToValidate[i+1]+"), expected "+colorToString(spotsToValidate[i+2])+
        		            ", got "+ colorToString(getPixelIgnoreAlpha(spotsToValidate[i], spotsToValidate[i+1]))+" : ";
        	}
    	}
    	
    	if(!pass) 
    	{
    		fail(errorMsg);
    	}	
	}
	
	/**
	 * Test drawRoundRect with two graphics
	 */
	void testDrawRoundRect()
	{
	 	boolean pass = true;
    	String errorMsg = "Pixel check failed at: ";
    	
    	Graphics g1 = testImage.getGraphics();
    	Graphics g2 = testImage.getGraphics();
    	
    	// run different settings in the graphics instances
    	applyGraphicsSettings(g1, GRAPHICS_SETTINGS_1);
    	applyGraphicsSettings(g2, GRAPHICS_SETTINGS_2);
    	
    	g1.drawRoundRect(0, 0, 6, 6, 2, 2);
    	g2.drawRoundRect(0, 0, 6, 6, 2, 2);
    	g1.drawRoundRect(0, 8, 5, 5, 2, 2);
    	
     	// read image pixels to member array
    	readTestImagePixels();
    	//print(pixelData, testImageWidth);
    	
    	// Most of the points to validate are based on the values 
    	// set by applyGraphicsSettings() -method
    	final int[] spotsToValidate = {
    			// Dotted rectangle
    			1, 6, BLUE,  // left side of rectangle 1
    			7, 6, BLUE,  // right side of rectangle 1
    			3, 5, BLUE, // top side of rectangle 1 
    		    3, 11, BLUE, // bottom side of rectangle 1
    		    5, 5, WHITE, // middle of one side, verify that its dotted
    		    
    		    0, 7, WHITE, // Left border (outside) of rectangle 1
    		    7, 7, WHITE, // Right border (outside) of rectangle 1
    		    3, 4, WHITE, // Top border (outside) of rectangle 1
    		    3, 12, WHITE, // Bottom border (outside) of rectangle 1
    		    
    		    // Solid rectangle
    			10, 6, RED,  // left side of rectangle 2
    			16, 6, RED,  // right side of rectangle 2
    			12, 4, RED, // top side of rectangle 2 
    		    12, 10, RED, // bottom side corner of rectangle 2
    		    
    		    9, 6, WHITE, // Left border (outside) of rectangle 2
    		    17, 6, WHITE, // Right border (outside) of rectangle 2
    		    12, 3, WHITE, // Top border (outside) of rectangle 2
    		    14, 11, WHITE, // Bottom border (outside) of rectangle 2
    		    
    		    // Dotted rectangle
    			1, 15, BLUE,  // left side of of rectangle 3
    			6, 14, BLUE,  // right side of of rectangle 3
    			2, 13, BLUE, // top side of rectangle 3 
    		    2, 18, BLUE, // bottom side of rectangle 3
    		    1,  4, WHITE, // middle of one side, verify that its dotted
    		    
    		    0, 14, WHITE, // Left border (outside) of rectangle 3
    		    7, 14, WHITE, // Right border (outside) of rectangle 3
    		    3, 12, WHITE, // Top border (outside) of rectangle 3
    		    3, 19, WHITE, // Bottom border (outside) of rectangle 3
    	};
    	
    	// Validate test points against reference color
    	for(int i = 0; i < spotsToValidate.length ; i=i+3) 
    	{
    		if(!validatePixel(spotsToValidate[i], spotsToValidate[i+1], spotsToValidate[i+2]))
        	{
        		pass = false;
        		errorMsg += "("+spotsToValidate[i]+","+spotsToValidate[i+1]+"), expected "+colorToString(spotsToValidate[i+2])+
        		            ", got "+ colorToString(getPixelIgnoreAlpha(spotsToValidate[i], spotsToValidate[i+1]))+" : ";
        	}
    	}
    	
    	if(!pass) 
    	{
    		fail(errorMsg);
    	}	
	}
	
	/**
	 * Test drawString with two graphics
	 */
	void testDrawString()
	{
	 	boolean pass = true;
    	String errorMsg = "Pixel check failed at: ";
    	
    	Graphics g1 = testImage.getGraphics();
    	Graphics g2 = testImage.getGraphics();
    	
    	// run different settings in the graphics instances
    	applyGraphicsSettings(g1, GRAPHICS_SETTINGS_1);
    	applyGraphicsSettings(g2, GRAPHICS_SETTINGS_2);
    	
    	final int anchor = Graphics.TOP | Graphics.LEFT;
    	
    	g1.drawString("I", 0, -9, anchor);
    	g2.drawString("I", -2, -3, anchor);
    	g1.drawString("I", 11, -9, anchor);

     	// read image pixels to member array
    	readTestImagePixels();
    	
    	//print(pixelData, testImageWidth);
    	
    	// Most of the points to validate are based on the values 
    	// set by applyGraphicsSettings() -method
    	final int[] spotsToValidate = {
    			// "I" with large font 1
    		  3, 1, BLUE,  // top inside 
    		  3, 19, BLUE, // bottom inside
    		  3, 0, WHITE, // top outside
    		    
    		  // "I" with small font
    		  10, 4, RED, // top inside
    		  10, 15, RED, // bottom inside
    		  10,  3, WHITE, // top outside
    		  10,  16, WHITE, // bottom outside
    		    
    		  // "I" with large font 2
    		  15, 1, BLUE,  // top inside 
    		  15, 19, BLUE, // bottom inside
    		  15, 0, WHITE // top outside
    	};
    	
    	// Validate test points against reference color
    	for(int i = 0; i < spotsToValidate.length ; i=i+3) 
    	{
    		if(!validatePixel(spotsToValidate[i], spotsToValidate[i+1], spotsToValidate[i+2]))
        	{
        		pass = false;
        		errorMsg += "("+spotsToValidate[i]+","+spotsToValidate[i+1]+"), expected "+colorToString(spotsToValidate[i+2])+
        		            ", got "+ colorToString(getPixelIgnoreAlpha(spotsToValidate[i], spotsToValidate[i+1]))+" : ";
        	}
    	}
    	
    	if(!pass) 
    	{
    		fail(errorMsg);
    	}
	}
	
	/**
	 * Test fillArc with two graphics
	 */
	void testFillArc()
	{
    	boolean pass = true;
    	String errorMsg = "Pixel check failed at: ";
    	Graphics g1 = testImage.getGraphics();
    	Graphics g2 = testImage.getGraphics();
    	
    	// run different settings in the graphics instances
    	applyGraphicsSettings(g1, GRAPHICS_SETTINGS_1);
    	applyGraphicsSettings(g2, GRAPHICS_SETTINGS_2);
    	
    	g1.fillArc(0, 0, 8, 8, 0, 180);
    	g2.fillArc(0, 0, 8, 8, 0, 180);
    	g1.fillArc(0, 8, 8, 8, 0, 180);
    	
     	// read image pixels to member array
    	readTestImagePixels();
    	
    	// Most of the points to validate are based on the values 
    	// set by applyGraphicsSettings() -method
    	final int[] spotsToValidate = {
    			2,  8, BLUE, // Starting point of the first arc 
    			8,  8, BLUE, // end point
    			5,  6, BLUE, // tip of the arc 
    			11, 7, RED,  // start arc 2
    			17, 7, RED,  // end arc 2
    			14, 5, RED,  // tip of arc 2
    			2,  16, BLUE, // start arc 3
    			8,  16, BLUE, // end arc 3
    			5,  14, BLUE, // tip of arc 3	
    			1,  1, WHITE // one point to validate that whole image is not filled
    	};
    	
    	// Validate test points against reference color
    	for(int i = 0; i < spotsToValidate.length ; i=i+3) 
    	{
    		if(!validatePixel(spotsToValidate[i], spotsToValidate[i+1], spotsToValidate[i+2]))
        	{
        		pass = false;
        		errorMsg += "("+spotsToValidate[i]+","+spotsToValidate[i+1]+"), expected "+colorToString(spotsToValidate[i+2])+
        		            ", got "+ colorToString(getPixelIgnoreAlpha(spotsToValidate[i], spotsToValidate[i+1]))+" : ";
        	}
    	}
    	
    	if(!pass) 
    	{
    		fail(errorMsg);
    	}
	}
	
	/**
	 * Test fillRect with two graphics
	 */
	void testFillRect()
	{
		boolean pass = true;
    	String errorMsg = "Pixel check failed at: ";
    	
    	Graphics g1 = testImage.getGraphics();
    	Graphics g2 = testImage.getGraphics();
    	
    	// run different settings in the graphics instances
    	applyGraphicsSettings(g1, GRAPHICS_SETTINGS_1);
    	applyGraphicsSettings(g2, GRAPHICS_SETTINGS_2);
    	
    	g1.fillRect(0, 0, 6, 6);
    	g2.fillRect(0, 0, 6, 6);
    	g1.fillRect(0, 8, 3, 3);
    	
     	// read image pixels to member array
    	readTestImagePixels();
    	
    	// Most of the points to validate are based on the values 
    	// set by applyGraphicsSettings() -method
    	final int[] spotsToValidate = {
    			// filled rectangle 1
    			1, 5, BLUE,  // top-left corner of rectangle 1
    			6, 5, BLUE,  // top-right corner of rectangle 1
    			1, 10, BLUE, // bottom-left corner of rectangle 1 
    		    6, 10, BLUE, // bottom-right corner of rectangle 1
    		    
    		    0, 7, WHITE, // Left border (outside) of rectangle 1
    		    7, 7, WHITE, // Right border (outside) of rectangle 1
    		    3, 4, WHITE, // Top border (outside) of rectangle 1
    		    3, 11, WHITE, // Bottom border (outside) of rectangle 1
    		    
    		    // filled rectangle 2
    			10, 4, RED,  // top-left corner of rectangle 2
    			15, 4, RED,  // top-right corner of rectangle 2
    			10, 9, RED, // bottom-left corner of rectangle 2 
    		    15, 9, RED, // bottom-right corner of rectangle 2
    		    
    		    9, 6, WHITE, // Left border (outside) of rectangle 2
    		    16, 6, WHITE, // Right border (outside) of rectangle 2
    		    12, 3, WHITE, // Top border (outside) of rectangle 2
    		    14, 10, WHITE, // Bottom border (outside) of rectangle 2
    		    
    		    // filled rectangle 3
    			1, 13, BLUE,  // top-left corner of rectangle 3
    			3, 13, BLUE,  // top-right corner of rectangle 3
    			1, 15, BLUE, // bottom-left corner of rectangle 3 
    		    3, 15, BLUE, // bottom-right corner of rectangle 3
    		    
    		    0, 14, WHITE, // Left border (outside) of rectangle 3
    		    4, 14, WHITE, // Right border (outside) of rectangle 3
    		    3, 12, WHITE, // Top border (outside) of rectangle 3
    		    3, 16, WHITE, // Bottom border (outside) of rectangle 3
    	};
    	
    	// Validate test points against reference color
    	for(int i = 0; i < spotsToValidate.length ; i=i+3) 
    	{
    		if(!validatePixel(spotsToValidate[i], spotsToValidate[i+1], spotsToValidate[i+2]))
        	{
        		pass = false;
        		errorMsg += "("+spotsToValidate[i]+","+spotsToValidate[i+1]+"), expected "+colorToString(spotsToValidate[i+2])+
        		            ", got "+ colorToString(getPixelIgnoreAlpha(spotsToValidate[i], spotsToValidate[i+1]))+" : ";
        	}
    	}
    	
    	if(!pass) 
    	{
    		fail(errorMsg);
    	}	
	}
	
	/**
	 * Test fillRoundRect with two graphics
	 */
	void testFillRoundRect()
	{
	 	boolean pass = true;
    	String errorMsg = "Pixel check failed at: ";
    	
    	Graphics g1 = testImage.getGraphics();
    	Graphics g2 = testImage.getGraphics();
    	
    	// run different settings in the graphics instances
    	applyGraphicsSettings(g1, GRAPHICS_SETTINGS_1);
    	applyGraphicsSettings(g2, GRAPHICS_SETTINGS_2);
    	
    	g1.fillRoundRect(0, 0, 6, 6, 2, 2);
    	g2.fillRoundRect(0, 0, 6, 6, 2, 2);
    	g1.fillRoundRect(0, 8, 5, 5, 2, 2);
    	
     	// read image pixels to member array
    	readTestImagePixels();
    	
    	// Most of the points to validate are based on the values 
    	// set by applyGraphicsSettings() -method
    	final int[] spotsToValidate = {
    			// rectangle 1
    			1, 6, BLUE,  // left side of rectangle 1
    			6, 6, BLUE,  // right side of rectangle 1
    			3, 5, BLUE, // top side of rectangle 1 
    		    3, 10, BLUE, // bottom side of rectangle 1
    		    
    		    0, 7, WHITE, // Left border (outside) of rectangle 1
    		    7, 7, WHITE, // Right border (outside) of rectangle 1
    		    3, 4, WHITE, // Top border (outside) of rectangle 1
    		    3, 11, WHITE, // Bottom border (outside) of rectangle 1
    		    
    		    // rectangle 2
    			10, 6, RED,  // left side of rectangle 2
    			15, 5, RED,  // right side of rectangle 2
    			12, 4, RED, // top side of rectangle 2 
    		    12, 9, RED, // bottom side corner of rectangle 2
    		    
    		    9, 6, WHITE, // Left border (outside) of rectangle 2
    		    16, 6, WHITE, // Right border (outside) of rectangle 2
    		    12, 3, WHITE, // Top border (outside) of rectangle 2
    		    14, 10, WHITE, // Bottom border (outside) of rectangle 2
    		    
    		    // rectangle 3
    			1, 15, BLUE,  // left side of of rectangle 3
    			5, 14, BLUE,  // right side of of rectangle 3
    			2, 13, BLUE, // top side of rectangle 3 
    		    3, 17, BLUE, // bottom side of rectangle 3
    		    
    		    0, 14, WHITE, // Left border (outside) of rectangle 3
    		    6, 14, WHITE, // Right border (outside) of rectangle 3
    		    3, 12, WHITE, // Top border (outside) of rectangle 3
    		    3, 18, WHITE, // Bottom border (outside) of rectangle 3
    	};
    	
    	// Validate test points against reference color
    	for(int i = 0; i < spotsToValidate.length ; i=i+3) 
    	{
    		if(!validatePixel(spotsToValidate[i], spotsToValidate[i+1], spotsToValidate[i+2]))
        	{
        		pass = false;
        		errorMsg += "("+spotsToValidate[i]+","+spotsToValidate[i+1]+"), expected "+colorToString(spotsToValidate[i+2])+
        		            ", got "+ colorToString(getPixelIgnoreAlpha(spotsToValidate[i], spotsToValidate[i+1]))+" : ";
        	}
    	}
    	
    	if(!pass) 
    	{
    		fail(errorMsg);
    	}	
	}
	
	/**
	 * Test fillTriangle with two graphics
	 */
	void testFillTriangle()
	{
	 	boolean pass = true;
    	String errorMsg = "Pixel check failed at: ";
    	
    	Graphics g1 = testImage.getGraphics();
    	Graphics g2 = testImage.getGraphics();
    	
    	// run different settings in the graphics instances
    	applyGraphicsSettings(g1, GRAPHICS_SETTINGS_1);
    	applyGraphicsSettings(g2, GRAPHICS_SETTINGS_2);
    	
    	g1.fillTriangle(0, 0, 5, 6, 5, 0);
    	g2.fillTriangle(0, 0, 5, 6, 5, 0);
    	g1.fillTriangle(0, 8, 5, 14, 5, 8);
    	
     	// read image pixels to member array
    	readTestImagePixels();
    	
    	// Most of the points to validate are based on the values 
    	// set by applyGraphicsSettings() -method
    	final int[] spotsToValidate = {
    			// triangle 1
    			1, 5, BLUE,  // top-left
    			6, 5, BLUE,  // top-right
    			4, 4, WHITE, // outside near triangle 1
    		    
    			// triangle 2
    			10, 4, RED,  // top-left
    			15, 4, RED,  // top-right
    			13, 3, WHITE, // outside near triangle 1
    		    
    			// triangle 3
    			1, 13, BLUE,  // top-left
    			6, 13, BLUE,  // top-right
    			4, 12, WHITE, // outside near triangle 1
    	};
    	
    	// Validate test points against reference color
    	for(int i = 0; i < spotsToValidate.length ; i=i+3) 
    	{
    		if(!validatePixel(spotsToValidate[i], spotsToValidate[i+1], spotsToValidate[i+2]))
        	{
        		pass = false;
        		errorMsg += "("+spotsToValidate[i]+","+spotsToValidate[i+1]+"), expected "+colorToString(spotsToValidate[i+2])+
        		            ", got "+ colorToString(getPixelIgnoreAlpha(spotsToValidate[i], spotsToValidate[i+1]))+" : ";
        	}
    	}
    	
    	if(!pass) 
    	{
    		fail(errorMsg);
    	}
	}
  
	private void applyGraphicsSettings(Graphics g, int settings) 
	{
		
		Font defaultFont = Font.getDefaultFont();
		if(settings == GRAPHICS_SETTINGS_1)
		{
			Font font = Font.getFont(defaultFont.getFace(), defaultFont.getStyle(), Font.SIZE_LARGE);
			g.setColor(BLUE);
			g.translate(1, 5);
			g.setStrokeStyle(Graphics.DOTTED);
			g.setFont(font);
		}
		else if(settings == GRAPHICS_SETTINGS_2)
		{
			Font font = Font.getFont(defaultFont.getFace(), defaultFont.getStyle(), Font.SIZE_SMALL);
			g.setColor(RED);
			g.translate(10, 4);
			g.setStrokeStyle(Graphics.SOLID);
			g.setFont(font);
		}
	}
	
    /**
     * Reads all pixels of the testImage to the class member array pixelData
     */
    private void readTestImagePixels()
    {
    	testImage.getRGB(pixelData, 0, testImageWidth, 0, 0, testImageWidth, testImageHeight);
    }
    
	void print(int[] array, int scanlength) {
		final int width = scanlength;
		String output = "";
		
		int y = 0;
		for(int i = 0; i < array.length; i++) {
			
			int a = ((array[i] >> 24) & 0xff);
			int r = ((array[i] >> 16) & 0xff); 
			int g = ((array[i] >> 8 ) & 0xff);
			int b = ( array[i] & 0xff);
			
			if (a == 255 && r == 0 && g == 0 && b == 0) {
				output += "B,";
			} else if (a == 255 && r == 255 && g == 255 && b == 255) {
				output += "0,";
			}
			else if(a == 255 && r == 255 && g == 0 && b == 0) {
				output += "r,";
			}
			else if(a == 255 && r == 0 && g == 255 && b == 0) {
				output += "g,";
			}
			else if(a == 255 && r == 0 && g == 0 && b == 255) {
				output += "b,";
			}
			else {
				//output += "x,";
				output += "("+a+","+r+","+g+","+b+")";
			}
			
			if(i==((y*width) + (width-1))) {
				output += "\n";
				y++;
			}
		}
		System.out.println(output);
	}
    
    
    /**
     * Validates that the pixel in given coordinate equals given color and ignores alpha.
     * @return True if given pixel has given refColor value, otherwise false
     */
    private boolean validatePixel(final int x, final int y, final int refColor)
    {
    	final int sweetSpot = pixelData[(testImageWidth*y)+x];
    	final int[] sweetSpotComponents = getColorComponents(sweetSpot);
    	final int[] refColorComponents = getColorComponents(refColor);
    	
    	if((sweetSpotComponents[1] == refColorComponents[1]) &&    // red
    		(sweetSpotComponents[2] == refColorComponents[2]) &&   // green 
    		(sweetSpotComponents[3] == refColorComponents[3]))     // blue
    	{
    		return true;
    	}
    	return false;
    }
    
    private int getPixel(int x, int y)
    {
    	return pixelData[(testImageWidth*y)+x];
    }
    
    private int getPixelIgnoreAlpha(int x, int y) 
    {
    	int pixel = pixelData[(testImageWidth*y)+x];
    	return pixel & 0x00ffffff; // mask alpha to zero
    }
    
    private String colorToString(final int color) 
    {
    	int[] c = getColorComponents(color);
    	return new String("("+c[0]+","+c[1]+","+c[2]+","+c[3]+") ");
    }
    
    private int[] getColorComponents(final int color) 
    {
    	final int a = ((color >> 24) & 0xff);
    	final int r = ((color >> 16) & 0xff); 
    	final int g = ((color >> 8 ) & 0xff);
    	final int b = ( color & 0xff);
    	return new int[] {a, r, g, b};
    }
    
    private void printColor(final int color) {
    	int[] components = getColorComponents(color);
    	System.out.println("Color components ("+color+"): r="+components[0]+", g="+components[1]+
    			            ", b="+components[2]+", a="+components[3]);
    }

}
