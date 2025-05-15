#pragma once
#include <vector>

// Player state
struct Player {
    float x = -0.8f;
    float y = -0.3f;
    float width = 0.1f;
    float height = 0.1f;
    float velocityY = 0.0f;
    bool isJumping = false;
    bool facingRight = true;
    
    // Animation state
    float animTime = 0.0f;
    int animFrame = 0;
};

// Platform structure
struct Platform {
    float x;
    float y;
    float width;
    float height;
    
    Platform(float _x, float _y, float _w, float _h) : x(_x), y(_y), width(_w), height(_h) {}
};

// Enemy structure
struct Enemy {
    float x;
    float y;
    float width;
    float height;
    float velocity;
    
    Enemy(float _x, float _y) : x(_x), y(_y), width(0.08f), height(0.08f), velocity(0.00009f) {} // Reduced from 0.0002f to 0.00005f
};

// Collectible coin structure
struct Coin {
    float x;
    float y;
    float width = 0.05f;
    float height = 0.05f;
    bool collected = false;
    
    Coin(float _x, float _y) : x(_x), y(_y) {}
};

// Flag structure (end of level marker)
struct Flag {
    float x;
    float y;
    float width = 0.1f;
    float height = 0.2f;
    
    Flag(float _x, float _y) : x(_x), y(_y) {}
};