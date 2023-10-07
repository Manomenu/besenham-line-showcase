#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <cstdlib>
#include <iostream>
#include <my/Shader.h>
#include <vector>
#include <math.h>

using namespace std;

template<typename T = int>
struct point
{
    T x = -1, y = -1;

    void reset()
    {
        x = y = -1;
    }
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);
void updateLine(vector<point<float>>& lineVertices, unsigned int lineVAO, unsigned int lineVBO);
void drawLine(const size_t lineVerticesCount, unsigned int lineVAO);
void updatePoint(point<float>& pointVertex, unsigned int pointVAO, unsigned int pointVBO);
void drawPoint(unsigned int pointVAO);

// data
point p1, p2;

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// flags
bool shouldUpdateLine = false;
bool shouldUpdatePoint = false;
bool isLineEstabilished = false;

int main()
{
    cout << "1. Kliknij w dwa rozne miejsce, by narysowac linie.\n";
    cout << "2. Nacisnij q by zresetowac scene.\n";
    cout << "3. Nacisnij escape by zakonczyc program.";

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bresenham line algorithm", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD\n";
        return EXIT_FAILURE;
    }

    // global config
    // -------------
    glPointSize(5);

    Shader defaultShader("./shaders/default.vs", "./shaders/default.fs");

    point<float> pointVertex;
    unsigned int pointVBO, pointVAO;
    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);
    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(point<float>), (void*)0);
    glBindVertexArray(0);

    vector<point<float>> lineVertices;
    unsigned int lineVBO, lineVAO;
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(point<float>), (void*)0);
    glBindVertexArray(0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        // niepotrzebne na razie

        // input
        // -----
        processInput(window);
        if (shouldUpdateLine)
        {
            shouldUpdateLine = !shouldUpdateLine;
            updateLine(lineVertices, lineVAO, lineVBO);
        }
        if (shouldUpdatePoint)
        {
            shouldUpdatePoint = !shouldUpdatePoint;
            updatePoint(pointVertex, pointVAO, pointVAO);
        }

        // draw config
        // -----------
        glClearColor(0.2f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        defaultShader.use();
        
        // draw
        // ----
        if (p2.x != -1)
            drawLine(lineVertices.size(), lineVAO);
        else if (p1.x != -1)
            drawPoint(pointVAO);
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &pointVAO);
    glDeleteBuffers(1, &pointVBO);
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &lineVBO);

	return EXIT_SUCCESS;
}

void updateLine(vector<point<float>>& lineVertices, unsigned int lineVAO, unsigned int lineVBO)
{
    // do sth with equal x1 x2!

    int y1 = SCR_HEIGHT - p1.y, y2 = SCR_HEIGHT - p2.y;
    int x1 = p1.x, x2 = p2.x;
    int stepX = 1, stepY;
    float delta;
    float dx = float(x2 - x1);
    float dy = float(y2 - y1);
    float error = 0.5f;
    vector<point<int>> line;
    int len;
    
    if (x2 > x1)
    {
        if (y2 > y1)
        {
            stepY = 1;
            delta = dy / dx;
        }
        else
        {
            delta = -dy / dx;
            stepY = -1;
        }
    }
    else
    {
        if (y2 > y1)
        {
            swap(x1, x2);
            swap(y1, y2);
            delta = -dy / dx;
            stepY = -1;
        }
        else
        {
            swap(x1, x2);
            swap(y1, y2);
            delta = dy / dx;
            stepY = 1;
        }
    }

    if (abs(dx) > abs(dy))
    {
        len = abs(x2 - x1);
        line.resize(len);
        line[0] = { x1, y1 };
        for (int i = 1; i < len; ++i)
        {
            line[i].x = line[i - 1].x + stepX;
            error += delta;
            if (error > 0.5f)
            {
                error -= 1.0f;
                line[i].y = line[i - 1].y + stepY;
            }
            else
                line[i].y = line[i - 1].y;
        }
    }
    else
    {
        delta = 1.0f / delta;
        len = abs(y2 - y1);
        line.resize(len);
        line[0] = { x1, y1 };
        for (int i = 1; i < len; ++i)
        {
            line[i].y = line[i - 1].y + stepY;
            error += delta;
            if (error > 0.5f)
            {
                error -= 1.0f;
                line[i].x = line[i - 1].x + stepX;
            }
            else
                line[i].x = line[i - 1].x;
        }
    }

    lineVertices.clear();
    for (int i = 0; i < len; ++i)
        lineVertices.push_back(point(2.0f * line[i].x / (float)SCR_WIDTH - 1.0f, 2.0f * line[i].y / (float)SCR_HEIGHT - 1.0f));

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point<float>) * lineVertices.size(), &lineVertices[0], GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void drawLine(const size_t lineVerticesCount, unsigned int lineVAO)
{
    glBindVertexArray(lineVAO);
    glDrawArrays(GL_POINTS, 0, lineVerticesCount);
    glBindVertexArray(0);
}

void updatePoint(point<float>& pointVertex, unsigned int pointVAO, unsigned int pointVBO)
{
    pointVertex.x = 2.0f * p1.x / (float)SCR_WIDTH - 1.0f;
    pointVertex.y = 1.0f - 2.0f * p1.y / (float)SCR_HEIGHT;

    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointVertex), &pointVertex, GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void drawPoint(unsigned int pointVAO)
{
    glBindVertexArray(pointVAO);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    // tutaj moge zrobic following za kursorem dla drugiego punktu, ale to i tak bedzie do zmiany w aplikacji
    if (p1.x == -1 || isLineEstabilished)
        return;
    if (xposIn != p1.x || yposIn != p1.y)
    {
        p2.x = xposIn;
        p2.y = yposIn;
        shouldUpdateLine = !shouldUpdateLine;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double x, y;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        //getting cursor position
        glfwGetCursorPos(window, &x, &y);
        if (p1.x == -1)
        {
            p1.x = x;
            p1.y = y;
            shouldUpdatePoint = !shouldUpdatePoint;
        }
        else if (x != p1.x || y != p1.y)
        {
            p2.x = x;
            p2.y = y;
            shouldUpdateLine = !shouldUpdateLine;
            isLineEstabilished = true;
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        p1.reset();
        p2.reset();
        isLineEstabilished = false;
    }
}