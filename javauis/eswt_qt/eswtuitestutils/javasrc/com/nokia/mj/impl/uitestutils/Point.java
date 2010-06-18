
package com.nokia.mj.impl.uitestutils;

/**
 * Class for storing point, i.e. x and y values.
 * This is used in matric calculations.
 *
 * @author ktuokkol
 */
public class Point {
    public float x;
    public float y;
    
    /**
     * Ctor
     */
    public Point() {
        x = 0f;
        y = 0f;
    }

    /**
     * Ctor
     * @param x
     * @param y
     */
    public Point(float x, float y) {
        this.x = x;
        this.y = y;
    }
    
}
