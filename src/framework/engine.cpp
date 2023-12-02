
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

int Engine::getSurroundingWallCount(int wallX, int wallY) {
    int count = 0;

    for (int neighborX = wallX-1; neighborX <= wallX+1; neighborX++) {
        for (int neighborY = wallY-1; neighborY <= wallY+1; neighborY++) {
            if(neighborX >= 0 && neighborX < gameDimensions && neighborX >= 0 && neighborX < gameDimensions){
                if(neighborX != wallX || neighborY != wallY){
                    if (map[neighborX][neighborY].isWall()){
                        count++;
                    }
                }
            }else{
                count++;
            }
        }
    }
    return count;
}

void Engine::smoothMap() {
    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {
            int neighborWalls = getSurroundingWallCount(x,y);
            if (neighborWalls > 4){
                map[x][y].setWall(true);
            }else if(neighborWalls < 4){
                map[x][y].setWall(false);
            }

        }
    }

}


void Engine::caveGeneration() {

    //setting size of singular rectangle
    rectDimen = height / gameDimensions;
    vec2 squareSize = {rectDimen, rectDimen};

    // Initialize the map with all walls and create Rect objects
    vector<Rect> row = {};
    for (int y = 0; y < gameDimensions; ++y) {
        row.clear();
        for (int x = 0; x < gameDimensions; ++x) {

            // Set the position based on the grid and rectDimen
            vec2 position = {x * rectDimen+rectDimen/2, y * rectDimen+rectDimen/2};


            // Create a Rect object based on the position, size, and color
            Rect r = Rect(shapeShader, position, squareSize, black);

            //pushing rect onto row array
            row.push_back(r);

        }
        //adding rows to 2d vec
        map.push_back(row);
    }

    //creating and defining random device
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);


    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {
            //create a random noise grid

            double randomValue = dis(gen);

            int noisePercent = 47;

            if (randomValue < noisePercent / 100.0) {
                // Set as wall
                map[y][x].setWall(true);
            } else {
                // Not a wall
                map[y][x].setWall(false);
            }

        }
    }

    //NOISE GRID GENERATED
    for (int i=0;i<5;i++){
        smoothMap();
    }
    //set borders to walls
    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {
            if(x==0||x==gameDimensions-1||y==0||y==gameDimensions-1) {
                map[y][x].setWall(true);
            }
        }
    }

    /*
    const int gameDimensions = 10; // Replace 10 with your desired dimensions

    float rectDimen = height / gameDimensions;
    vec2 squareSize = {rectDimen, rectDimen};

// Initialize the map with all walls and create Rect objects


    for (int y = 0; y < gameDimensions; ++y) {
        for (int x = 0; x < gameDimensions; ++x) {
            // Set the position based on the grid and rectDimen
            vec2 position = {x * rectDimen, y * rectDimen};

            color fill = gray;

            // Create a Rect object based on the position, size, and color
            map[y][x] = Rect(shapeShader, position, squareSize, fill);

            //if (twoDArray[x][y]){
            //    map[y][x].setWall(1);
            //} else {
            //    map[y][x].setWall(0);
            //}
        }
    }

    for (int y = 0; y < gameDimensions; ++y) {
        for (int x = 0; x < gameDimensions; ++x) {
            // create a random noise grid
            int noisePercent = 50; // Replace 50 with your desired noise percentage

            //if ((rand() % 100) < noisePercent) {
            //    map[y][x].setWall(true); // Set as wall
            //} else {
            //    map[y][x].setWall(false); // Not a wall
            //}
            map[x][y].setWall(false);

            cout << map[y][x].isWall() << " ";

            // Set the color based on whether it's a wall or not
            color fill = map[y][x].isWall() ? gray : yellow;
            map[y][x].setColor(fill);
        }
        cout << endl;
    }
}*/


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

    /*
    attempting to resize screen to display size, creates many coordinate problems

    // Get the video mode
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    // Set window size to the size of the monitor
     width= mode->width;
     height = mode->height;
     height *= .91;

    */

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

    //generating map
    caveGeneration();

    //button size 10% of width, 5% of height
    buttonWidth= width*.1;
    buttonHeight= height*0.05;
    vec2 buttonSize = {buttonWidth,buttonHeight};

    //button position = middle width, middle height (quit moved 200 below)
    vec2 startButtonPos = {width/2,height/2};
    vec2 quitButtonPos = {width/2,height/2-200};

    //start and quit buttons
    startButton = make_unique<Rect>(shapeShader, startButtonPos, buttonSize, color{1, 1, 1, 1});
    quitButton = make_unique<Rect>(shapeShader, quitButtonPos, buttonSize, color{1, 1, 1, 1});

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

    // Invert y-axis of mouse position
    MouseY = height - MouseY;

    //debugging mouse cords
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

        mousePressedLastFrame = mousePressed;

    }

    // Close window if escape key is pressed
    if (keys[GLFW_KEY_ESCAPE])
        glfwSetWindowShouldClose(window, true);

    // Mouse position saved to check for collisions
    glfwGetCursorPos(window, &MouseX, &MouseY);
    MouseY = height - MouseY;




}

void Engine::update() {

    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;


}

void Engine::render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set shader to use for all shapes
    shapeShader.use();

    // Render differently depending on screen
    switch (screen) {
        case start: {
            //declaring strings to be displayed
            string startLabel = "Welcome to ScubaDiver";
            string playLabel = "Play";
            string quitLabel = "Quit";

            //drawing start and quit buttons
            startButton->setUniforms();
            startButton->draw();
            quitButton->setUniforms();
            quitButton->draw();

            //drawing text
            this->fontRenderer->renderText(startLabel, 290,340, .5, vec3{1, 1, 1});
            this->fontRenderer->renderText(playLabel,368,292,.7, vec3{0, 0, 0});
            this->fontRenderer->renderText(quitLabel,368,232,.7, vec3{0, 0, 0});

            break;
        }

        case play: {


            // Draw the maze
            for (int y = 0; y < gameDimensions; ++y) {
                for (int x = 0; x < gameDimensions; ++x) {


                    color fill = map[y][x].isWall() ? black: gray;
                    map[y][x].setColor(fill);

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
