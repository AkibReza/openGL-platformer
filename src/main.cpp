#include "glad.h"
#include "glfw3.h"
#include "GameConstants.h"
#include "GameObjects.h"
#include "Shaders.h"
#include "Utils.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void initGame();
void displayInstructions();
void updateHUD();

// Game state
float cameraOffset = 0.0f;
bool gameOver = false;
bool gameWin = false;
int score = 0;

// Game objects
Player player;
std::vector<Platform> platforms;
std::vector<Enemy> enemies;
std::vector<Coin> coins;
Flag levelFlag(LEVEL_END_X, -0.3f);

// Initialize game objects
void initGame() {
    // Set up platforms (x, y, width, height)
    platforms.push_back(Platform(-1.0f, -0.5f, 0.5f, 0.1f));
    platforms.push_back(Platform(-0.3f, -0.7f, 0.5f, 0.1f));
    platforms.push_back(Platform(0.3f, -0.5f, 0.5f, 0.1f));
    platforms.push_back(Platform(0.8f, -0.3f, 0.5f, 0.1f));
    platforms.push_back(Platform(1.3f, -0.5f, 0.5f, 0.1f));
    platforms.push_back(Platform(1.9f, -0.7f, 0.5f, 0.1f));
    platforms.push_back(Platform(2.4f, -0.5f, 0.5f, 0.1f));
    platforms.push_back(Platform(2.9f, -0.3f, 0.5f, 0.1f));
    platforms.push_back(Platform(3.4f, -0.5f, 0.7f, 0.1f));
    
    // Set up ground (a long platform at the bottom)
    platforms.push_back(Platform(-1.0f, -0.9f, 5.0f, 0.1f));
    
    // Add some platform gaps for challenging jumps
    platforms.push_back(Platform(0.0f, 0.0f, 0.2f, 0.1f));
    platforms.push_back(Platform(0.5f, 0.1f, 0.2f, 0.1f));
    platforms.push_back(Platform(1.0f, 0.2f, 0.2f, 0.1f));
    platforms.push_back(Platform(1.5f, 0.1f, 0.2f, 0.1f));
    platforms.push_back(Platform(2.0f, 0.0f, 0.2f, 0.1f));
    
    // Add a staircase pattern
    platforms.push_back(Platform(2.5f, 0.0f, 0.3f, 0.1f));
    platforms.push_back(Platform(2.8f, 0.1f, 0.3f, 0.1f));
    platforms.push_back(Platform(3.1f, 0.2f, 0.3f, 0.1f));
    platforms.push_back(Platform(3.4f, 0.3f, 0.3f, 0.1f));
    
    // Set up enemies
    enemies.push_back(Enemy(0.5f, -0.4f));
    enemies.push_back(Enemy(1.5f, -0.6f));
    enemies.push_back(Enemy(2.5f, -0.4f));
    enemies.push_back(Enemy(3.2f, -0.8f));
    
    // Set up coins
    coins.push_back(Coin(-0.3f, -0.5f));
    coins.push_back(Coin(0.3f, -0.3f));
    coins.push_back(Coin(1.3f, -0.3f));
    coins.push_back(Coin(2.3f, -0.3f));
    // Add coins on the higher platforms
    coins.push_back(Coin(0.0f, 0.2f));
    coins.push_back(Coin(0.5f, 0.3f));
    coins.push_back(Coin(1.0f, 0.4f));
    coins.push_back(Coin(1.5f, 0.3f));
    coins.push_back(Coin(2.0f, 0.2f));
    coins.push_back(Coin(3.4f, 0.5f));
}

// Function to display instructions at the start
void displayInstructions() {
    std::cout << "===== MARIO-LIKE PLATFORMER GAME =====" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  LEFT ARROW  - Move left" << std::endl;
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
void updateHUD() {
    // Display the score
    std::cout << "Score: " << std::setw(6) << std::setfill('0') << score << "                            \r";
    
    // Show coins collected
    int coinsCollected = 0;
    for (const Coin& coin : coins) {
        if (coin.collected) coinsCollected++;
    }
    std::cout << "Coins: " << coinsCollected << "/" << coins.size() << "                               " << std::endl;
}

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mario-like Platformer", NULL, NULL);
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
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Rectangle vertices (for drawing all game objects)
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,  // bottom left
         0.5f, -0.5f, 0.0f,  // bottom right
         0.5f,  0.5f, 0.0f,  // top right
         
        -0.5f, -0.5f, 0.0f,  // bottom left
         0.5f,  0.5f, 0.0f,  // top right
        -0.5f,  0.5f, 0.0f   // top left
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    // Setup VAO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Initialize game objects
    initGame();
    
    // Get transform uniform location
    glUseProgram(shaderProgram);
    int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    int colorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    
    // Variables for timing
    double lastTime = glfwGetTime();
    
    // Display instructions at startup
    displayInstructions();
    
    // render loop
    while (!glfwWindowShouldClose(window) && !gameOver && !gameWin)
    {
        // Calculate delta time
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        // Update animation timer
        player.animTime += deltaTime;
        if (player.animTime > 0.2f) { // Change animation frame every 0.2 seconds
            player.animTime = 0.0f;
            player.animFrame = (player.animFrame + 1) % 2; // Simple 2-frame animation
        }
        
        // input
        processInput(window);
        
        // Update player vertical position (gravity)
        player.velocityY -= GRAVITY * deltaTime * 1000;
        player.y += player.velocityY;
        
        // Check for platform collisions (for player)
        bool onGround = false;
        for (const Platform& platform : platforms) {
            // Check if player collides with platform
            if (checkCollision(
                player.x - player.width/2, player.y - player.height/2, player.width, player.height,
                platform.x - platform.width/2 - cameraOffset, platform.y - platform.height/2, platform.width, platform.height)) {
                    
                // Above platform
                if (player.velocityY < 0 && 
                    player.y - player.height/2 > platform.y - platform.height/2 + platform.height/2) {
                    player.y = platform.y + platform.height/2 + player.height/2;
                    player.velocityY = 0;
                    onGround = true;
                }
            }
        }
        
        // Update player state
        if (onGround) {
            player.isJumping = false;
        }
        
        // Screen boundaries
        if (player.y < -1.0f) {
            gameOver = true; // Fell out of the world
        }
        
        // Camera following player
        if (player.x > 0.0f) {
            cameraOffset = player.x;
        }
        
        // Update enemies (move back and forth)
        for (Enemy& enemy : enemies) {
            enemy.x += enemy.velocity;
            
            // Simple AI: reverse direction at edges
            if (enemy.x > 3.0f || enemy.x < -1.0f) {
                enemy.velocity = -enemy.velocity;
            }
            
            // Check for collision with player
            if (checkCollision(
                player.x - player.width/2, player.y - player.height/2, player.width, player.height,
                enemy.x - enemy.width/2 - cameraOffset, enemy.y - enemy.height/2, enemy.width, enemy.height)) {
                // Game over if player touches enemy
                gameOver = true;
            }
        }
        
        // Check for coin collection
        for (Coin& coin : coins) {
            if (!coin.collected && checkCollision(
                player.x - player.width/2, player.y - player.height/2, player.width, player.height,
                coin.x - coin.width/2 - cameraOffset, coin.y - coin.height/2, coin.width, coin.height)) {
                coin.collected = true;
                score += 100;
                std::cout << "Score: " << score << std::endl;
            }
        }

        // Check if player has reached the level end flag
        if (checkCollision(
            player.x - player.width/2, player.y - player.height/2, player.width, player.height,
            levelFlag.x - levelFlag.width/2 - cameraOffset, levelFlag.y - levelFlag.height/2, levelFlag.width, levelFlag.height)) {
            gameWin = true;
        }

        // Update HUD every few frames for better performance
        static int frameCount = 0;
        if (frameCount % 10 == 0) {
            updateHUD();
        }
        frameCount++;

        // render
        glClearColor(0.4f, 0.6f, 1.0f, 1.0f); // Sky blue background
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        
        // Draw platforms
        for (const Platform& platform : platforms) {
            Matrix4 transform;
            transform.translate(platform.x - cameraOffset, platform.y, 0.0f);
            transform.scale(platform.width, platform.height, 1.0f);
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
            glUniform4f(colorLocation, 0.0f, 0.5f, 0.0f, 1.0f); // Green
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        
        // Draw enemies
        for (const Enemy& enemy : enemies) {
            Matrix4 transform;
            transform.translate(enemy.x - cameraOffset, enemy.y, 0.0f);
            transform.scale(enemy.width, enemy.height, 1.0f);
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
            glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f); // Red
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        
        // Draw coins
        for (const Coin& coin : coins) {
            if (!coin.collected) {
                Matrix4 transform;
                transform.translate(coin.x - cameraOffset, coin.y, 0.0f);
                transform.scale(coin.width, coin.height, 1.0f);
                glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
                glUniform4f(colorLocation, 1.0f, 1.0f, 0.0f, 1.0f); // Yellow
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
        
        // Draw level end flag
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
        
        // Draw player with rudimentary animations
        Matrix4 transform;
        transform.translate(player.x, player.y + (player.animFrame * 0.01f), 0.0f); // Simple bounce animation
        
        // Draw player body
        transform.scale(player.width, player.height, 1.0f);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform.m);
        glUniform4f(colorLocation, 0.0f, 0.0f, 1.0f, 1.0f); // Blue player
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Draw player eyes (white circles)
        Matrix4 eyeTransform;
        float eyeDirection = player.facingRight ? 0.02f : -0.02f;
        
        // Right/forward eye
        eyeTransform = Matrix4();
        eyeTransform.translate(player.x + eyeDirection, player.y + 0.02f, 0.0f);
        eyeTransform.scale(0.02f, 0.02f, 1.0f);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, eyeTransform.m);
        glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f); // White
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (gameWin) {
        std::cout << std::endl << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "   CONGRATULATIONS! YOU WON!" << std::endl;
        std::cout << "   Final Score: " << score << std::endl;
        std::cout << "=====================================" << std::endl;
    } else if (gameOver) {
        std::cout << std::endl << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "   GAME OVER!" << std::endl;
        std::cout << "   Final Score: " << score << std::endl;
        std::cout << "   Press R to try again" << std::endl;
        std::cout << "=====================================" << std::endl;
    }

    // optional: de-allocate all resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
        
    // Player movement
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        player.x += MOVEMENT_SPEED;
        player.facingRight = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        player.x -= MOVEMENT_SPEED;
        player.facingRight = false;
    }
    if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || 
         glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && !player.isJumping) {
        player.velocityY = JUMP_FORCE;
        player.isJumping = true;
    }
    
    // Restart game with R key
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
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
        for (Coin& coin : coins) {
            coin.collected = false;
        }
    }
}

// glfw: whenever the window size changed this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}