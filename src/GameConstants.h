#pragma once

// Window settings
const unsigned int SCR_WIDTH = 800;    // Increased from 1024 to 1280 for wider view
const unsigned int SCR_HEIGHT = 600;    // Reduced from default height

// Game physics
const float GRAVITY = 0.000013f;         
const float JUMP_FORCE = 0.005f;       // Reduced from 0.005f to 0.015f for more reasonable jump height
const float MOVEMENT_SPEED = 0.0009f;   

// Game state
const float LEVEL_END_X = 2.0f;         // Keeping level length the same

const float ASPECT_RATIO = 4.0f / 3.0f;
const float WINDOW_WIDTH = SCR_HEIGHT * ASPECT_RATIO;