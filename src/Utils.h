#pragma once

// Simple matrix struct for 2D transformations
struct Matrix4 {
    float m[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    void translate(float x, float y, float z) {
        m[12] += x;
        m[13] += y;
        m[14] += z;
    }
    
    void scale(float x, float y, float z) {
        m[0] *= x;
        m[5] *= y;
        m[10] *= z;
    }
};

// AABB collision detection
bool checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    return (x1 < x2 + w2 && 
            x1 + w1 > x2 && 
            y1 < y2 + h2 && 
            y1 + h1 > y2);
} 