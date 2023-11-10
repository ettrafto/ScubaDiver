
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
    // outline
    vec2 squareSize2 = vec2{90,90};
    for (int y = 50; y < 501; y+= 100){
        for (int x = 50; x < 501; x+= 100){
            outline.push_back(make_unique<Rect>(shapeShader, vec2{x, y}, squareSize2, transparent));
        }
    }

    // squares
    int numSquares = 25;
    vec2 squareSize = vec2{75,75};
    for (int y = 50; y < 501; y+= 100){
        for (int x = 50; x < 501; x+= 100){
            squares.push_back(make_unique<Rect>(shapeShader, vec2{x, y}, squareSize, yellow));
        }
    }

}

void Engine::processInput() {
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

    if (screen == play) {

        // Check if the mouse is hovering over any of the squares
        for (auto &s: outline) {
            bool isHovered = s->isMouseOver(*s, MouseX, MouseY);
            s->setHover(isHovered);
        }

        for (int i = 0; i < squares.size(); ++i) {
            bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            // Use a separate boolean to track whether the square was clicked on this frame
            bool clicked = squares[i]->isMouseOver(*squares[i], MouseX, MouseY) && mousePressed && !mousePressedLastFrame;

            if (clicked) {
                // Toggle the status of the square
                rectStatus[i] = !rectStatus[i];

                // Now set the color based on the new status
                if (rectStatus[i]) {
                    squares[i]->setColor(gray);
                } else {
                    squares[i]->setColor(yellow);
                }
            } else {
                // If not clicked, just set the color based on the current status
                if (rectStatus[i]) {
                    squares[i]->setColor(gray);
                } else {
                    squares[i]->setColor(yellow);
                }
            }
        }

// At the end of the frame, remember to update `mousePressedLastFrame`
        mousePressedLastFrame = mousePressed;


        for (bool status: RectStatus) {
            if (!status) {
                valid += valid;
            }
            if (valid == 25) {
                screen = over;
            }
        }
    }

    // TODO: When in play screen, if the user clicks a lit square, change it to unlit
    // Adding so i can push it

    // TODO: When in play screen, if the user clicks an unlit square, change it to lit

    // TODO: When a user clicks a square, change the 4 surrounding squares to their opposite value (lit -> unlit, etc.)
    //when user clicks

    // Select x+1, x-1, x+5, and x-5 (if they are within bounds)
    int clickedSquare;
    int x = clickedSquare;
    if (x >= 0 && x < 25) {
        int xPlus1 = (x + 5 < 25) ? (x + 5) : -1;
        int xMinus1 = (x - 1 >= 0) ? (x + 5) : -1;
        int xPlus5 = (x + 5 < 25) ? (x + 5) : -1;
        int xMinus5 = (x - 5 < 25) ? (x + 5) : -1;

if (xPlus1 != -1){
    if (!rectStatus[xPlus1]) {
        rectStatus[xPlus1] = true;
    } else {
        rectStatus[xPlus1] = false;
    }
}

if (xMinus1 != -1){
    if (!rectStatus[xMinus1]) {
        rectStatus[xMinus1] = true;
    } else {
        rectStatus[xMinus1] = false;
    }
}

if (xMinus5 != -1){
    if (!rectStatus[xMinus5]) {
        rectStatus[xMinus5] = true;
    } else {
        rectStatus[xMinus5] = false;
    }
}

if(xPlus5 != -1){
    if (!rectStatus[xPlus5]) {
        rectStatus[xPlus5] = true;
    } else {
        rectStatus[xPlus5] = false;
    }
}

    }
}


    // Hint: the button was released if it was pressed last frame and is not pressed now
    // TODO: Make sure the square is not outlined when the user is not hovering.

/*
// Save mousePressed for next frame
mousePressedLastFrame = mousePressed;

*/


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
            // Draw the outlines first
            for (auto &o : outline) {
                // Set uniforms for the shader
                o->setUniforms();

                if (o->isHovered()) {
                    o->setColor(red);
                } else {
                    o->setColor(transparent);
                }
                // Render the outline
                o->draw();
            }
            /*
            for (size_t i = 0; i < squares.size(); ++i) {
                if (i < 25 / 25) {
                    // Assuming rectState is an array of booleans
                    squares[i]->setColor(rectState[i] ? white : brown);
                } else {
                    // Handle the case where the index is out of bounds of rectState
                    // You may want to set a default color or handle this case in a way that makes sense for your application.
                }
            }
             */

            // Then draw the squares
            for (auto &s : squares) {
                // Set uniforms for the shader
                s->setUniforms();

                // You can check if it is hovered to change the color or add logic here
                s->setColor(yellow); // Assuming yellow is the color you want for squares
                // Render the square
                s->draw(); // This should render the square itself
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
