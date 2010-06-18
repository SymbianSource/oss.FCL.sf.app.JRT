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

//import midp classes
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;
import java.io.IOException;
import java.util.*;

/**
 * This is the class to test the ImageItems Layouts
 *
 *  added layout for every StringItem
 */
public class Test2 extends Form implements CommandListener
{

    private Command prevCommand;
    private Command changeCommand;
    private Command backCommand;

    private Command toggleSizeCommand;

    private Image jigsaw_large = null;
    private Image jigsaw_small = null;
    private Image jigsaw_micro = null;
    private Image jigsaw = null;

    public Test2()
    {
        super("Layout test");
        backCommand = new Command("Back", Command.SCREEN, 5);
        prevCommand = new Command("Prev", Command.SCREEN, 2);
        changeCommand = new Command("Next", Command.SCREEN, 1);
        toggleSizeCommand = new Command("Toggle size", Command.SCREEN, 4);

        jigsaw = jigsaw_large = Images.getInstance().jigsaw;
        jigsaw_small = Images.getInstance().smallJigsaw;
        jigsaw_micro = Images.getInstance().microJigsaw;

        Item s1 = new StringItem("","");
        Item s2 = new StringItem("","This is an ImageItem layouting test.");
        Item s3 = new StringItem("","");
        Item s4 = new StringItem("","");
        Item s5 = new StringItem("","");
        append(s1);
        append(s2);
        append(s3);
        append(s4);
        append(s5);

        addCommand(prevCommand);
        addCommand(changeCommand);
        addCommand(backCommand);
        addCommand(toggleSizeCommand);

        setCommandListener(this);
    }

    static int change = -1;

    public void commandAction(Command c, Displayable s)
    {
        if (c == prevCommand)
        {
            change--;
            changeForm();
        }
        else if (c == changeCommand)
        {
            change++;
            changeForm();
        }
        else if (c == backCommand)
        {
            ImageItemTests.show();
        }
        else if (c == toggleSizeCommand)
        {
            if (jigsaw == jigsaw_large)
            {
                jigsaw = jigsaw_small;
            }
            else if (jigsaw == jigsaw_small)
            {
                jigsaw = jigsaw_micro;
            }
            else
            {
                jigsaw = jigsaw_large;
            }
            changeForm();
        }
    }

    private void changeForm()
    {
        Item s1 = null;
        Item s2 = null;
        Item s3 = null;
        Item s4 = null;
        Item jigsawItem = null;
        Item jigsawItem2 = null;
        Item jigsawItem3 = null;

        Item testStr = new StringItem("","This is an ImageItem layouting test.");
        Item empty1 = new StringItem("","");
        Item empty2 = new StringItem("","");
        Item empty3 = new StringItem("","");
        Item empty4 = new StringItem("","");

        switch (change)
        {
        case -1:
            setTitle("Layout test");
            set(0, empty1);
            set(1, testStr);
            set(2, empty2);
            set(3, empty3);
            set(4, empty4);
            break;
        case 0:
            setTitle("DEFAULT");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_DEFAULT, "Jigsaw.");
            s1.setLayout(Item.LAYOUT_DEFAULT);
            s2.setLayout(Item.LAYOUT_DEFAULT);
            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            break;
        case 1:
            setTitle("LEFT");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_LEFT, "Jigsaw.");
            s1.setLayout(Item.LAYOUT_NEWLINE_AFTER);
            s2.setLayout(Item.LAYOUT_LEFT);
            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            break;
        case 2:
            setTitle("RIGHT");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_RIGHT, "Jigsaw.");
            s1.setLayout(Item.LAYOUT_LEFT);
            s2.setLayout(Item.LAYOUT_LEFT);
            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            break;
        case 3:
            setTitle("CENTER");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_CENTER, "Jigsaw.");
            s1.setLayout(Item.LAYOUT_LEFT);
            s2.setLayout(Item.LAYOUT_LEFT);
            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            break;
        case 4:
            setTitle("NEWLINE_BEFORE");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_NEWLINE_BEFORE, "Jigsaw.");
            s1.setLayout(Item.LAYOUT_LEFT);
            s2.setLayout(Item.LAYOUT_LEFT);
            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            break;
        case 5:
            setTitle("NEWLINE_AFTER");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_NEWLINE_AFTER, "Jigsaw.");
            s1.setLayout(Item.LAYOUT_LEFT);
            s2.setLayout(Item.LAYOUT_LEFT);
            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            break;
        case 6:
            setTitle("NEWLINE_BEFORE|AFTER");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_NEWLINE_BEFORE | ImageItem.LAYOUT_NEWLINE_AFTER, "Jigsaw.");
            s1.setLayout(Item.LAYOUT_LEFT);
            s2.setLayout(Item.LAYOUT_LEFT);
            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            break;
        case 7:
            setTitle("NEWLINE_BEFORE|LEFT");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_NEWLINE_BEFORE | ImageItem.LAYOUT_LEFT, "Jigsaw.");
            s1.setLayout(Item.LAYOUT_NEWLINE_AFTER);
            s2.setLayout(Item.LAYOUT_LEFT);
            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            break;
        case 8:
            setTitle("NEWLINE_BEFORE|RIGHT");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_NEWLINE_BEFORE | ImageItem.LAYOUT_RIGHT, "Jigsaw.");
            s1.setLayout(Item.LAYOUT_NEWLINE_AFTER);
            s2.setLayout(Item.LAYOUT_LEFT);
            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            break;
        case 9:
            setTitle("NEWLINE_BEFORE|CENTER");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_NEWLINE_BEFORE | ImageItem.LAYOUT_CENTER, "Jigsaw.");
            s1.setLayout(Item.LAYOUT_LEFT);
            s2.setLayout(Item.LAYOUT_LEFT);
            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            break;
        case 10:
            setTitle("NEWLINE_AFTER|LEFT");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_NEWLINE_AFTER | ImageItem.LAYOUT_LEFT, "Jigsaw.");
            s1.setLayout(Item.LAYOUT_NEWLINE_AFTER);
            s2.setLayout(Item.LAYOUT_LEFT);
            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            break;
        case 11:
            setTitle("NEWLINE_AFTER|RIGHT");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_NEWLINE_AFTER | ImageItem.LAYOUT_RIGHT, "Jigsaw.");
            s1.setLayout(Item.LAYOUT_LEFT);
            s2.setLayout(Item.LAYOUT_LEFT);
            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            break;
        case 12:
            setTitle("NEWLINE_AFTER|CENTER");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_NEWLINE_AFTER | ImageItem.LAYOUT_CENTER, "Jigsaw.");
            s1.setLayout(Item.LAYOUT_LEFT);
            s2.setLayout(Item.LAYOUT_LEFT);
            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            break;
        case 13:
            setTitle("NEWLINE_BEFORE|AFTER|LEFT");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_NEWLINE_BEFORE | ImageItem.LAYOUT_NEWLINE_AFTER | ImageItem.LAYOUT_LEFT, "Jigsaw.");
            s1.setLayout(Item.LAYOUT_NEWLINE_AFTER);
            s2.setLayout(Item.LAYOUT_LEFT);
            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            break;
        case 14:
            setTitle("NEWLINE_BEFORE|AFTER|RIGHT");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_NEWLINE_BEFORE | ImageItem.LAYOUT_NEWLINE_AFTER | ImageItem.LAYOUT_RIGHT, "Jigsaw.");
            s1.setLayout(Item.LAYOUT_LEFT);
            s2.setLayout(Item.LAYOUT_LEFT);
            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            break;
        case 15:
            setTitle("NEWLINE_BEFORE|AFTER|CENTER");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_NEWLINE_BEFORE | ImageItem.LAYOUT_NEWLINE_AFTER | ImageItem.LAYOUT_CENTER, "Jigsaw.");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");

            set(0, s1);
            set(1, jigsawItem);
            set(2, s2);
            set(3, empty1);
            break;
        case 16:
            setTitle("2 DEFAULT Images");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_DEFAULT, "Jigsaw.");
            jigsawItem2 = new ImageItem("", jigsaw_micro, ImageItem.LAYOUT_DEFAULT, "Jigsaw micro.");
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");

            set(0, s1);
            set(1, jigsawItem);
            set(2, jigsawItem2);
            set(3, s2);
            set(4, empty1);
            break;
        case 17:
            s1 = new StringItem("", "Lorem ipsum ");
            s2 = new StringItem("", " dolor sit amet, consectetur adipiscing elit. Maecenas" +
                                    " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                                    " urna, hendrerit id fringilla eu, feugiat blandit tellus.");

            setTitle("3 DEFAULT Images");
            jigsawItem = new ImageItem("", jigsaw, ImageItem.LAYOUT_DEFAULT, "Jigsaw.");
            jigsawItem2 = new ImageItem("", jigsaw_micro, ImageItem.LAYOUT_DEFAULT, "Jigsaw micro.");
            jigsawItem3 = new ImageItem("", jigsaw_small, ImageItem.LAYOUT_DEFAULT, "Jigsaw small.");
            set(0, s1);
            set(1, jigsawItem);
            set(2, jigsawItem2);
            set(3, jigsawItem3);
            set(4, s2);
            break;
        case 18:
            change = -1;
            changeForm();
            return;
        default:
            change = 17;
            changeForm();
            return;
        }
    }
}
