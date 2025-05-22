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
void restartGame(); // Changed from initGame to a more comprehensive restart function
void initGameInternal(); // For core game object initialization
void displayInstructions();
void updateHUD();
void drawCircleVertices(float *vertices, int segments, float radius);
void DrawCircle(unsigned int shaderProgram, unsigned int VAO, int transformLoc, int colorLoc,
                float x, float y, float size, const glm::vec4 &color);
void DrawTriangle(unsigned int shaderProgram, unsigned int VAO, int transformLoc, int colorLoc,
                  float x, float y, float size, const glm::vec4 &color);
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
                float x, float y, float size, const glm::vec4 &color)
{
    Matrix4 transform;
    transform.translate(x - cameraOffset, y, 0.0f);
    transform.scale(size, size, 1.0f);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
    glUniform4f(colorLoc, color.x, color.y, color.z, color.w);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 32 + 2); // Draw circle with triangle fan
}

void DrawTriangle(unsigned int shaderProgram, unsigned int VAO, int transformLoc, int colorLoc,
                  float x, float y, float size, const glm::vec4 &color)
{
    Matrix4 transform;
    transform.translate(x - cameraOffset, y, 0.0f);
    transform.scale(size, size, 1.0f);
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
    glUniform4f(colorLoc, color.x, color.y, color.z, color.w);
    glBindVertexArray(VAO);
    drawTriangle();
}

void drawCircle()
{
    // Draw a circle using triangle fan mode (center vertex + outline vertices)
    const int circleSegments = 32;
    glDrawArrays(GL_TRIANGLE_FAN, 0, circleSegments + 2);
}

void drawTriangle()
{
    // Draw a simple triangle (3 vertices)
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void initBackground()
{
    // Create clouds at different positions and heights
    clouds.push_back(Cloud(-0.8f, 0.7f));
    clouds.push_back(Cloud(0.4f, 0.6f));
    clouds.push_back(Cloud(1.5f, 0.8f));
    clouds.push_back(Cloud(-0.2f, 0.75f)); // New cloud
    clouds.push_back(Cloud(0.9f, 0.65f));  // New cloud
    clouds.push_back(Cloud(2.0f, 0.7f));   // New cloud
    clouds.push_back(Cloud(-1.2f, 0.55f)); // New cloud
    clouds.push_back(Cloud(1.2f, 0.85f));  // New cloud

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
void initGameInternal() // Renamed to avoid conflict, called by restartGame
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

    // After platforms are created
    levelFlag.x = platforms.back().x + platforms.back().width/2 + 0.2f;
    levelFlag.y = platforms.back().y;  // Match platform height
    
    // Add enemies at strategic positions with faster movement
    Enemy enemy1(0.3f, -0.4f);
    enemy1.velocity = 0.0002f; // Doubled speed
    enemies.push_back(enemy1);

    Enemy enemy2(1.7f, -0.4f);
    enemy2.velocity = 0.0002f; // Doubled speed
    enemies.push_back(enemy2);

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

void restartGame() {
    // Reset player state
    player.x = -0.8f;
    player.y = -0.3f;
    player.velocityY = 0.0f;
    player.isJumping = false;
    player.animTime = 0.0f;
    player.animFrame = 0;
    player.facingRight = true;

    // Reset game state variables
    cameraOffset = 0.0f;
    gameOver = false;
    gameWin = false;
    score = 0;

    // Clear and reinitialize game objects
    // initGameInternal will clear and repopulate platforms, enemies, coins
    initGameInternal();    // This also resets levelFlag position

    // Clear and reinitialize background elements
    clouds.clear();
    birds.clear();
    mountains.clear();
    trees.clear();
    initBackground(); // Repopulate background elements

    // Update HUD to reflect reset state
    updateHUD();
}

unsigned int VBO, VAO, circleVAO, circleVBO, triangleVAO, triangleVBO;
float diamondVertices[] = {
    0.0f, 1.0f, 0.0f,  // top
    1.0f, 0.0f, 0.0f,  // right
    0.0f, -1.0f, 0.0f, // bottom
    -1.0f, 0.0f, 0.0f, // left
    0.0f, 1.0f, 0.0f,  // top (repeated for triangle fan)
};

unsigned int diamondVAO, diamondVBO;

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
        -0.5f, 0.5f, 0.0f};

    // Set up the VAO/VBO for rectangular shapes (platforms, player, enemies, etc.)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Circle setup (for sun/clouds)
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);

    // Triangle setup (for birds)
    glGenVertexArrays(1, &triangleVAO);
    glGenBuffers(1, &triangleVBO);

    // Diamond setup (for coins)
    glGenVertexArrays(1, &diamondVAO);
    glGenBuffers(1, &diamondVBO);

    // Circle vertices (dynamic)
    // Circle setup with proper sizing
    const int circleSegments = 32;
    float circleVertices[(circleSegments + 2) * 3]; // +2 for center vertex and closing vertex
    drawCircleVertices(circleVertices, circleSegments, 1.0f);

    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Triangle vertices setup
    glBindVertexArray(triangleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Diamond vertices setup
    glBindVertexArray(diamondVAO);
    glBindBuffer(GL_ARRAY_BUFFER, diamondVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(diamondVertices), diamondVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Initialize game objects
    restartGame(); // Initial setup of the game state
    // initBackground() is called within restartGame()

    glUseProgram(shaderProgram);
    int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    int colorLocation = glGetUniformLocation(shaderProgram, "ourColor");

    // Make sure these uniform locations are correctly retrieved
    transformLoc = glGetUniformLocation(shaderProgram, "transform");
    colorLocation = glGetUniformLocation(shaderProgram, "ourColor");

    // Check if uniform locations are valid
    if (transformLoc == -1 || colorLocation == -1)
    {
        std::cout << "Error: Failed to get uniform locations" << std::endl;
    }

    double lastTime = glfwGetTime();
    displayInstructions(); // Display instructions once at the very start

    // Main game loop
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;

    while (!glfwWindowShouldClose(window)) // Loop continues until ESC is pressed
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Handle game state transitions (win/loss) and auto-restart
        if (gameOver || gameWin) {
            if (gameWin) {
                std::cout << std::endl << std::endl;
                std::cout << "=====================================" << std::endl;
                std::cout << "   CONGRATULATIONS! YOU WON!" << std::endl;
                std::cout << "   Final Score: " << score << std::endl;
                std::cout << "=====================================" << std::endl;
            } else { // gameOver
                std::cout << std::endl << std::endl;
                std::cout << "=====================================" << std::endl;
                std::cout << "   GAME OVER!" << std::endl;
                std::cout << "   Final Score: " << score << std::endl;
                std::cout << "=====================================" << std::endl;
            }
            // Optional: Add a small delay here for messages to be read before restart.
            // For example, using a timer or glfwWaitEventsTimeout if input processing during pause is desired.
            // For simplicity, we'll restart immediately.
            // Example of a pause (would require event handling during pause):
            // double pauseEndTime = glfwGetTime() + 2.0; // Pause for 2 seconds
            // while(glfwGetTime() < pauseEndTime) {
            //    glfwPollEvents(); // Keep processing events like ESC
            //    if (glfwWindowShouldClose(window)) break;
            // }
            // if (glfwWindowShouldClose(window)) break; // Exit main loop if ESC was pressed during pause
            restartGame(); // Resets game state, including gameOver and gameWin flags
        }

        // Update platform and tree positions
        float floatSpeed = 2.0f;
        float floatAmplitude = 0.03f;

        for (Platform &platform : platforms)
        {
            platform.floatTimer += deltaTime;
            platform.y = platform.initialY + sin(platform.floatTimer * floatSpeed) * floatAmplitude;
        }

        for (Tree &tree : trees)
        {
            tree.floatTimer += deltaTime;
            tree.y = tree.initialY + sin(tree.floatTimer * floatSpeed) * floatAmplitude;
        }

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
                    platform.x - platform.width / 2, platform.y - platform.height / 2, platform.width, platform.height))
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
        if (player.y < -1.0f && !gameOver && !gameWin) // Prevent re-triggering if already over
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
                    enemy.x - enemy.width / 2, enemy.y - enemy.height / 2, enemy.width, enemy.height))
            {   
                if (!gameOver && !gameWin) // Prevent re-triggering
                gameOver = true;
            }
        }

        // Check coin collection
        for (Coin &coin : coins)
        {
            if (!coin.collected && checkCollision(
                                       player.x - player.width / 2, player.y - player.height / 2, player.width, player.height,
                                       coin.x - coin.width / 2, coin.y - coin.height / 2, coin.width, coin.height))
            {
                coin.collected = true;
                score += 100;
            }
        }

        // Check if player reached the flag
        if (!gameWin && !gameOver && checkCollision( // Prevent re-triggering
                player.x - player.width / 2, player.y - player.height / 2, player.width, player.height,
                levelFlag.x - levelFlag.width / 2, levelFlag.y - levelFlag.height / 2, levelFlag.width, levelFlag.height))
        {
            
            gameWin = true;
        }

        updateHUD();

        // Rendering
        glClearColor(0.4f, 0.6f, 1.0f, 1.0f); // Sky blue background
        glClear(GL_COLOR_BUFFER_BIT);

        // Update bird logic (moved from drawing loop for better structure and deltaTime usage)
        const float BIRD_ANGLE_SPEED = 0.6f; // Approx 0.01 rad/frame * 60 fps

        for (Bird &bird : birds) {
            // Bird speed is 0.03f in constructor, assume units/sec
            float timeBasedFluctuation = (0.8f + sin((float)glfwGetTime() * 0.5f) * 0.2f); // Use glfwGetTime() for smooth sine wave
            float effectiveSpeed = bird.speed * timeBasedFluctuation;

            if (bird.movingRight) {
                bird.x += effectiveSpeed * deltaTime;
                if (bird.x > 2.5f + cameraOffset) // Adjust patrol limits relative to camera if they are world-space
                    bird.movingRight = false;
            } else {
                bird.x -= effectiveSpeed * deltaTime;
                if (bird.x < -1.5f + cameraOffset)
                    bird.movingRight = true;
            }
            bird.angle += BIRD_ANGLE_SPEED * deltaTime;
        }


        // Cloud bouncing
        for (Cloud &cloud : clouds) {
            // Update bounce animation
            float bounceFreq = 0.9f;  // Controls how fast the cloud bounces
            float bounceAmount = 0.000006f; // Controls how much the cloud moves up/down
            cloud.bounceOffset += deltaTime * bounceFreq;
            
            // Calculate vertical offset using sine wave
            float verticalOffset = sin(cloud.bounceOffset) * bounceAmount;
            cloud.y = cloud.y + verticalOffset;
        }

        // Bird movement update (replace existing bird movement code)
        const float BIRD_VERTICAL_SPEED = 0.3f;
        const float BIRD_HORIZONTAL_SPEED = 0.2f;
        const float BIRD_VERTICAL_RANGE = 0.05f;

        for (Bird &bird : birds) {
            // Horizontal movement
            if (bird.movingRight) {
                bird.x += BIRD_HORIZONTAL_SPEED * deltaTime;
                if (bird.x > 2.5f + cameraOffset) {
                    bird.movingRight = false;
                }
            } else {
                bird.x -= BIRD_HORIZONTAL_SPEED * deltaTime;
                if (bird.x < -1.5f + cameraOffset) {
                    bird.movingRight = true;
                }
            }
            
            // Vertical movement (smooth sine wave)
            bird.angle += BIRD_VERTICAL_SPEED * deltaTime;
            bird.y = bird.y + sin(bird.angle) * BIRD_VERTICAL_RANGE * deltaTime;
        }

        // Draw mountains (triangular shape)
        for (const Mountain &mountain : mountains)
        {
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
        for (const Tree &tree : trees)
        {
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

        // Draw rotating sun with rays
        // Draw rotating sun with rays
        {
            float time = (float)glfwGetTime();
            float rotationAngle = time * 0.2f;             // Rotation speed
            float pulse = sin(time * 2.0f) * 0.01f + 1.0f; // Subtle pulsing effect

            // Main sun circle
            Matrix4 sunTransform;
            // First scale (applied first)
            sunTransform.scale(0.15f * pulse, 0.15f * pulse, 1.0f);
            // Then rotate (applied second)
            sunTransform.rotate(rotationAngle);
            // Finally translate to position (applied last)
            sunTransform.translate(-0.8f, 0.8f, 0.0f);

            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, sunTransform.m);
            glUniform4f(colorLocation, 1.0f, 0.84f, 0.0f, 1.0f);
            glBindVertexArray(circleVAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 32 + 2);

            // Sun rays
            for (int i = 0; i < 8; i++)
            {
                float rayAngle = rotationAngle + (i * 3.14159f / 4.0f);
                float rayLength = 0.05f * pulse;

                Matrix4 rayTransform;
                // Order matters: scale -> rotate -> translate
                rayTransform.scale(0.02f, rayLength, 1.0f);
                rayTransform.rotate(rayAngle);
                rayTransform.translate(-0.8f + cos(rayAngle) * 0.2f,
                                       0.8f + sin(rayAngle) * 0.2f, 0.0f);

                glUniformMatrix4fv(transformLoc, 1, GL_FALSE, rayTransform.m);
                glUniform4f(colorLocation, 1.0f, 0.9f, 0.3f, 1.0f);
                glBindVertexArray(triangleVAO);
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
        }
        // Clouds (circles)
        for (const Cloud &cloud : clouds)
        {
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
        // Drawing part of birds, positions are now updated above
        for (const Bird &bird : birds) { // Iterate as const since we are only drawing
            float yOffset = sin(bird.angle) * 0.015f; // bird.angle is updated with deltaTime
            DrawTriangle(shaderProgram, triangleVAO, transformLoc, colorLocation,
                         bird.x, bird.y + yOffset, 0.05f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            // More complex bird drawing (wings) would also go here, using bird.x, bird.y, bird.angle
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
        for (Enemy &enemy : enemies)
        {
            // Update scale animation
            enemy.scaleTimer += deltaTime * enemy.zoomSpeed;
            float currentScale = enemy.baseScale + sin(enemy.scaleTimer) * enemy.zoomAmount;
            
            Matrix4 transform;
            transform.translate(enemy.x - cameraOffset, enemy.y, 0.0f);
            transform.scale(enemy.width * currentScale, enemy.height * currentScale, 1.0f);
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
            glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f); // Red enemies
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // Draw coins (no rotation)
        for (Coin &coin : coins)
        {
            if (!coin.collected)
            {
                Matrix4 transform;
                transform.translate(coin.x - cameraOffset, coin.y, 0.0f);
                transform.scale(coin.width, coin.height, 1.0f);

                glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
                glUniform4f(colorLocation, 1.0f, 0.84f, 0.0f, 1.0f); // Gold coins
                glBindVertexArray(diamondVAO);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 5);
            }
        }

        // Draw flag base/platform
        {
            Matrix4 baseTransform;
            baseTransform.translate(levelFlag.x - cameraOffset, levelFlag.y, 0.0f);
            baseTransform.scale(0.1f, 0.05f, 1.0f);
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, baseTransform.m);
            glUniform4f(colorLocation, 0.5f, 0.35f, 0.05f, 1.0f);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // Draw flag pole
        {
            Matrix4 poleTransform;
            poleTransform.translate(levelFlag.x - cameraOffset, levelFlag.y + 0.15f, 0.0f);
            poleTransform.scale(0.02f, 0.3f, 1.0f);
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, poleTransform.m);
            glUniform4f(colorLocation, 0.7f, 0.7f, 0.7f, 1.0f);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // Draw flag
        {
            float flagVertices[] = {
                0.0f, 0.5f, 0.0f,
                0.0f, -0.5f, 0.0f,
                -1.0f, 0.0f, 0.0f
            };

            Matrix4 flagTransform;
            flagTransform.translate(levelFlag.x - cameraOffset, levelFlag.y + 0.3f, 0.0f);
            flagTransform.scale(0.08f, 0.1f, 1.0f);
            
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, flagTransform.m);
            glUniform4f(colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
            
            glBindVertexArray(triangleVAO);
            glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(flagVertices), flagVertices, GL_STATIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            
            // Restore original triangle vertices
            glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
        }

        // Draw player
        Matrix4 transform;
        transform.translate(player.x - cameraOffset, player.y + (player.animFrame * 0.01f), 0.0f);
        transform.scale(player.width, player.height, 1.0f);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
        glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f); // Blue player
        glBindVertexArray(VAO);                             // Make sure to bind the rectangular VAO before drawing
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

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Game over/win messages are now handled inside the loop before restart.
    // No final messages needed here as the loop only exits on ESC.

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &circleVAO);
    glDeleteBuffers(1, &circleVBO);
    glDeleteVertexArrays(1, &triangleVAO);
    glDeleteBuffers(1, &triangleVBO);
    glDeleteVertexArrays(1, &diamondVAO);
    glDeleteBuffers(1, &diamondVBO);
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
        std::cout << "\nGame manually restarted by R key.\n"; // Optional: feedback
        restartGame(); // Call the unified restart function
    }
}

// glfw: whenever the window size changed this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}