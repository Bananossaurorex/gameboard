#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

const char *fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoord);
}
)";

// Funções utilitárias (chamar textura ou crear um programa de shader)
unsigned int loadTexture(const char* path);
unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar GLFW" << std::endl;
        return -1;
    }

    // Configurações do GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criar a janela
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Juizo Final (Alpha)", NULL, NULL);
    if (!window) {
        std::cerr << "Falha ao criar a janela" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Inicializar GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar GLAD" << std::endl;
        return -1;
    }

    // Compilar e criar o programa de shaders
    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Configuração inicial dos buffers
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Índices para os dois triângulos que formam um quadrado
    unsigned int indices[] = {
        0, 1, 3, // primeiro triângulo
        1, 2, 3  // segundo triângulo
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Carregar a textura
    unsigned int texture = loadTexture("img/1.jpg");

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        // Limpa a tela
        glClear(GL_COLOR_BUFFER_BIT);

        // Usa o shaderprogram
        glUseProgram(shaderProgram);

        // Vincula a textura
        glBindTexture(GL_TEXTURE_2D, texture);

        // Desenha as bordas do tabuleiro (10x10 casas, com o centro vazio)
        for (int i = 0; i < 11; i++) {
            for (int j = 0; j < 11; j++) {
                // Ignorar o centro do tabuleiro (evita de ter casas aleatorias)
                if ((i > 0 && i < 10) && (j > 0 && j < 10)) {
                    continue;
                }

                // Translada cada casa para a posição correta na tela
                float offsetX = (j - 5) * 0.2f; // 0.2f é o espaçamento entre as casas
                float offsetY = (i - 5) * 0.2f;

                // Definindo as coordenadas dos vértices para cada célula (casa/quadrado)
                float vertices[] = {
                    // posições         // coordenadas de textura
                    0.1f + offsetX,  0.1f + offsetY, 0.0f, 1.0f, 1.0f, // top right
                    0.1f + offsetX, -0.1f + offsetY, 0.0f, 1.0f, 0.0f, // bottom right
                   -0.1f + offsetX, -0.1f + offsetY, 0.0f, 0.0f, 0.0f, // bottom left
                   -0.1f + offsetX,  0.1f + offsetY, 0.0f, 0.0f, 1.0f  // top left 
                };

                // Atualiza o VBO com os novos vértices
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                // Atributos dos vértices
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);

                // Atributos das coordenadas de textura
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);

                // Desenha a célula (elementos visuais, EX: os quadrados/casas)
                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

        // Trocar os buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpa os recursos
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

// Função para carregar textura
unsigned int loadTexture(const char* path) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Configurações de textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Carrega a imagem da textura
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Falha ao carregar textura: " << path << std::endl;
    }
    stbi_image_free(data);

    return texture;
}

// Função para criar e linkar os shaders
unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
    // Compilar vertexshader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Compila o fragmentshader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Linkar os shaders em um programa
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Excluir os shaders que já estão linkados
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Compilar o código: g++ -g -std=c++17 -I include -L lib main.cpp src/glad.c -lglfw3dll
// Startar o código: .\a.exe