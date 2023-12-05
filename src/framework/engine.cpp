
#include <iomanip>
#include "engine.h"

enum state {start,test,gameMenu,pause, play, over};
state screen;

// Colors
color originalFill, outlineFill, pressFill;

Engine::Engine() : keys() {

    this->initWindow();
    this->initShaders();
    this->initShapes();
}

Engine::~Engine() {}

bool Engine::pix2Rect(int x, int y) {
    int newX = x/rectDimen;
    int newY = y/rectDimen;

    return map[newY][newX].isWall();
}
bool Engine::ifValidMove(int dir, bool speed){

    float playerX = player->getPosX();
    float playerY = player->getPosY();

    float speedOut;
    if(speed){
        speedOut = 2.0f;
    }else{
        speedOut = 1.0f;
    }


    if(dir==1) {
        playerY = playerY + rectDimen/2;
        if (pix2Rect(playerX, playerY + (speedOut))) {

            return false;
        }
    }
    if(dir==-1) {
        playerY = playerY - rectDimen/2;
        if (pix2Rect(playerX, playerY - (speedOut))) {
            return false;
        }
    }
    if(dir==2) {
        playerX = playerX + rectDimen/2;
        if (pix2Rect(playerX + (speedOut), playerY )) {
            return false;
        }
    }
    if(dir==-2) {
        playerX = playerX - rectDimen/2;
        if (pix2Rect(playerX - (speedOut), playerY )) {
            return false;
        }
    }
    return true;

}

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
    std::vector<std::pair<int, int>> nonWallPositions;

    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {

            if (!map[y][x].isWall()) {
                // Add non-wall positions to the vector
                nonWallPositions.push_back(std::make_pair(x, y));
            }else{
                wallPositions.push_back(std::make_pair(x,y));
            }
        }
    }

    //setting user to a random startpoint that is not a wall
    if (!nonWallPositions.empty()) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dis(0, nonWallPositions.size() - 1);

        size_t randomIndex = dis(gen);
        int startX = nonWallPositions[randomIndex].first;
        int startY = nonWallPositions[randomIndex].second;


        // Set the player's start point
        playerStart = (vec2(startX * rectDimen + rectDimen / 2, startY * rectDimen + rectDimen / 2));
        //cout<<playerStart.x<<" "<<playerStart.y<<endl;

    }
    for (int i=0;i<numTreasure;i++){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dis(0, nonWallPositions.size() - 1);

        size_t randomIndex = dis(gen);
        int posX = nonWallPositions[randomIndex].first;
        int posY = nonWallPositions[randomIndex].second;

        //cout<<posX<<posY<<endl;

        vec2 treasurePosition{posX*rectDimen+rectDimen/2,posY*rectDimen+rectDimen/2};
        treasurePos.push_back(treasurePosition);
    }

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

    // Create the window
    window = glfwCreateWindow(width, height, "ScubaDiver", nullptr, nullptr);
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

    //creating treasure rects
    for (int i = 0; i < numTreasure; i++) {
        vec2 pos = treasurePos[i];
        auto t = std::make_unique<Rect>(shapeShader, pos, vec2{rectDimen, rectDimen}, color{1, 1, 0, 1});
        treasure.push_back(std::move(t));
    }

    //creating player
    player = make_unique<Rect>(shapeShader, vec2{width/2,height/2}, vec2{rectDimen, rectDimen}, color{1, 0, 0, 1});

    //setting player to playerStartPos
    player->setPos(playerStart);

    background = make_unique<Rect>(shapeShader, vec2{width/2,height/2},vec2{width,height}, gray);


    //creating game menu
    float menuWidth = width* 0.15;
    float menuHeight = height * 0.088;

    float menuX = width-menuWidth/2;
    float menuY = height-menuHeight/2;

    vec2 menuPos = {menuX,menuY};
    vec2 menuSize = {menuWidth,menuHeight};

    gameMenu = make_unique<Rect>(shapeShader, menuPos, menuSize, color{1, 1, 1, .95});

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
    if (screen == play) {

        vec2 position = player->getPos();
        vec2 newPosition =  player->getPos();;

        float buttonWidth = player->getSize().x;
        float buttonHeight = player->getSize().y;

        //pressing the shift button doubles speed
        if (keys[GLFW_KEY_UP] && newPosition.y + buttonHeight / 2.0f + 1.0f <= height) {
            if (keys[GLFW_KEY_LEFT_SHIFT]){
                if(ifValidMove(1,true)) {
                    newPosition.y += 2.0f;
                }
            }else {
                if(ifValidMove(1,false)) {
                    newPosition.y += 1.0f;
                }
            }
        }

        if (keys[GLFW_KEY_DOWN] && newPosition.y - buttonHeight / 2.0f - 1.0f >= 0) {
            if (keys[GLFW_KEY_LEFT_SHIFT]){
                if(ifValidMove(-1,true)) {
                    newPosition.y -= 2.0f;
                }
            }else {
                if(ifValidMove(-1,false)) {
                    newPosition.y -= 1.0f;
                }
            }
        }

        if (keys[GLFW_KEY_LEFT] && newPosition.x - buttonWidth / 2.0f - 1.0f >= 0) {
            if (keys[GLFW_KEY_LEFT_SHIFT]){
                if(ifValidMove(-2,true)) {
                    newPosition.x -= 2.0f;
                }
            }else {
                if(ifValidMove(-2,false)) {
                    newPosition.x -= 1.0f;
                }
            }
        }

        if (keys[GLFW_KEY_RIGHT] && newPosition.x + buttonWidth / 2.0f + 1.0f <= width) {
            if (keys[GLFW_KEY_LEFT_SHIFT]){
                if(ifValidMove(2,true)) {
                    newPosition.x += 2.0f;
                }
            }else {
                if(ifValidMove(2,false)) {
                    newPosition.x += 1.0f;
                }
            }
        }


        player->setPos(newPosition);
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

    //used to display time
    if(screen==play) {
        totalTime += deltaTime;
    }
    // Get the player's movement speed
    float movementSpeed = keys[GLFW_KEY_LEFT_SHIFT] ? 2.0f : 1.0f;

    // Define O2 decrease rates based on movement speed
    float o2DecreaseRate = 0.0f;
    if (movementSpeed == 0.0f) {
        // If not moving
        o2DecreaseRate = 0.05f;
    } else if (movementSpeed == 1.0f) {
        // If moving at a speed of 1.0f
        o2DecreaseRate = 0.1f;
    } else if (movementSpeed == 2.0f) {
        // If moving at a speed of 2.0f
        o2DecreaseRate = 0.3f;
    }

    // Decrease O2 based on the defined rate
    if(screen==play) {
        O2 -= o2DecreaseRate * deltaTime;
    }
    // Check if O2 is less than or equal to 0
    if (O2 <= 0.0f && screen != over) {
        // Handle game over condition
        screen = over;


        endTime=totalTime;
    }




    // Assuming playerRadius and treasureRadius are defined appropriately
    float playerRadius = player->getSize().x / 2.0f;
    if(treasure.size() ==0 ){
        screen=over;
        return;
    }
    float treasureRadius = treasure[0]->getSize().x / 2.0f;

    // Assuming there is a loop to iterate through treasure objects
    for (int i = 0; i < treasure.size(); ++i) {
        vec2 playerPos = player->getPos();
        vec2 treasurePos = treasure[i]->getPos();

        // Calculate the distance between the player and the treasure
        float distance = glm::length(playerPos - treasurePos);

        // Check for collision between player and treasure
        if (distance < (playerRadius + treasureRadius)) {

            // Player collected the treasure, so remove it
            treasure.erase(treasure.begin() + i);

            // Decrease the count of remaining treasures
            numTreasure--;

            //removing a treasure adds a point
            points++;


            break;
        }
    }
    if(treasure.size() == 0 && screen != over){
        screen = over;
        endTime = totalTime;
    }
}

void Engine::render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set shader to use for all shapes
    shapeShader.use();

    // Render differently depending on screen
    switch (screen) {
        /*case test:{
            testRect->setUniforms();
            testRect->draw();
        }*/
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
            this->fontRenderer->renderText(quitLabel,368,225,.7, vec3{0, 0, 0});

            break;
        }

        case play: {

            // Draw the map
            /*for (int y = 0; y < gameDimensions; ++y) {
                for (int x = 0; x < gameDimensions; ++x) {



                }
            }*/
            background->setUniforms();
            background->draw();

            for(int i=0;i<wallPositions.size();i++){

                int wallX = wallPositions[i].first;
                int wallY = wallPositions[i].second;
                color fill = map[wallY][wallX].isWall() ? black: gray;

                map[wallY][wallX].setColor(fill);
                map[wallY][wallX].setUniforms();
                map[wallY][wallX].draw();
            }

            //draw treasure
            for (int i=0;i<numTreasure;i++){


                treasure[i]->setUniforms();
                treasure[i]->draw();
            }

            //draw player
            player->setUniforms();
            player->draw();


            //drawing menu
            gameMenu->setUniforms();
            gameMenu->draw();

            // Convert totalTime to minutes and seconds
            int minutes = static_cast<int>(totalTime) / 60;
            int seconds = static_cast<int>(totalTime) % 60;

            string pointsLabel = "Points: " + std::to_string(points);

            // Ensure O2 is not less than 0
            O2 = std::max(O2, 0.0f);

            // Calculate the percentage of O2 relative to totalO2
            float o2Percentage = (O2 / startO2) * 100.0f;

            // Convert o2Percentage to a string with 1 decimal point and concatenate with the label
            std::ostringstream o2Stream;
            o2Stream << std::fixed << std::setprecision(1) << o2Percentage;
            string o2Label = "O2: " + o2Stream.str() + "%";

            string timeLabel = "Time: " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s";

            this->fontRenderer->renderText(pointsLabel, 690,585, .35, vec3{0, 0, 0});
            this->fontRenderer->renderText(o2Label,690,570,.35, vec3{0, 0, 0});
            this->fontRenderer->renderText(timeLabel,690,555,.35, vec3{0, 0, 0});


            break;
        }

        case over: {
            //TODO: CREATE END GAME MENU
            string message = "Game Over";
            this->fontRenderer->renderText(message,350,340, .7, vec3{1, 1, 1});

            // Convert totalTime to minutes and seconds
            int minutes = static_cast<int>(endTime) / 60;
            int seconds = static_cast<int>(endTime) % 60;

            string pointsLabel = "Points: " + std::to_string(points);

            // Ensure O2 is not less than 0
            O2 = std::max(O2, 0.0f);

            // Calculate the percentage of O2 relative to totalO2
            float o2Percentage = (O2 / startO2) * 100.0f;

            // Convert o2Percentage to a string with 1 decimal point and concatenate with the label
            std::ostringstream o2Stream;
            o2Stream << std::fixed << std::setprecision(1) << o2Percentage;
            string o2Label = "O2: " + o2Stream.str() + "%";

            string timeLabel = "Time: " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s";

            this->fontRenderer->renderText(pointsLabel, 350,320, .35, vec3{1, 1, 1});
            this->fontRenderer->renderText(o2Label,350,300,.35, vec3{1, 1, 1});
            this->fontRenderer->renderText(timeLabel,350,280,.35, vec3{1, 1, 1});


            break;
        }
    }

    glfwSwapBuffers(window);
}


bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}
