#include "glad.h"
#include "glfw3.h"
#include "GameConstants.h"
#include "GameObjects.h"
#include "Shaders.h"
#include "Utils.h"
#include "glm/glm.hpp"

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

// Function prototypes
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void initGame();
void displayInstructions();
void updateHUD();
void drawCircleVertices(float* vertices, int segments, float radius);
void DrawCircle(unsigned int shaderProgram, unsigned int VAO, int transformLoc, int colorLoc, 
               float x, float y, float size, const glm::vec4& color);
void DrawTriangle(unsigned int shaderProgram, unsigned int VAO, int transformLoc, int colorLoc, 
                 float x, float y, float size, const glm::vec4& color);
void drawCircle();
void drawTriangle();

// Game state
float cameraOffset = 0.0f;
bool gameOver = false;
bool gameWin = false;
int score = 0;
float screenRightLimit = 0.5f; // Right limit for camera to start following

// Game objects
Player player;
std::vector<Platform> platforms;
std::vector<Enemy> enemies;
std::vector<Coin> coins;
std::vector<Cloud> clouds;
std::vector<Bird> birds;
std::vector<Mountain> mountains;
std::vector<Tree> trees;

Flag levelFlag(LEVEL_END_X, -0.3f);

// Triangle vertices are defined in Utils.h

void DrawCircle(unsigned int shaderProgram, unsigned int VAO, int transformLoc, int colorLoc, 
               float x, float y, float size, const glm::vec4& color) {
    Matrix4 transform;
    transform.translate(x - cameraOffset, y, 0.0f);
    transform.scale(size, size, 1.0f);
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
    glUniform4f(colorLoc, color.x, color.y, color.z, color.w);
    glBindVertexArray(VAO);
    drawCircle();
}

void DrawTriangle(unsigned int shaderProgram, unsigned int VAO, int transformLoc, int colorLoc, 
                 float x, float y, float size, const glm::vec4& color) {
    Matrix4 transform;
    transform.translate(x - cameraOffset, y, 0.0f);
    transform.scale(size, size, 1.0f);
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
    glUniform4f(colorLoc, color.x, color.y, color.z, color.w);
    glBindVertexArray(VAO);
    drawTriangle();
}

void drawCircle() {
    // Draw a circle using triangle fan mode (center vertex + outline vertices)
    const int circleSegments = 32;
    glDrawArrays(GL_TRIANGLE_FAN, 0, circleSegments + 2);
}

void drawTriangle() {
    // Draw a simple triangle (3 vertices)
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void initBackground()
{
    // Create clouds at different positions and heights
    clouds.push_back(Cloud(-0.8f, 0.7f));
    clouds.push_back(Cloud(0.4f, 0.6f));
    clouds.push_back(Cloud(1.5f, 0.8f));
    clouds.push_back(Cloud(-0.2f, 0.75f));    // New cloud
    clouds.push_back(Cloud(0.9f, 0.65f));     // New cloud
    clouds.push_back(Cloud(2.0f, 0.7f));      // New cloud
    clouds.push_back(Cloud(-1.2f, 0.55f));    // New cloud
    clouds.push_back(Cloud(1.2f, 0.85f));     // New cloud

    // Initialize birds
    birds.push_back(Bird(-0.5f, 0.5f));
    birds.push_back(Bird(0.2f, 0.4f));
    birds.push_back(Bird(0.8f, 0.6f));
    birds.push_back(Bird(1.4f, 0.5f));

    // Initialize mountains with green colors and proper bottom alignment
    mountains.push_back(Mountain(-0.8f, -0.7f, 0.8f, 0.4f, 
        glm::vec4(0.2f, 0.4f, 0.2f, 1.0f))); // Dark forest green
    mountains.push_back(Mountain(0.2f, -0.55f, 1.0f, 0.5f, 
        glm::vec4(0.25f, 0.45f, 0.25f, 1.0f))); // Medium forest green
    mountains.push_back(Mountain(1.0f, -0.55f, 0.9f, 0.45f, 
        glm::vec4(0.3f, 0.5f, 0.3f, 1.0f))); // Light forest green
    mountains.push_back(Mountain(1.8f, -0.55f, 0.7f, 0.35f, 
        glm::vec4(0.35f, 0.55f, 0.35f, 1.0f))); // Lighter forest green

    // Initialize trees with better spacing and sizing
    trees.push_back(Tree(-0.9f, -0.4f, 0.2f));
    trees.push_back(Tree(-0.3f, -0.4f, 0.25f));
    trees.push_back(Tree(0.4f, -0.4f, 0.22f));
    trees.push_back(Tree(1.2f, -0.4f, 0.23f));
    trees.push_back(Tree(1.9f, -0.4f, 0.21f));
}

// Initialize game objects
void initGame()
{
    // Reset game objects
    platforms.clear();
    enemies.clear();
    coins.clear();

    // Base platform position
    float y = -0.5f;

    // Create a series of platforms with gaps
    // First platform (starting platform)
    platforms.push_back(Platform(-1.0f, y, 0.5f, 0.1f));

    // Second platform after a small gap
    platforms.push_back(Platform(-0.3f, y, 0.4f, 0.1f));

    // Third platform after gap
    platforms.push_back(Platform(0.3f, y, 0.5f, 0.1f));

    // Fourth platform
    platforms.push_back(Platform(1.0f, y, 0.4f, 0.1f));

    // Fifth platform (end platform)
    platforms.push_back(Platform(1.7f, y, 0.5f, 0.1f));

    // Add enemies at strategic positions
    enemies.push_back(Enemy(0.3f, -0.4f)); // On third platform
    enemies.push_back(Enemy(1.7f, -0.4f)); // On last platform

    // Add coins over gaps and platforms
    coins.push_back(Coin(-0.6f, -0.2f)); // First platform
    coins.push_back(Coin(-0.1f, -0.2f)); // Over first gap
    coins.push_back(Coin(0.5f, -0.2f));  // Third platform
    coins.push_back(Coin(0.7f, -0.2f));  // Third platform
    coins.push_back(Coin(1.4f, -0.2f));  // Over last gap

    // Position flag at the end of the last platform
    levelFlag.x = 2.0f;
    levelFlag.y = -0.3f;
}

// Function to display instructions at the start
void displayInstructions()
{
    std::cout << "===== MARIO-LIKE PLATFORMER GAME =====" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  LEFT ARROW  - Move left (but cannot move behind camera)" << std::endl;
    std::cout << "  RIGHT ARROW - Move right" << std::endl;
    std::cout << "  SPACE/UP    - Jump" << std::endl;
    std::cout << "  R           - Restart game" << std::endl;
    std::cout << "  ESC         - Quit game" << std::endl;
    std::cout << std::endl;
    std::cout << "Objectives:" << std::endl;
    std::cout << "  - Collect coins for points" << std::endl;
    std::cout << "  - Avoid red enemies" << std::endl;
    std::cout << "  - Reach the green flag to win" << std::endl;
    std::cout << "=====================================" << std::endl;
}

// Function to draw text on screen (simulated with console output)
void updateHUD()
{
    // Clear previous line
    std::cout << "\r                                                 ";
    // Display the score and coins collected
    int coinsCollected = 0;
    for (const Coin &coin : coins)
    {
        if (coin.collected)
            coinsCollected++;
    }
    std::cout << "\rScore: " << std::setw(6) << std::setfill('0') << score
              << " | Coins: " << coinsCollected << "/" << coins.size();
    std::cout.flush();
}

unsigned int VBO, VAO, circleVAO, circleVBO, triangleVAO, triangleVBO;

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mario-like Platformer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };
    
    // Set up the VAO/VBO for rectangular shapes (platforms, player, enemies, etc.)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Circle setup (for sun/clouds)
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);

    // Triangle setup (for birds)
    glGenVertexArrays(1, &triangleVAO);
    glGenBuffers(1, &triangleVBO);

    // Circle vertices (dynamic)
    // Circle setup with proper sizing
    const int circleSegments = 32;
    float circleVertices[(circleSegments + 2) * 3]; // +2 for center vertex and closing vertex
    drawCircleVertices(circleVertices, circleSegments, 1.0f);

    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Triangle vertices setup
    glBindVertexArray(triangleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Initialize game objects
    initGame();
    initBackground();

    glUseProgram(shaderProgram);
    int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    int colorLocation = glGetUniformLocation(shaderProgram, "ourColor");

    // Make sure these uniform locations are correctly retrieved
    transformLoc = glGetUniformLocation(shaderProgram, "transform");
    colorLocation = glGetUniformLocation(shaderProgram, "ourColor");

    // Check if uniform locations are valid
    if (transformLoc == -1 || colorLocation == -1) {
        std::cout << "Error: Failed to get uniform locations" << std::endl;
    }

    double lastTime = glfwGetTime();
    displayInstructions();

    // Main game loop
    while (!glfwWindowShouldClose(window) && !gameOver && !gameWin)
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        player.animTime += deltaTime;
        if (player.animTime > 0.2f)
        {
            player.animTime = 0.0f;
            player.animFrame = (player.animFrame + 1) % 2;
        }

        processInput(window);

        // Apply gravity
        player.velocityY -= GRAVITY * deltaTime * 1000;
        player.y += player.velocityY;

        // Check for platform collisions
        bool onGround = false;
        for (const Platform &platform : platforms)
        {
            if (checkCollision(
                    player.x - player.width / 2, player.y - player.height / 2, player.width, player.height,
                    platform.x - platform.width / 2 - cameraOffset, platform.y - platform.height / 2, platform.width, platform.height))
            {

                // Check if player is landing on top of platform
                if (player.velocityY < 0 &&
                    (player.y - player.height / 2) > (platform.y + platform.height / 2 - 0.01f))
                {
                    player.y = platform.y + platform.height / 2 + player.height / 2;
                    player.velocityY = 0;
                    onGround = true;
                }
            }
        }

        if (onGround)
        {
            player.isJumping = false;
        }

        // Check if player fell off the screen
        if (player.y < -1.0f)
        {
            gameOver = true;
        }

        // Camera follows player
        cameraOffset = player.x - screenRightLimit;
        if (cameraOffset < 0)
            cameraOffset = 0; // Don't let camera go past left edge

        // Update enemies
        for (Enemy &enemy : enemies)
        {
            enemy.x += enemy.velocity;
            if (enemy.x > enemy.patrolRight || enemy.x < enemy.patrolLeft)
            {
                enemy.velocity = -enemy.velocity;
            }

            // Check for collision with enemy
            if (checkCollision(
                    player.x - player.width / 2, player.y - player.height / 2, player.width, player.height,
                    enemy.x - enemy.width / 2 - cameraOffset, enemy.y - enemy.height / 2, enemy.width, enemy.height))
            {
                gameOver = true;
            }
        }

        // Check coin collection
        for (Coin &coin : coins)
        {
            if (!coin.collected && checkCollision(
                                       player.x - player.width / 2, player.y - player.height / 2, player.width, player.height,
                                       coin.x - coin.width / 2 - cameraOffset, coin.y - coin.height / 2, coin.width, coin.height))
            {
                coin.collected = true;
                score += 100;
            }
        }

        // Check if player reached the flag
        if (checkCollision(
                player.x - player.width / 2, player.y - player.height / 2, player.width, player.height,
                levelFlag.x - levelFlag.width / 2 - cameraOffset, levelFlag.y - levelFlag.height / 2, levelFlag.width, levelFlag.height))
        {
            gameWin = true;
        }

        updateHUD();

        // Rendering
        glClearColor(0.4f, 0.6f, 1.0f, 1.0f); // Sky blue background
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw mountains (triangular shape)
        for (const Mountain& mountain : mountains) {
            // Draw mountain as a triangle
            Matrix4 transform;
            transform.translate(mountain.x - cameraOffset * 0.5f, mountain.y, 0.0f);
            
            // Use triangleVAO for mountain shape
            glBindVertexArray(triangleVAO);
            transform.scale(mountain.width * 2.0f, mountain.height * 2.0f, 1.0f);
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
            glUniform4f(colorLocation, 
                        mountain.color.x,
                        mountain.color.y, 
                        mountain.color.z, 
                        mountain.color.w);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        // Draw trees (after mountains but before other game elements)
        for (const Tree& tree : trees) {
            // Draw trunk
            Matrix4 trunkTransform;
            trunkTransform.translate(tree.x - cameraOffset * 0.7f, tree.y, 0.0f);
            trunkTransform.scale(tree.size * 0.2f, tree.size * 0.8f, 1.0f);
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, trunkTransform.m);
            glUniform4f(colorLocation, 0.45f, 0.3f, 0.2f, 1.0f); // Brown trunk
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Draw tree crown (triangle shape)
            Matrix4 crownTransform;
            crownTransform.translate(tree.x - cameraOffset * 0.7f, tree.y + tree.size * 0.8f, 0.0f);
            crownTransform.scale(tree.size * 1.2f, tree.size * 1.5f, 1.0f);
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, crownTransform.m);
            glUniform4f(colorLocation, 0.1f, 0.6f, 0.1f, 1.0f); // Green crown
            glBindVertexArray(triangleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        // Sun (circle)
        DrawCircle(shaderProgram, circleVAO, transformLoc, colorLocation, 
                  -0.8f, 0.8f, 0.15f, glm::vec4(1.0f, 0.84f, 0.0f, 1.0f));
        
        // Clouds (circles)
        for (const Cloud& cloud : clouds) {
            // Main cloud circle
            DrawCircle(shaderProgram, circleVAO, transformLoc, colorLocation, 
                      cloud.x, cloud.y, cloud.size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            
            // Additional circles to form a cloud shape
            DrawCircle(shaderProgram, circleVAO, transformLoc, colorLocation, 
                      cloud.x + 0.08f, cloud.y, cloud.size * 0.8f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            
            DrawCircle(shaderProgram, circleVAO, transformLoc, colorLocation, 
                      cloud.x - 0.08f, cloud.y, cloud.size * 0.9f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            
            DrawCircle(shaderProgram, circleVAO, transformLoc, colorLocation, 
                      cloud.x, cloud.y + 0.03f, cloud.size * 0.7f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        }
        
        // Birds (triangles)
        for (Bird &bird : birds) {
      DrawTriangle(shaderProgram, triangleVAO, transformLoc, colorLocation, 
                bird.x, bird.y + sin(bird.angle)*0.02f, 0.05f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        }

        // Draw platforms
        glBindVertexArray(VAO);
        for (const Platform &platform : platforms)
        {
            Matrix4 transform;
            transform.translate(platform.x - cameraOffset, platform.y, 0.0f);
            transform.scale(platform.width, platform.height, 1.0f);
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
            glUniform4f(colorLocation, 0.5f, 0.35f, 0.05f, 1.0f); // Brown color for platforms
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // Draw enemies
        for (const Enemy &enemy : enemies)
        {
            Matrix4 transform;
            transform.translate(enemy.x - cameraOffset, enemy.y, 0.0f);
            transform.scale(enemy.width, enemy.height, 1.0f);
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
            glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f); // Red enemies
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // Draw coins
        for (const Coin &coin : coins)
        {
            if (!coin.collected)
            {
                Matrix4 transform;
                transform.translate(coin.x - cameraOffset, coin.y, 0.0f);
                transform.scale(coin.width, coin.height, 1.0f);
                glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
                glUniform4f(colorLocation, 1.0f, 0.84f, 0.0f, 1.0f); // Gold coins
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        // Draw flag
        Matrix4 flagTransform;
        flagTransform.translate(levelFlag.x - cameraOffset, levelFlag.y, 0.0f);
        flagTransform.scale(levelFlag.width, levelFlag.height, 1.0f);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, flagTransform.m);
        glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f); // Green flag
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Draw flag pole
        Matrix4 poleTransform;
        poleTransform.translate(levelFlag.x - cameraOffset - 0.05f, levelFlag.y - 0.1f, 0.0f);
        poleTransform.scale(0.02f, 0.4f, 1.0f);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, poleTransform.m);
        glUniform4f(colorLocation, 0.5f, 0.5f, 0.5f, 1.0f); // Gray pole
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Draw player
        Matrix4 transform;
        transform.translate(player.x - cameraOffset, player.y + (player.animFrame * 0.01f), 0.0f);
        transform.scale(player.width, player.height, 1.0f);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
        glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f); // Blue player
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Draw player eyes
        Matrix4 eyeTransform;
        float eyeDirection = player.facingRight ? 0.02f : -0.02f;

        eyeTransform = Matrix4();
        eyeTransform.translate(player.x - cameraOffset + eyeDirection, player.y + 0.02f, 0.0f);
        eyeTransform.scale(0.02f, 0.02f, 1.0f);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, eyeTransform.m);
        glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f); // White eyes
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Update and draw birds
for (Bird& bird : birds) {
    float time = (float)glfwGetTime();
    
    // Horizontal movement
    if (bird.movingRight) {
        bird.x += bird.speed * (0.8f + sin(time * 0.5f) * 0.2f);
        if (bird.x > 2.5f) bird.movingRight = false;
    } else {
        bird.x -= bird.speed * (0.8f + sin(time * 0.5f) * 0.2f);
        if (bird.x < -1.5f) bird.movingRight = true;
    }
    
    // Vertical body movement
    bird.angle += 0.01f;
    float yOffset = sin(bird.angle) * 0.015f;
    
    float direction = bird.movingRight ? 1.0f : -1.0f;
    glm::vec4 birdColor(0.2f, 0.2f, 0.2f, 1.0f);
    float bodySize = 0.04f;
    
    // Main body triangle
    DrawTriangle(shaderProgram, triangleVAO, transformLoc, colorLocation,
                bird.x, bird.y + yOffset, bodySize,
                birdColor);
    
    // Wing animation - more pronounced up/down movement
    float wingFlap = sin(time * 8.0f) * 0.04f;  // Faster and larger vertical movement
    
    // Wing triangle with emphasized vertical flapping
    DrawTriangle(shaderProgram, triangleVAO, transformLoc, colorLocation,
                bird.x - (0.03f * direction), 
                bird.y + yOffset + wingFlap,  // Add vertical flapping offset
                bodySize * 0.6f,
                birdColor);
}

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (gameWin)
    {
        std::cout << std::endl
                  << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "   CONGRATULATIONS! YOU WON!" << std::endl;
        std::cout << "   Final Score: " << score << std::endl;
        std::cout << "=====================================" << std::endl;
    }
    else if (gameOver)
    {
        std::cout << std::endl
                  << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "   GAME OVER!" << std::endl;
        std::cout << "   Final Score: " << score << std::endl;
        std::cout << "   Press R to try again" << std::endl;
        std::cout << "=====================================" << std::endl;
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &circleVAO);
    glDeleteBuffers(1, &circleVBO);
    glDeleteVertexArrays(1, &triangleVAO);
    glDeleteBuffers(1, &triangleVBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Player movement
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        player.x += MOVEMENT_SPEED;
        player.facingRight = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        // Only allow moving left if not at the camera's left edge
        if (player.x > cameraOffset - 0.8f)
        {
            player.x -= MOVEMENT_SPEED;
            player.facingRight = false;
        }
    }
    if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ||
         glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) &&
        !player.isJumping)
    {
        player.velocityY = JUMP_FORCE;
        player.isJumping = true;
    }

    // Restart game with R key
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        // Reset game state
        player.x = -0.8f;
        player.y = -0.3f;
        player.velocityY = 0.0f;
        player.isJumping = false;
        cameraOffset = 0.0f;
        gameOver = false;
        gameWin = false;
        score = 0;

        // Reset collected coins
        for (Coin &coin : coins)
        {
            coin.collected = false;
        }

        // Reinitialize game objects
        initGame();
    }
}

// glfw: whenever the window size changed this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
