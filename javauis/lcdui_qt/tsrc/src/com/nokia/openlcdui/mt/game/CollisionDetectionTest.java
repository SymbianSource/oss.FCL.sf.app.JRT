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
package com.nokia.openlcdui.mt.game;

import junit.framework.*;

import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.Sprite;
import javax.microedition.lcdui.game.TiledLayer;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * Unit tests for LCDUI Sprite collision detection.
 */
public class CollisionDetectionTest extends SWTTestCase {

    /**
     * Constructor.
     */
    public CollisionDetectionTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public CollisionDetectionTest(String sTestName) {
        super(sTestName);
    }

    /**
     * To create the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods.
     *
     * @return new testsuite.
     */
    public static Test suite() {
		TestSuite suite = new TestSuite();

	    java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = CollisionDetectionTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new CollisionDetectionTest((String)e.nextElement()));
	    }

		return suite;
	}

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testSpriteImageCollision");
        methodNames.addElement("testSpriteSpriteCollision");
        methodNames.addElement("testSpriteTiledLayerCollision");
        methodNames.addElement("testCollisionTransform");
        return methodNames;
    }
    
    protected void runTest() throws Throwable {
        if (getName().equals("testSpriteImageCollision")) testSpriteImageCollision();
        else if (getName().equals("testSpriteSpriteCollision")) testSpriteSpriteCollision();
        else if (getName().equals("testSpriteTiledLayerCollision")) testSpriteTiledLayerCollision();
        else if (getName().equals("testCollisionTransform")) testCollisionTransform();
        else super.runTest();
    }
    
    /** Tests collision between sprite and image.
     */
    public void testSpriteImageCollision() {
        Image image1 = null;
        Image image2 = null;
        try {
            //100x100, 0,0, -> 50,50 full opaque, 0,50 -> 50,70 partially
            //transparent, other area full transparent.
            image1 = Image.createImage("opaqueTest.png");
            image2 = Image.createImage("opaqueTest.png");
        }
        catch (java.io.IOException e) {
            System.out.println(e);
            fail("" + e);
        }

        Sprite sprite1 = new Sprite(image1);

        // Test non-overlapping sprite and image:
        sprite1.setPosition(100, 100);
        assertTrue("sprite-image fail 1.", !sprite1.collidesWith(image2, 100, 0, false));
        assertTrue("sprite-image fail 2.", !sprite1.collidesWith(image2, 100, 0, true));

        assertTrue("sprite-image fail 3.", !sprite1.collidesWith(image2, 100, 200, false));
        assertTrue("sprite-image fail 4.", !sprite1.collidesWith(image2, 100, 200, true));

        assertTrue("sprite-image fail 5.", !sprite1.collidesWith(image2, 0, 100, false));
        assertTrue("sprite-image fail 6.", !sprite1.collidesWith(image2, 0, 100, true));

        assertTrue("sprite-image fail 7.", !sprite1.collidesWith(image2, 200, 100, false));
        assertTrue("sprite-image fail 8.", !sprite1.collidesWith(image2, 200, 100, true));

        // Test overlapping sprite and image:
        sprite1.setPosition(0, 0);
        assertTrue("sprite-image fail 9.", sprite1.collidesWith(image2, 0, 0, false));
        assertTrue("sprite-image fail 10.", sprite1.collidesWith(image2, 0, 0, true));

        // Test overlapping transparent sprite and image:
        assertTrue("sprite-image fail 11.", sprite1.collidesWith(image2, 50, 0, false));
        assertTrue("sprite-image fail 12.", !sprite1.collidesWith(image2, 50, 0, true));

        // Test collisionarea-settings:
        sprite1.defineCollisionRectangle(0, 0, 50, 100);
        assertTrue("sprite-image fail 13.", sprite1.collidesWith(image2, 0, 0, false));
        assertTrue("sprite-image fail 14.", sprite1.collidesWith(image2, 0, 0, true));
        sprite1.defineCollisionRectangle(50, 0, 50, 100);
        assertTrue("sprite-image fail 15.", sprite1.collidesWith(image2, 0, 0, false));
        assertTrue("sprite-image fail 16.", !sprite1.collidesWith(image2, 0, 0, true));
        sprite1.defineCollisionRectangle(0, 0, 50, 100);
        assertTrue("sprite-image fail 17.", !sprite1.collidesWith(image2, 50, 0, false));
        assertTrue("sprite-image fail 18.", !sprite1.collidesWith(image2, 50, 0, true));

        sprite1.defineCollisionRectangle(0, 0, 100, 100);

        // Test invisible sprite:
        sprite1.setVisible(false);
        assertTrue("sprite-image fail 19.", !sprite1.collidesWith(image2, 0, 0, false));
        sprite1.setVisible(true);

        // Test framed sprites:
        // 2x2 = 4 frames.
        Sprite sprite3 = new Sprite(image1, 50, 50);
        sprite3.setPosition(0, 0);
        assertTrue("sprite-image fail 20.", sprite3.collidesWith(image2, 0, 0, false));
        assertTrue("sprite-image fail 21.", sprite3.collidesWith(image2, 0, 0, true));
        sprite3.nextFrame();
        assertTrue("sprite-image fail 22.", sprite3.collidesWith(image2, 0, 0, false));
        assertTrue("sprite-image fail 23.", !sprite3.collidesWith(image2, 0, 0, true));

        // Test rotated sprite:
        sprite1.setTransform(Sprite.TRANS_ROT90);
        sprite1.setPosition(0, 0);
        assertTrue("sprite-image fail 24.", sprite1.collidesWith(image2, 0, 0, false));
        assertTrue("sprite-image fail 25.", sprite1.collidesWith(image2, 0, 0, true));

        sprite1.setTransform(Sprite.TRANS_ROT180);
        sprite1.setPosition(0, 0);
        assertTrue("sprite-image fail 26.", sprite1.collidesWith(image2, 0, 0, false));
        assertTrue("sprite-image fail 27.", !sprite1.collidesWith(image2, 0, 0, true));

        // Test collision rectangle in transformed sprite:
        sprite1.setTransform(Sprite.TRANS_NONE);
        sprite1.defineCollisionRectangle(0, 0, 50, 50);
        sprite1.setTransform(Sprite.TRANS_ROT90);
        sprite1.setPosition(0, 0);
        assertTrue("sprite-image fail 28.", sprite1.collidesWith(image2, 0, 0, false));
        assertTrue("sprite-image fail 29.", !sprite1.collidesWith(image2, 0, 0, true));
    }

    /** Tests collision between two sprites.
     */
    public void testSpriteSpriteCollision() {
        Image image1 = null;
        Image image2 = null;
        try {
            //100x100, 0,0, -> 50,50 full opaque, 0,50 -> 50,70 partially
            //transparent, other area full transparent.
            image1 = Image.createImage("opaqueTest.png");
            image2 = Image.createImage("opaqueTest.png");
        }
        catch (java.io.IOException e) {
            System.out.println(e);
            fail("" + e);
        }

        Sprite sprite1 = new Sprite(image1);
        Sprite sprite2 = new Sprite(image2);

        // Test non-overlapping sprites:
        sprite1.setPosition(100, 100);
        sprite2.setPosition(100, 0);
        assertTrue("sprite-sprite fail 1.", !sprite1.collidesWith(sprite2, false));
        assertTrue("sprite-sprite fail 2.", !sprite1.collidesWith(sprite2, true));

        sprite2.setPosition(100, 200);
        assertTrue("sprite-sprite fail 3.", !sprite1.collidesWith(sprite2, false));
        assertTrue("sprite-sprite fail 4.", !sprite1.collidesWith(sprite2, true));

        sprite2.setPosition(0, 100);
        assertTrue("sprite-sprite fail 5.", !sprite1.collidesWith(sprite2, false));
        assertTrue("sprite-sprite fail 6.", !sprite1.collidesWith(sprite2, true));

        sprite2.setPosition(200, 100);
        assertTrue("sprite-sprite fail 7.", !sprite1.collidesWith(sprite2, false));
        assertTrue("sprite-sprite fail 8.", !sprite1.collidesWith(sprite2, true));

        // Test overlapping sprites:
        sprite1.setPosition(0, 0);
        sprite2.setPosition(0, 0);
        assertTrue("sprite-sprite fail 9.", sprite1.collidesWith(sprite2, false));
        assertTrue("sprite-sprite fail 10.", sprite1.collidesWith(sprite2, true));

        // Test overlapping transparent sprites:
        sprite2.setPosition(50, 0);
        assertTrue("sprite-sprite fail 11.", sprite1.collidesWith(sprite2, false));
        assertTrue("sprite-sprite fail 12.", !sprite1.collidesWith(sprite2, true));

        // Test collisionarea-settings:
        sprite2.setPosition(0, 0);
        sprite1.defineCollisionRectangle(0, 0, 50, 100);
        assertTrue("sprite-sprite fail 13.", sprite1.collidesWith(sprite2, false));
        assertTrue("sprite-sprite fail 14.", sprite1.collidesWith(sprite2, true));
        sprite1.defineCollisionRectangle(50, 0, 50, 100);
        assertTrue("sprite-sprite fail 15.", sprite1.collidesWith(sprite2, false));
        assertTrue("sprite-sprite fail 16.", !sprite1.collidesWith(sprite2, true));
        sprite1.defineCollisionRectangle(0, 0, 50, 100);
        sprite2.setPosition(50, 0);
        assertTrue("sprite-sprite fail 17.", !sprite1.collidesWith(sprite2, false));
        assertTrue("sprite-sprite fail 18.", !sprite1.collidesWith(sprite2, true));

        sprite1.defineCollisionRectangle(0, 0, 100, 100);

        // Test invisible sprite:
        sprite1.setPosition(0, 0);
        sprite2.setPosition(0, 0);
        sprite1.setVisible(false);
        assertTrue("sprite-sprite fail 19.", !sprite1.collidesWith(sprite2, false));
        sprite1.setVisible(true);

        // Test framed sprites:
        // 2x2 = 4 frames.
        Sprite sprite3 = new Sprite(image1, 50, 50);
        sprite3.setPosition(0, 0);
        assertTrue("sprite-sprite fail 20.", sprite3.collidesWith(sprite1, false));
        assertTrue("sprite-sprite fail 21.", sprite3.collidesWith(sprite1, true));
        sprite3.nextFrame();
        assertTrue("sprite-sprite fail 22.", sprite3.collidesWith(sprite1, false));

        sprite3.setPosition(10, 20);
        sprite1.setPosition(10, 20);
        assertTrue("sprite-sprite fail 23.", !sprite3.collidesWith(sprite1, true));

        // Test rotated sprite:
        sprite1.setTransform(Sprite.TRANS_ROT90);
        sprite1.setPosition(0, 0);
        assertTrue("sprite-sprite fail 24.", sprite1.collidesWith(sprite2, false));
        assertTrue("sprite-sprite fail 25.", sprite1.collidesWith(sprite2, true));

        sprite1.setTransform(Sprite.TRANS_ROT180);
        sprite1.setPosition(0, 0);
        assertTrue("sprite-sprite fail 26.", sprite1.collidesWith(sprite2, false));
        assertTrue("sprite-sprite fail 27.", !sprite1.collidesWith(sprite2, true));

        // Test collision rectangle in transformed sprite:
        sprite1.setTransform(Sprite.TRANS_NONE);
        sprite1.defineCollisionRectangle(0, 0, 50, 50);
        sprite1.setTransform(Sprite.TRANS_ROT90);
        sprite1.setPosition(0, 0);
        assertTrue("sprite-sprite fail 28.", sprite1.collidesWith(sprite2, false));
        assertTrue("sprite-sprite fail 29.", !sprite1.collidesWith(sprite2, true));
    }

    /** Tests collision between sprite and TiledLayer.
     */
    public void testSpriteTiledLayerCollision() {
        Image image1 = null;
        Image image2 = null;
        TiledLayer tiledLayer = null;
        try {
            //100x100, 0,0, -> 50,50 full opaque, 0,50 -> 50,70 partially
            //transparent, other area full transparent.
            image1 = Image.createImage("opaqueTest.png");
            image2 = Image.createImage("opaqueTest.png");
        }
        catch (java.io.IOException e) {
            fail(e.toString());
        }

        Sprite sprite1 = new Sprite(image1);
        tiledLayer = new TiledLayer(2, 2, image2, 50, 50);
        tiledLayer.setCell(0, 0, 1);
        tiledLayer.setCell(0, 1, 1);
        tiledLayer.setCell(1, 0, 0);
        tiledLayer.setCell(1, 1, 0);

        // Test non-overlapping sprite:
        sprite1.setPosition(100, 100);
        tiledLayer.setPosition(100, 0);
        assertTrue("sprite-tiledlayer fail 1.", !sprite1.collidesWith(tiledLayer, false));
        assertTrue("sprite-tiledlayer fail 2.", !sprite1.collidesWith(tiledLayer, true));

        tiledLayer.setPosition(100, 200);
        assertTrue("sprite-tiledlayer fail 3.", !sprite1.collidesWith(tiledLayer, false));
        assertTrue("sprite-tiledlayer fail 4.", !sprite1.collidesWith(tiledLayer, true));

        tiledLayer.setPosition(0, 100);
        assertTrue("sprite-tiledlayer fail 5.", !sprite1.collidesWith(tiledLayer, false));
        assertTrue("sprite-tiledlayer fail 6.", !sprite1.collidesWith(tiledLayer, true));

        tiledLayer.setPosition(200, 100);
        assertTrue("sprite-tiledlayer fail 7.", !sprite1.collidesWith(tiledLayer, false));
        assertTrue("sprite-tiledlayer fail 8.", !sprite1.collidesWith(tiledLayer, true));

        // Test overlapping sprite:
        sprite1.setPosition(0, 0);
        tiledLayer.setPosition(0, 0);
        assertTrue("sprite-tiledlayer fail 9.", sprite1.collidesWith(tiledLayer, false));
        assertTrue("sprite-tiledlayer fail 10.", sprite1.collidesWith(tiledLayer, true));

        // Test overlapping transparent sprite:
        tiledLayer.setPosition(50, 0);
        assertTrue("sprite-tiledlayer fail 11.", sprite1.collidesWith(tiledLayer, false));
        assertTrue("sprite-tiledlayer fail 12.", !sprite1.collidesWith(tiledLayer, true));

        // Test collisionarea-settings:
        tiledLayer.setPosition(0, 0);
        sprite1.defineCollisionRectangle(0, 0, 50, 100);
        assertTrue("sprite-tiledlayer fail 13.", sprite1.collidesWith(tiledLayer, false));
        assertTrue("sprite-tiledlayer fail 14.", sprite1.collidesWith(tiledLayer, true));
        sprite1.defineCollisionRectangle(50, 0, 50, 100);
        assertTrue("sprite-tiledlayer fail 15.", !sprite1.collidesWith(tiledLayer, false));
        assertTrue("sprite-tiledlayer fail 16.", !sprite1.collidesWith(tiledLayer, true));
        sprite1.defineCollisionRectangle(0, 0, 50, 100);
        tiledLayer.setPosition(50, 0);
        assertTrue("sprite-tiledlayer fail 17.", !sprite1.collidesWith(tiledLayer, false));
        assertTrue("sprite-tiledlayer fail 18.", !sprite1.collidesWith(tiledLayer, true));

        sprite1.defineCollisionRectangle(0, 0, 100, 100);

        // Test invisible sprite:
        sprite1.setPosition(0, 0);
        tiledLayer.setPosition(0, 0);
        sprite1.setVisible(false);
        assertTrue("sprite-tiledlayer fail 19.", !sprite1.collidesWith(tiledLayer, false));
        sprite1.setVisible(true);

        // Test rotated sprite:
        sprite1.setTransform(Sprite.TRANS_ROT90);
        sprite1.setPosition(0, 0);
        assertTrue("sprite-tiledlayer fail 24.", sprite1.collidesWith(tiledLayer, false));
        assertTrue("sprite-tiledlayer fail 25.", sprite1.collidesWith(tiledLayer, true));

        sprite1.setTransform(Sprite.TRANS_ROT180);
        sprite1.setPosition(0, 0);
        assertTrue("sprite-tiledlayer fail 26.", sprite1.collidesWith(tiledLayer, false));
        assertTrue("sprite-tiledlayer fail 27.", !sprite1.collidesWith(tiledLayer, true));

        // Test collision rectangle in transformed sprite:
        sprite1.setTransform(Sprite.TRANS_NONE);
        sprite1.defineCollisionRectangle(0, 0, 50, 50);
        sprite1.setTransform(Sprite.TRANS_ROT90);
        sprite1.setPosition(0, 0);
        assertTrue("sprite-tiledlayer fail 28.", sprite1.collidesWith(tiledLayer, false));
        assertTrue("sprite-tiledlayer fail 29.", !sprite1.collidesWith(tiledLayer, true));

    }


    public void testCollisionTransform() {
        int[] rgb1 = { 0xFF00FF00, 0xFF00FF00, 0x0000FF00,
                       0x0000FF00, 0x0000FF00, 0xFF00FF00 };
        // O O T
        // T T O

        Image spriteImage1 = Image.createRGBImage(rgb1, 3, 2, true);
        Sprite sprite1 = new Sprite(spriteImage1, 1, 2);
        sprite1.defineCollisionRectangle(0, 0, 1, 1);

        int[] rgb2 = { 0xFF0000FF, 0x000000FF,
                       0x000000FF, 0xFF0000FF,
                       0xFF0000FF, 0x000000FF, };
        // O T
        // T O
        // O T

        Image spriteImage2 = Image.createRGBImage(rgb2, 2, 3, true);
        Sprite sprite2 = new Sprite(spriteImage2, 2, 1);
        sprite2.defineReferencePixel(1, 0);
        sprite2.defineCollisionRectangle(1, 0, 1, 1);

        // should not get collision - one transparent, one opaque
        for (int transform1 = 0; transform1 < 8; transform1++) {
            sprite1.setTransform(transform1);
            for (int transform2 = 0; transform2 < 8; transform2++) {
                sprite2.setTransform(transform2);
                sprite2.setRefPixelPosition(0, 0);
                boolean result = !sprite1.collidesWith(sprite2, true);
                if (!result) {
                    print("1. Sprite should't collide with Sprite - frame 1");
                    fail("transform1=" + transform1 + " transform2=" + transform2);
                }
            }
        }

        sprite1.nextFrame(); // frame 2
        sprite2.nextFrame(); // frame 2
        // should always get collision - both opaque
        for (int transform1 = 0; transform1 < 8; transform1++) {
            sprite1.setTransform(transform1);
            for (int transform2 = 0; transform2 < 8; transform2++) {
                sprite2.setTransform(transform2);
                boolean result = sprite1.collidesWith(sprite2, true);
                if (!result) {
                    print("2. Sprite should collide with Sprite - frame 2");
                    fail("transform1=" + transform1 + " transform2=" + transform2);
                }
            }
        }

        sprite1.nextFrame(); // frame 3
        sprite2.nextFrame(); // frame 3
        // should not get collision - both transparent
        for (int transform1 = 0; transform1 < 8; transform1++) {
            sprite1.setTransform(transform1);
            for (int transform2 = 0; transform2 < 8; transform2++) {
                sprite2.setTransform(transform2);
                boolean result = !sprite1.collidesWith(sprite2, true);
                if (!result) {
                    print("3. Sprite should't collide with Sprite - frame 3");
                    fail("transform1=" + transform1 + " transform2=" + transform2);
                }
            }
        }

        // same test but with image

        Image image = Image.createRGBImage(rgb1, 1, 2, true);
        sprite2.setFrame(0);
        // should not get collision - sprite transparent
        for (int transform = 0; transform < 8; transform++) {
            sprite2.setTransform(transform);
            sprite2.setRefPixelPosition(0, 0);
            boolean result = !sprite2.collidesWith(image, 0, 0, true);
            if (!result) {
                print("4. Sprite should't collide with Image - frame 1");
                fail("transform=" + transform);
            }
        }

        sprite2.setFrame(1);
        // should get collision - sprite opaque
        for (int transform = 0; transform < 8; transform++) {
            sprite2.setTransform(transform);
            sprite2.setRefPixelPosition(0, 0);
            boolean result = sprite2.collidesWith(image, 0, 0, true);
            if (!result) {
                print("5. Sprite should collide with Image - frame 2");
                fail("transform=" + transform);
            }
        }

        // same test but with tiledlayer

        int[] rgb4 = { 0xFF00FF00 };
        // O

        Image tiledLayerImage = Image.createRGBImage(rgb4, 1, 1, true);
        TiledLayer tiledLayer = new TiledLayer(1, 2, tiledLayerImage, 1, 1);
        tiledLayer.setCell(0, 0, 1); // opaque

        sprite2.setFrame(0);
        // should not get collision - sprite transparent
        for (int transform = 0; transform < 8; transform++) {
            sprite2.setTransform(transform);
            sprite2.setRefPixelPosition(0, 0);
            boolean result = !sprite2.collidesWith(tiledLayer, true);
            if (!result) {
                print("6. Sprite should't collide with TiledLayer - frame 1");
                fail("transform=" + transform);
            }
        }

        sprite2.setFrame(1);
        // should get collision - sprite opaque
        for (int transform = 0; transform < 8; transform++) {
            sprite2.setTransform(transform);
            sprite2.setRefPixelPosition(0, 0);
            boolean result = sprite2.collidesWith(tiledLayer, true);
            if (!result) {
                print("7. Sprite should collide with TiledLayer - frame 2");
                fail("transform=" + transform);
            }
        }
    }

}



