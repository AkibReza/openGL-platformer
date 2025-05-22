#pragma once
#include <vector>
#include <glm/glm.hpp> // Include GLM for glm::vec4

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
    float initialY;     // Store initial Y position
    float floatTimer;   // Timer for floating animation
    
    Platform(float _x, float _y, float _w, float _h) 
        : x(_x), y(_y), width(_w), height(_h), initialY(_y), floatTimer(0.0f) {}
};

// Modified Enemy struct
struct Enemy {
    float x;
    float y;
    float width;
    float height;
    float velocity;
    float patrolLeft;
    float patrolRight;
    
    // Add these new variables for zoom animation
    float scaleTimer = 0.0f;
    float baseScale = 1.0f;
    float zoomAmount = 0.2f;    // How much it zooms (20%)
    float zoomSpeed = 2.0f;     // Speed of zoom animation
    
    Enemy(float _x, float _y) : x(_x), y(_y), width(0.08f), height(0.08f), 
           velocity(0.00009f), 
           patrolLeft(_x - 0.8f),
           patrolRight(_x + 0.8f) {} 
};


// Collectible coin structure
struct Coin {
    float x, y;
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


struct Cloud {
    float x, y;
    float size;
    float bounceOffset;  // For vertical bouncing
    
    Cloud(float _x, float _y) 
        : x(_x), y(_y), size(0.1f), bounceOffset(static_cast<float>(rand()) / RAND_MAX * 6.28f) {}
};

struct Bird {
    float x, y;
    float speed;
    float angle;
    bool movingRight;

    // Reduced speed significantly for smoother movement
    Bird(float _x, float _y) : x(_x), y(_y), speed(0.0002f), angle(0.0f), movingRight(true) {}
};

struct Mountain {
    float x;
    float y;
    float width;
    float height;
    glm::vec4 color;
    
    Mountain(float _x, float _y, float _width, float _height, glm::vec4 _color)
        : x(_x), y(_y), width(_width), height(_height), color(_color) {}
};

struct Star {
    float x;
    float y;
    float size;
    float twinkleSpeed;
    float twinklePhase;
    
    Star(float _x, float _y) 
        : x(_x), y(_y), 
          size(0.005f + static_cast<float>(rand())/RAND_MAX * 0.005f),
          twinkleSpeed(1.0f + static_cast<float>(rand())/RAND_MAX * 2.0f),
          twinklePhase(static_cast<float>(rand())/RAND_MAX * 6.28f) {}
};

struct GrassTuft {
    float x;
    float y;
    float height;
    
    GrassTuft(float _x, float _y, float _height)
        : x(_x), y(_y), height(_height) {}
};

struct Tree {
    float x, y;
    float size;
    float initialY;     // Store initial Y position
    float floatTimer;   // Timer for floating animation
    
    Tree(float x, float y, float size) 
        : x(x), y(y), size(size), initialY(y), floatTimer(0.0f) {}
};

struct Sun {
    float x;
    float y;
    float size;
    float rotation;
    
    Sun(float _x, float _y) 
        : x(_x), y(_y), size(0.15f), rotation(0.0f) {}
};