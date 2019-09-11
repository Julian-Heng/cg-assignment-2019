#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stb_image.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "graphics.h"

#define WIDTH 1440
#define HEIGHT 900
#define TITLE "Game"

static const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
static const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

backend* init()
{
    backend* engine;

    if ((engine = (backend*)malloc(sizeof(backend))))
    {
        memset(engine, 0, sizeof(backend));

        initWindow(engine);
        initGlad(engine);

        if (engine->window)
        {
            initShader(engine);
            initShapes(engine);
        }
    }
    else
    {
        fprintf(stderr, "Cannot allocate memory\n");
    }

    return engine;
}


void initWindow(backend* engine)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if defined(__APPLE__) && defined(__MACH__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    if ((engine->window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL)))
    {
        glfwMakeContextCurrent(engine->window);
        glfwSetKeyCallback(engine->window, input);
        glfwSetFramebufferSizeCallback(engine->window, framebuffer_size_callback);
    }
    else
    {
        fprintf(stderr, "Failed to initialise window\n");
    }
}


void initGlad(backend* engine)
{
    if (engine->window &&
        ! gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialise GLAD\n");
        engine->window = NULL;
    }
}


void initShader(backend* engine)
{
    int success = 0;
    char info[BUFSIZ];
    int vertexShader;
    int fragmentShader;

    memset(info, 0, BUFSIZ);

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (! success)
    {
        glGetShaderInfoLog(vertexShader, BUFSIZ, NULL, info);
        fprintf(stderr, "Error: Shader vertex compilation failed\n%s", info);
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (! success)
    {
        glGetShaderInfoLog(fragmentShader, BUFSIZ, NULL, info);
        fprintf(stderr, "Error: Shader fragment compilation failed\n%s", info);
    }

    engine->shaderProgram = glCreateProgram();
    glAttachShader(engine->shaderProgram, vertexShader);
    glAttachShader(engine->shaderProgram, fragmentShader);
    glLinkProgram(engine->shaderProgram);

    glGetProgramiv(engine->shaderProgram, GL_LINK_STATUS, &success);
    if (! success)
    {
        glGetShaderInfoLog(fragmentShader, BUFSIZ, NULL, info);
        fprintf(stderr, "Error: Shader program linking failed\n%s", info);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


void initShapes(backend* engine)
{
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f  // top left
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &(engine->VAO));
    glGenBuffers(1, &(engine->VBO));
    glGenBuffers(1, &(engine->EBO));

    glBindVertexArray(engine->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, engine->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, engine->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void loop(backend* engine)
{
    while (! glfwWindowShouldClose(engine->window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(engine->shaderProgram);
        glBindVertexArray(engine->VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(engine->window);
        glfwPollEvents();
    }
}


void terminate(backend** engine)
{
    if (*engine)
    {
        free(*engine);
        *engine = NULL;
    }

    glfwTerminate();
}


void input(GLFWwindow* win, int key, int scancode, int action, int mods)
{
    static int wireframeToggle = 0;

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(win, true);
                break;
            case GLFW_KEY_TAB:
                wireframeToggle= (wireframeToggle + 1) % 3;
                glPolygonMode(GL_FRONT_AND_BACK,
                              wireframeToggle == 0 ? GL_FILL :
                              wireframeToggle == 1 ? GL_LINE :
                              GL_POINT);
                break;
            default:
                break;
        }
    }
}


void framebuffer_size_callback(GLFWwindow* win, int width, int height)
{
    glViewport(0, 0, width, height);
}
