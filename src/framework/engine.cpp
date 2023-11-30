
#include "engine.h"

enum state {start,gameMenu,pause, play, over};
state screen;

// Colors
color originalFill, outlineFill, pressFill;

Engine::Engine() : keys() {

    this->initWindow();
    this->initShaders();
    this->initShapes();
}

Engine::~Engine() {}

void Engine::caveGeneration() {


    rectDimen = height / gameDimensions;
    vec2 squareSize = {rectDimen, rectDimen};

    // Initialize the map with all walls and create Rect objects

    for (int y = 0; y < gameDimensions; ++y) {
        vector<Rect> rects = {};
        for (int x = 0; x < gameDimensions; ++x) {
            // Set the position based on the grid and rectDimen
            vec2 position = {x * rectDimen, y * rectDimen};

            // Set the color based on whether it's a wall or not
            color fill = gray; // Initially, set all rectangles to be walls

            // Create a Rect object based on the position, size, and color
            rects.push_back(Rect(shapeShader, position, squareSize, fill));
            rects[x].setWall(true);


        }
        map.push_back(rects);
    }
}



       /* // Initialize a list to store walls
        std::vector<std::pair<int, int>> walls;

        // Start from a random point
        int startX = rand() % gameDimensions;
        int startY = rand() % gameDimensions;

        // Mark the starting point as part of the maze
        maze[startY * 2][startX * 2].setWall(false);

        // Add neighboring walls to the list
        addWalls(startX * 2, startY * 2, walls);

        int iteration = 0;
        while (!walls.empty()) {
            // Randomly select a wall from the list
            int randomIndex = rand() % walls.size();
            auto randomWall = walls[randomIndex];
            int wallX = randomWall.first;
            int wallY = randomWall.second;

            cout << "WallX:" << wallX << " wallY: " << wallY << endl;

            // Find the adjacent cell
            int adjX, adjY;

            if (wallX % 2 == 0) {
                // Vertical wall edge
                adjX = wallX;
                adjY = wallY - 1;
                // Ensure adjY is within bounds
                if (adjY < 0) {
                    adjY = 0; // Adjust to the minimum valid index
                }
            } else {
                // Horizontal wall edge
                adjX = wallX - 1;
                adjY = wallY;
                // Ensure adjX is within bounds
                if (adjX < 0) {
                    adjX = 0; // Adjust to the minimum valid index
                }
            }

            // Now, you may also want to check the upper bounds of adjX and adjY
            // Assuming gameDimensions is the maximum valid index
            if (adjX >= gameDimensions * 2 - 1) {
                adjX = gameDimensions * 2 - 2; // Adjust to the maximum valid index
            }

            if (adjY >= gameDimensions * 2 - 1) {
                adjY = gameDimensions * 2 - 2; // Adjust to the maximum valid index
            }

            cout << "AdjX:" << adjX << " AdjY: " << adjY << endl;

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

            cout << iteration << endl;
            iteration = 1 + iteration;
        }*/
   // }




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

    // Get the video mode
  //const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    // Set window size to the size of the monitor
// width= mode->width;
//  height = mode->height;
//  height *= .91;

    // Create the window
    window = glfwCreateWindow(width, height, "Lights Out!", nullptr, nullptr);
    if (!window) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    // Make the window context current
    glfwMakeContextCurrent(window);

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

    // Set the initial screen
    screen = start;

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

   caveGeneration();

    buttonWidth= width*.1;
    buttonHeight= height*0.05;

    vec2 buttonSize = {buttonWidth,buttonHeight};

    vec2 startButtonPos = {width/2,height/2};
    vec2 quitButtonPos = {width/2,height/2-200};
    //start and quit buttons
    startButton = make_unique<Rect>(shapeShader, vec2{width/2,height/2}, vec2{100, 50}, color{1, 0, 0, 1});

    startButton = make_unique<Rect>(shapeShader, startButtonPos, buttonSize, color{1, 1, 1, 1});
    quitButton = make_unique<Rect>(shapeShader, quitButtonPos, buttonSize, color{1, 1, 1, 1});



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
    // Mouse position is inverted because the origin of the window is in the top left corner
    glfwGetCursorPos(window, &MouseX, &MouseY);

    // Set keys to true if pressed, false if released
    for (int key = 0; key < 1024; ++key) {
        if (glfwGetKey(window, key) == GLFW_PRESS)
            keys[key] = true;
        else if (glfwGetKey(window, key) == GLFW_RELEASE)
            keys[key] = false;
    }

    MouseY = height - MouseY; // Invert y-axis of mouse position


    //cout <<"MouseX: "<< MouseX<< "MouseY: " << MouseY << endl;

    if (screen == start) {
        bool startButtonOverlapsMouse = startButton->isOverlapping(vec2(MouseX, MouseY));
        bool quitButtonOverlapsMouse = quitButton->isOverlapping(vec2(MouseX, MouseY));

        bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

        if (startButtonOverlapsMouse) {
            startButton->setColor(vec4(1.0f, 1.0f, 1.0f, 0.7f)); // Change color when hovering
        } else {
            startButton->setColor(vec4(1.0f, 1.0f, 1.0f, 1.0f)); // Reset color when not hovering
        }
        if (quitButtonOverlapsMouse) {
            quitButton->setColor(vec4(1.0f, 1.0f, 1.0f, 0.7f)); // Change color when hovering
        } else {
            quitButton->setColor(vec4(1.0f, 1.0f, 1.0f, 1.0f)); // Reset color when not hovering
        }
        // When in play screen, if the button was released, then spawn confetti
        if (mousePressed && startButtonOverlapsMouse) {
            screen = play;
        }
        if (mousePressed && quitButtonOverlapsMouse) {
            glfwSetWindowShouldClose(window, true);
        }

        //screen = play;
        mousePressedLastFrame = mousePressed;

    }






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
            string startLabel = "Welcome to ScubaDiver";
            string playLabel = "Play";
            string quitLabel = "Quit";

            startButton->setUniforms();
            startButton->draw();
            quitButton->setUniforms();
            quitButton->draw();


            this->fontRenderer->renderText(startLabel, 290,340, .5, vec3{1, 1, 1});


            this->fontRenderer->renderText(playLabel,368,292,.7, vec3{0, 0, 0});
            this->fontRenderer->renderText(quitLabel,368,232,.7, vec3{0, 0, 0});

            break;
        }

        case play: {
            //generateMaze

            // Draw the maze

            for (int y = 0; y < gameDimensions; ++y) {
                for (int x = 0; x < gameDimensions; ++x) {
                    // Set the position based on the grid and rectDimen
                    vec2 position = {x * rectDimen, y * rectDimen};

                    // Set the size based on your requirements
                    vec2 size = {rectDimen, rectDimen};

                    // Set the color based on whether it's a wall or not
                    color fill = map[y][x].isWall() ? gray : yellow;
                    map[y][x].setColor(fill);
                    //bool temp = map[y][x].isWall();
                    //cout << "x:" << x << " y:"<< y << "isWall: " << temp << endl;


                    map[y][x].setUniforms();

                    map[y][x].draw();

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
