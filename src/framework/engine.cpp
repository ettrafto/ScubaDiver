
#include "engine.h"

enum state {start, play, over};
state screen;

// Colors
color originalFill, outlineFill, pressFill;

Engine::Engine() : keys() {
    this->initWindow();
    this->initShaders();
    this->initShapes();
}

Engine::~Engine() {}


void Engine::addWalls(int x, int y, std::vector<std::pair<int, int>>& walls) {
    // Add the neighboring walls to the list
    if (x > 1) walls.push_back({x - 2, y});
    if (x < 2 * gameDimensions - 2) walls.push_back({x + 2, y});
    if (y > 1) walls.push_back({x, y - 2});
    if (y < 2 * gameDimensions - 2) walls.push_back({x, y + 2});
}
void Engine::generateMaze() {

    rectDimen = static_cast<float>(gameHeight) / gameDimensions;
    vec2 squareSize = { rectDimen, rectDimen };

    // Initialize the maze with all walls and create Rect objects
    for (int y = 0; y < gameDimensions * 2 - 1; ++y) {
        vector<Rect> rects = {};
        for (int x = 0; x < gameDimensions * 2 - 1; ++x) {
            // Set the position based on the grid and rectDimen
            vec2 position = {x * rectDimen, y * rectDimen};

            // Set the color based on whether it's a wall or not
            color fill = gray; // Initially, set all rectangles to be walls

            // Create a Rect object based on the position, size, and color
            rects.push_back(Rect(shapeShader, position, squareSize, yellow));
        }
        maze.push_back(rects);
    }

    // Initialize a list to store walls
    std::vector<std::pair<int, int>> walls;

    // Start from a random point
    int startX = rand() % gameDimensions;
    int startY = rand() % gameDimensions;

    // Mark the starting point as part of the maze
    maze[startY][startX].setWall(false);

    // Add neighboring walls to the list
    addWalls(startX, startY, walls);

    while (!walls.empty()) {
        // Randomly select a wall from the list
        int randomIndex = rand() % walls.size();
        auto randomWall = walls[randomIndex];
        int wallX = randomWall.first;
        int wallY = randomWall.second;

        // Find the adjacent cell
        int adjX, adjY;
        if (wallX % 2 == 0) {
            adjX = wallX;
            adjY = (wallY - 1) / 2;
        } else {
            adjX = (wallX - 1) / 2;
            adjY = wallY;
        }

        // Check if the adjacent cell is not part of the maze
        if (maze[adjY][adjX].isWall()) {
            // Connect the wall to the maze
            maze[wallY][wallX].setWall(false);

            // Mark the adjacent cell as part of the maze
            maze[adjY][adjX].setWall(false);

            // Add neighboring walls to the list
            addWalls(wallX, wallY, walls);
        }

        // Remove the selected wall from the list
        walls.erase(walls.begin() + randomIndex);
    }
        // TODO: debug isWalls

        //render();


}


unsigned int Engine::initWindow(bool debug) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    window = glfwCreateWindow(width, height, "Lights Out!", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    /*
     *
     *
     * int screenHeight = mode-> height;
    screenHeight -= 300;
    height = screenHeight;
    width = screenHeight;

    gameWidth = height*.9;
    gameHeight = height*.9;

    gameWindowPosX = height*.1;
    gameWindowPosY = height*.1;

    cout << "height:" <<height <<endl;
    cout << "gameHeight:" <<gameHeight <<endl;
    cout << "gameWindowPosX:" <<gameWindowPosX <<endl;
     *
     *
     * */



    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // OpenGL configuration
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(1);


    screen = play;
    return 0;
}

void Engine::initShaders() {
    // load shader manager
    shaderManager = make_unique<ShaderManager>();

    // Load shader into shader manager and retrieve it
    shapeShader = this->shaderManager->loadShader("../res/shaders/shape.vert", "../res/shaders/shape.frag",  nullptr, "shape");

    // Configure text shader and renderer
    textShader = shaderManager->loadShader("../res/shaders/text.vert", "../res/shaders/text.frag", nullptr, "text");
    fontRenderer = make_unique<FontRenderer>(shaderManager->getShader("text"), "../res/fonts/MxPlus_IBM_BIOS.ttf", 24);

    // Set uniforms
    textShader.use().setVector2f("vertex", vec4(100, 100, .5, .5));
    shapeShader.use().setMatrix4("projection", this->PROJECTION);
}

void Engine::initShapes() {

    //rectDimen = static_cast<float>(gameHeight) / gameDimensions;
    //vec2 squareSize = { rectDimen, rectDimen };

    // Generate the maze using Prim's algorithm
    //generateMaze();


    // Loop through the maze and create Rect objects based on the maze configuration
    //for (int y = 0; y < gameDimensions * 2 - 1; ++y) {
        //for (int x = 0; x < gameDimensions * 2 - 1; ++x) {

            // Set the position based on the grid and rectDimen
            //vec2 position = {x * rectDimen, y * rectDimen};



            // Set the color based on whether it's a wall or not
            //color fill = maze[y][x].isWall() ? gray : yellow;


            // Create a Rect object based on the position, size, and color
            //maze[y].push_back(Rect(shapeShader, position, squareSize, yellow));





}

void Engine::processInput() {
    glfwPollEvents();

    glfwPollEvents();
    // Mouse position is inverted because the origin of the window is in the top left corner
    bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    //creates array of states and valid counter
    bool RectStatus[25];
    int valid = 0;
    // Set keys to true if pressed, false if released
    for (int key = 0; key < 1024; ++key) {
        if (glfwGetKey(window, key) == GLFW_PRESS)
            keys[key] = true;
        else if (glfwGetKey(window, key) == GLFW_RELEASE)
            keys[key] = false;
    }
// Close window if escape key is pressed
    if (keys[GLFW_KEY_ESCAPE])
        glfwSetWindowShouldClose(window, true);
// Mouse position saved to check for collisions
    glfwGetCursorPos(window, &MouseX, &MouseY);
    MouseY = height - MouseY;
// TODO: If we're in the start screen and the user presses s, change screen to play
// Hint: The index is GLFW_KEY_S
    if (keys[GLFW_KEY_S])
        screen = play;


}

void Engine::update() {
    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // TODO: End the game when there are no more light up squares

}

void Engine::render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set shader to use for all shapes
    shapeShader.use();

    // Render differently depending on screen
    switch (screen) {
        case start: {
            string message = "Welcome to Lights out!";
            string instructions = "Click each light";
            string instructions1= "to turn them off";
            string instructions2 = "Pres S to start!";
            // (12 * message.length()) is the offset to center text.
            // 12 pixels is the width of each character scaled by 1.
            this->fontRenderer->renderText(message, 125 , 450, 1, vec3{1, 1, 1});
            this->fontRenderer->renderText(instructions, 125 , 340, 1, vec3{1, 1, 1});
            this->fontRenderer->renderText(instructions1, 125 , 300, 1, vec3{1, 1, 1});
            this->fontRenderer->renderText(instructions2, 125 , 150, 1, vec3{1, 1, 1});
            break;
        }

        case play: {
            //generateMaze
            generateMaze();

            // Draw the maze
            for (int y = 0; y < gameDimensions*2 - 1; ++y) {
                for (int x = 0; x < gameDimensions*2 - 1; ++x) {
                    // Set the position based on the grid and rectDimen
                    vec2 position = {x * rectDimen, y * rectDimen};

                    // Set the size based on your requirements
                    vec2 size = {rectDimen, rectDimen};

                    // Set the color based on whether it's a wall or not
                    color fill = maze[y][x].isWall() ? gray : yellow;
                    maze[y][x].setColor(fill);
                    bool temp = maze[y][x].isWall();
                    cout << "x:" << x << " y:"<< y << "isWall: " << temp << endl;


                    //maze[y][x].setUniforms();

                    //maze[y][x].draw();

                }
            }
            break;
        }

        case over: {
            string message = "You win!";
            break;
        }
    }

    glfwSwapBuffers(window);
}


bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}
