#pragma once
#include <cmath>

// Simple matrix struct for 2D transformations
struct Matrix4 {
    float m[16];

    Matrix4() {
        // Initialize to identity matrix
        for (int i = 0; i < 16; i++)
            m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }
    
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

    // Add this rotate method to Matrix4
    void rotate(float angle) {
        float c = cos(angle);
        float s = sin(angle);
        
        // Create rotation matrix
        float rotMatrix[16] = {
            c,   -s,    0.0f, 0.0f,
            s,    c,    0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        
        // Multiply current matrix with rotation matrix
        float temp[16];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                temp[i * 4 + j] = 0.0f;
                for (int k = 0; k < 4; k++) {
                    temp[i * 4 + j] += m[i * 4 + k] * rotMatrix[k * 4 + j];
                }
            }
        }
        
        // Copy result back to matrix
        for (int i = 0; i < 16; i++) {
            m[i] = temp[i];
        }
    }
};

// AABB collision detection
bool checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    return (x1 < x2 + w2 && 
            x1 + w1 > x2 && 
            y1 < y2 + h2 && 
            y1 + h1 > y2);
} 


// Circle drawing utility (triangle fan format)
void drawCircleVertices(float* vertices, int segments, float radius) {
    const float PI = 3.1415926f;
    
    // Center vertex
    vertices[0] = 0.0f;
    vertices[1] = 0.0f;
    vertices[2] = 0.0f;
    
    // Generate the vertex positions for the circle perimeter
    for (int i = 0; i <= segments; ++i) {
        float angle = i * (2.0f * PI / segments);
        vertices[(i+1)*3] = radius * cosf(angle);
        vertices[(i+1)*3+1] = radius * sinf(angle);
        vertices[(i+1)*3+2] = 0.0f;
    }
}


// Triangle vertices (for birds)
const float triangleVertices[] = {
    -0.5f, -0.5f, 0.0f,  // Left
     0.5f, -0.5f, 0.0f,  // Right
     0.0f,  0.5f, 0.0f   // Top
};