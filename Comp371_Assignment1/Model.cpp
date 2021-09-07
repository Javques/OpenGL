#include "Model.h"
#include <vector>
#include "tools/Shader.h"

//Alexis (can be deleted probs, since cube is already created earlier)

static const int ALEXISOFFSET = 0.5;

cubeList::cubeList(bool shuffle) {
    shuffling = shuffle;
    jamesShuffling = shuffle;
    jayShuffling = shuffle;
    moShuffling = shuffle;
    pabloShuffling = shuffle;
    akhiShuffling = shuffle;
}



//VAOs
unsigned int cubeList::createOriginalCube()
{
    vec3 vertexArray[] = {
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), //left - red
        vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),

        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),

        vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), // far - blue
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),

        vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),

        vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), // bottom - turquoise
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),

        vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),

        vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), // near - green
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), // right - purple
        vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),

        vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), // top - yellow
        vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f)
    };


    // Create a vertex array

    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    glBindVertexArray(0);
    return vertexArrayObject;
}

// cubes with normals, add color/texture to this and remove fixed color in shader
unsigned int cubeList::createVertexNormal()
{
    unsigned int cubeVAO = 0;
    unsigned int cubeVBO = 0;
    float vertices[] = {
        // positions          // normals           // texture coords
           -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
           -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
           -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

           -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
           -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
           -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

           -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
           -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
           -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
           -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
           -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
           -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

           -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
           -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
           -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

           -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
           -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
           -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };



    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    // fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // link vertex attributes
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return cubeVAO;
}

unsigned int cubeList::createLightVertexNormal()
{
    float vertices[] = {
        // positions          // normals           // texture coords
           -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
           -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
           -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

           -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
           -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
           -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

           -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
           -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
           -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
           -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
           -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
           -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

           -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
           -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
           -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

           -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
           -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
           -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    unsigned int VBO;
    glGenBuffers(1, &VBO);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);



    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set the vertex attribute 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    return lightCubeVAO;
}

unsigned int cubeList::createOriginalWall()
{
    vec3 vertexArray[] = {
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 01.0f, 01.0f), //left - red
        vec3(-0.5f,-0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),

        vec3(-0.5f,-0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),

        vec3(0.5f, 0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),

        vec3(0.5f, 0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),

        vec3(0.5f,-0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),

        vec3(0.5f,-0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),

        vec3(-0.5f, 0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(0.5f, 0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),

        vec3(0.5f,-0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(0.5f, 0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(0.5f, 0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(01.0f, 01.0f, 01.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(01.0f, 01.0f, 01.0f)
    };


    // Create a vertex array

    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        2 * sizeof(vec3), // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
        3,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(vec3),
        (void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    glBindVertexArray(0);
    return vertexArrayObject;
}


int cubeList::createColorCube() {

    float cubeCoordinates[] = {
     -0.5f,-0.5f,-0.5f,  // triangle 1 : begin
     -0.5f,-0.5f, 0.5f,
     -0.5f, 0.5f, 0.5f, // triangle 1 : end
     0.5f, 0.5f,-0.5f, // triangle 2 : begin
     -0.5f,-0.5f,-0.5f,
     -0.5f, 0.5f,-0.5f, // triangle 2 : end
     0.5f,-0.5f, 0.5f,
     -0.5f,-0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,
     0.5f, 0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,
     -0.5f,-0.5f,-0.5f,
     -0.5f,-0.5f,-0.5f,
     -0.5f, 0.5f, 0.5f,
     -0.5f, 0.5f,-0.5f,
     0.5f,-0.5f, 0.5f,
     -0.5f,-0.5f, 0.5f,
     -0.5f,-0.5f,-0.5f,
     -0.5f, 0.5f, 0.5f,
     -0.5f,-0.5f, 0.5f,
     0.5f,-0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,
     0.5f,-0.5f,-0.5f,
     0.5f, 0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,
     0.5f, 0.5f, 0.5f,
     0.5f,-0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,
     0.5f, 0.5f,-0.5f,
     -0.5f, 0.5f,-0.5f,
     0.5f, 0.5f, 0.5f,
     -0.5f, 0.5f,-0.5f,
     -0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,
     -0.5f, 0.5f, 0.5f,
     0.5f,-0.5f, 0.5f
    }; // Cube Vertices

    float coordinateColours[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };  //Colour for the Vertices


    // Create a vertex array

    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeCoordinates), cubeCoordinates, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        0, // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);
    GLuint colorBufferObject;
    glGenBuffers(1, &colorBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coordinateColours), coordinateColours, GL_STATIC_DRAW);

    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0    // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    glBindVertexArray(0);
    return vertexArrayObject;
}

int cubeList::createTexturedCube() {
    float cubeVertices[] = {
             -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
              0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
              0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
              0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
             -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

             -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
              0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
              0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
              0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
             -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
             -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

             -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

              0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
              0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
              0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
              0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
              0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
              0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

             -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
              0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
              0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
              0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

             -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
              0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
              0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
              0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
             -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        5 * sizeof(float), // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,                            // attribute 1 matches texture1 in fragment Shader
        2,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)(3 * sizeof(float))
    );



    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBindVertexArray(0);

    return vertexArrayObject;
}

//Alexis
void cubeList::alexisCube(cubeNode** root) {
    struct cubeNode* last = *root;
    vec3 allPos;

    for (int i = 0; i < 8; i++) {
        if (last->next != NULL) {
            last = last->next;
        }
        mat4 currentMat = last->ownMat;
        vec3 currentVec = vecArray[i];
        struct cubeNode* newNode = new cubeNode;
        newNode->ownMat = translate(currentMat, currentVec);
        newNode->next = NULL;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = newNode;

    }
}
void cubeList::updateList(cubeNode** root, mat4 newBase) {

    struct cubeNode* last = *root;
    last->ownMat = newBase;
    struct cubeNode* next = last->next;
    int i = 0;
    while (last->next != NULL) {
        vec3 currentVec;
        mat4 currentMat = last->ownMat;
        if (!shuffling)
            currentVec = vecArray[i];
        else
            currentVec = newVecArray[i];
        next->ownMat = translate(currentMat, currentVec);
        last = last->next;
        next = last->next;
        i++;
    }
}
void cubeList::alexisWall(mat4 originalPos, mat4 wallPos[7]) {
    float walls = 2.5f;
    float thiccness = 0.5f;
    float howFarAway = 9.0f;
    originalPos = translate(originalPos, vec3(0, 0, -howFarAway));
    wallPos[0] = translate(originalPos, vec3(-1, 1, 0));
    wallPos[1] = translate(originalPos, vec3(-1, -1, 0));
    wallPos[2] = translate(originalPos, vec3(1, -1, 0));
    wallPos[3] = translate(originalPos, vec3(walls, 0, 0));
    wallPos[4] = translate(originalPos, vec3(0, walls, 0));
    wallPos[5] = translate(originalPos, vec3(-walls, 0, 0));
    wallPos[6] = translate(originalPos, vec3(0, -walls, 0));


    wallPos[3] = scale(wallPos[3], vec3(2, 3, thiccness));
    wallPos[4] = scale(wallPos[4], vec3(7, 2, thiccness));
    wallPos[5] = scale(wallPos[5], vec3(2, 3, thiccness));
    wallPos[6] = scale(wallPos[6], vec3(7, 2, thiccness));

    wallPos[0] = scale(wallPos[0], vec3(1, 1, thiccness));
    wallPos[1] = scale(wallPos[1], vec3(1, 1, thiccness));
    wallPos[2] = scale(wallPos[2], vec3(1, 1, thiccness));



}
void cubeList::alexisShuffle() {
    //basically only translate on a specific axis by 1 or 2
    for (int i = 0; i < 8; i++) {

        int zTranslateNormal = linearRand(-1, 1);

        newVecArray[i] = vecArray[i];
        newVecArray[i].z = zTranslateNormal;
    }
}
void cubeList::toggleShuffle() {
    shuffling = !shuffling;
}
//Jay
void cubeList::jayCube(mat4 allPos[9], mat4 originalPos) {

    allPos[0] = originalPos;
    if (!jayShuffling) {


        for (int i = 1; i < 9; i++) {
            switch (i) {
            case 1:
                allPos[i] = translate(allPos[i - 1], vecJayArray[i - 1]);
                break;
            case 2:
                allPos[i] = translate(allPos[i - 1], vecJayArray[i - 1]);
                break;
            case 3:
                allPos[i] = translate(allPos[i - 1], vecJayArray[i - 1]);
                break;
            case 4:
                allPos[i] = translate(allPos[i - 1], vecJayArray[i - 1]);
                break;
            case 5:
                allPos[i] = translate(allPos[i - 1], vecJayArray[i - 1]);
                break;
            case 6:
                allPos[i] = translate(allPos[i - 1], vecJayArray[i - 1]);
                break;
            case 7:
                allPos[i] = translate(allPos[i - 1], vecJayArray[i - 1]);
                break;
            case 8:
                allPos[i] = translate(allPos[i - 1], vecJayArray[i - 1]);
                break;
            }
        }
    }
    else
    {
        for (int i = 1; i < 9; i++) {
            switch (i) {
            case 1:
                allPos[i] = translate(allPos[i - 1], vecNewJayArray[i - 1]);
                break;
            case 2:
                allPos[i] = translate(allPos[i - 1], vecNewJayArray[i - 1]);
                break;
            case 3:
                allPos[i] = translate(allPos[i - 1], vecNewJayArray[i - 1]);
                break;
            case 4:
                allPos[i] = translate(allPos[i - 1], vecNewJayArray[i - 1]);
                break;
            case 5:
                allPos[i] = translate(allPos[i - 1], vecNewJayArray[i - 1]);
                break;
            case 6:
                allPos[i] = translate(allPos[i - 1], vecNewJayArray[i - 1]);
                break;
            case 7:
                allPos[i] = translate(allPos[i - 1], vecNewJayArray[i - 1]);
                break;
            case 8:
                allPos[i] = translate(allPos[i - 1], vecNewJayArray[i - 1]);
                break;
            }
        }
    }
}
void cubeList::jayWall(mat4 originalPos, mat4 wallPos[7]) {
    float walls = 2.5f;
    float thiccness = 0.5f;
    float howFarAway = 9.0f;
    originalPos = translate(originalPos, vec3(0, 1, -howFarAway));
    wallPos[0] = translate(originalPos, vec3(1, 1, 0));
    wallPos[1] = translate(originalPos, vec3(-1, -1, 0));
    wallPos[2] = translate(originalPos, vec3(1, 0, 0));
    wallPos[0] = scale(wallPos[0], vec3(1, 1, thiccness));
    wallPos[1] = scale(wallPos[1], vec3(1, 1, thiccness));
    wallPos[2] = scale(wallPos[2], vec3(1, 1, thiccness));
    wallPos[3] = translate(originalPos, vec3(walls, 0, 0));
    wallPos[4] = translate(originalPos, vec3(0, walls, 0));
    wallPos[5] = translate(originalPos, vec3(-walls, 0, 0));
    wallPos[6] = translate(originalPos, vec3(0, -walls, 0));
    wallPos[3] = scale(wallPos[3], vec3(2, 3, thiccness));
    wallPos[4] = scale(wallPos[4], vec3(7, 2, thiccness));
    wallPos[5] = scale(wallPos[5], vec3(2, 3, thiccness));
    wallPos[6] = scale(wallPos[6], vec3(7, 2, thiccness));
}
void cubeList::toggleShuffleJay() {
    jayShuffling = !jayShuffling;
}
void cubeList::jayShuffle() {
    for (int i = 0; i < 8; i++) {

        int zTranslateNormal = linearRand(-1, 1);

        vecNewJayArray[i] = vecJayArray[i];
        vecNewJayArray[i].z = zTranslateNormal;
    }
}
//james
void cubeList::jamesCube(cubeNode** root) {
    struct cubeNode* last = *root;
    vec3 allPos;

    for (int i = 0; i < 8; i++) {
        if (last->next != NULL) {
            last = last->next;
        }
        mat4 currentMat = last->ownMat;
        vec3 currentVec = jamesVecArray[i];
        struct cubeNode* newNode = new cubeNode;
        newNode->ownMat = translate(currentMat, currentVec);
        newNode->next = NULL;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = newNode;
    }
}
void cubeList::jamesUpdateList(cubeNode** root, mat4 newBase) {
    struct cubeNode* last = *root;
    last->ownMat = newBase;
    struct cubeNode* next = last->next;
    int i = 0;
    while (last->next != NULL) {
        vec3 currentVec;
        mat4 currentMat = last->ownMat;
        if (!jamesShuffling)
            currentVec = jamesVecArray[i];
        else
            currentVec = jamesNewVecArray[i];
        next->ownMat = translate(currentMat, currentVec);
        last = last->next;
        next = last->next;
        i++;
    }
}
void cubeList::toggleShuffleJames() {
    jamesShuffling = !jamesShuffling;
}
void cubeList::jamesWall(mat4 originalPos, mat4 wallPos[22]) {
    float walls = 2.5f;
    float thiccness = 0.5f;
    float howFarAway = 9.0f;
    originalPos = translate(originalPos, vec3(0, 0, -howFarAway));

    wallPos[0] = translate(originalPos, vec3(0, 1, 0));
    wallPos[1] = translate(originalPos, vec3(0, 2, 0));
    wallPos[2] = translate(originalPos, vec3(0, 3, 0));
    wallPos[3] = translate(originalPos, vec3(1, 3, 0));
    wallPos[4] = translate(originalPos, vec3(2, 3, 0));
    wallPos[5] = translate(originalPos, vec3(3, 3, 0));
    wallPos[6] = translate(originalPos, vec3(4, 3, 0));
    wallPos[7] = translate(originalPos, vec3(4, 2, 0));
    wallPos[8] = translate(originalPos, vec3(4, 1, 0));
    wallPos[9] = translate(originalPos, vec3(4, 0, 0));
    wallPos[10] = translate(originalPos, vec3(3, 0, 0));
    wallPos[11] = translate(originalPos, vec3(2, 0, 0));
    wallPos[12] = translate(originalPos, vec3(2, -1, 0));
    wallPos[13] = translate(originalPos, vec3(1, -1, 0));
    wallPos[14] = translate(originalPos, vec3(0, -1, 0));
    wallPos[15] = translate(originalPos, vec3(-1, -1, 0));
    wallPos[16] = translate(originalPos, vec3(-1, 0, 0));
    wallPos[17] = translate(originalPos, vec3(-1, 1, 0));
    wallPos[18] = translate(originalPos, vec3(3, -1, 0));
    wallPos[19] = translate(originalPos, vec3(4, -1, 0));
    wallPos[20] = translate(originalPos, vec3(-1, 2, 0));
    wallPos[21] = translate(originalPos, vec3(-1, 3, 0));

}
void cubeList::jamesShuffle() {
    //basically only translate on a specific axis by 1 or 2
    for (int i = 0; i < 8; i++) {

        int zTranslateNormal = linearRand(-1, 1);

        jamesNewVecArray[i] = jamesVecArray[i];
        jamesNewVecArray[i].z = zTranslateNormal;
    }
}
//Mohamed
void cubeList::moCube(mat4 allPos[9], mat4 originalPos) {

    allPos[0] = originalPos;
    if (!moShuffling) {


        for (int i = 1; i < 9; i++) {
            switch (i) {
            case 1:
                allPos[i] = translate(allPos[i - 1], vecMoArray[i - 1]);
                break;
            case 2:
                allPos[i] = translate(allPos[i - 1], vecMoArray[i - 1]);
                break;
            case 3:
                allPos[i] = translate(allPos[i - 1], vecMoArray[i - 1]);
                break;
            case 4:
                allPos[i] = translate(allPos[i - 1], vecMoArray[i - 1]);
                break;
            case 5:
                allPos[i] = translate(allPos[i - 1], vecMoArray[i - 1]);
                break;
            case 6:
                allPos[i] = translate(allPos[i - 1], vecMoArray[i - 1]);
                break;
            case 7:
                allPos[i] = translate(allPos[i - 1], vecMoArray[i - 1]);
                break;
            case 8:
                allPos[i] = translate(allPos[i - 1], vecMoArray[i - 1]);
                break;
            }
        }
    }
    else
    {
        for (int i = 1; i < 9; i++) {
            switch (i) {
            case 1:
                allPos[i] = translate(allPos[i - 1], vecNewMoArray[i - 1]);
                break;
            case 2:
                allPos[i] = translate(allPos[i - 1], vecNewMoArray[i - 1]);
                break;
            case 3:
                allPos[i] = translate(allPos[i - 1], vecNewMoArray[i - 1]);
                break;
            case 4:
                allPos[i] = translate(allPos[i - 1], vecNewMoArray[i - 1]);
                break;
            case 5:
                allPos[i] = translate(allPos[i - 1], vecNewMoArray[i - 1]);
                break;
            case 6:
                allPos[i] = translate(allPos[i - 1], vecNewMoArray[i - 1]);
                break;
            case 7:
                allPos[i] = translate(allPos[i - 1], vecNewMoArray[i - 1]);
                break;
            case 8:
                allPos[i] = translate(allPos[i - 1], vecNewMoArray[i - 1]);
                break;
            }
        }
    }
}
void cubeList::moWall(mat4 originalPos, mat4 wallPos[7]) {
    float walls = 2.5f;
    float thiccness = 0.5f;
    float howFarAway = 9.0f;
    originalPos = translate(originalPos, vec3(-1, 1, -howFarAway));
    wallPos[0] = translate(originalPos, vec3(1, 1, 0));
    wallPos[1] = translate(originalPos, vec3(-1, -1, 0));
    wallPos[2] = translate(originalPos, vec3(1, 0, 0));
    wallPos[0] = scale(wallPos[0], vec3(1, 1, thiccness));
    wallPos[1] = scale(wallPos[1], vec3(1, 1, thiccness));
    wallPos[2] = scale(wallPos[2], vec3(1, 1, thiccness));
    wallPos[3] = translate(originalPos, vec3(walls, 0, 0));
    wallPos[4] = translate(originalPos, vec3(0, walls, 0));
    wallPos[5] = translate(originalPos, vec3(-walls, 0, 0));
    wallPos[6] = translate(originalPos, vec3(0, -walls, 0));
    wallPos[3] = scale(wallPos[3], vec3(2, 3, thiccness));
    wallPos[4] = scale(wallPos[4], vec3(7, 2, thiccness));
    wallPos[5] = scale(wallPos[5], vec3(2, 3, thiccness));
    wallPos[6] = scale(wallPos[6], vec3(7, 2, thiccness));
}
void cubeList::moShuffle() {
    for (int i = 0; i < 8; i++) {

        int zTranslateNormal = linearRand(-1, 1);

        vecNewMoArray[i] = vecMoArray[i];
        vecNewMoArray[i].z = zTranslateNormal;
    }
}
void cubeList::toggleShuffleMo() {
    moShuffling = !moShuffling;
}

void cubeList::drawAkhiCube(mat4 model_Matrix, Shader shaderProgram, int drawMode, std::string matName)
{
    vec3 vecUsed;
    for (int i = -1; i < 8; i++) {

        if (!akhiShuffling) {
            vecUsed = cubePositionsAkhi[i];
        }
        else
        {
            vecUsed = cubePositionsAkhiShuffled[i];
        }
        model_Matrix = glm::translate(model_Matrix, vecUsed);

        shaderProgram.setMat4(matName.c_str(), (model_Matrix));

        glDrawArrays(drawMode, 0, 6 * 2 * 3);
    }
}

void cubeList::akhiToggleShuffle()
{
    akhiShuffling = !akhiShuffling;
}

void cubeList::akhiShuffle() {
    std::vector<glm::vec3> takenCoordinates;
    glm::vec3 objectCoordinate = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 newCoordinate;
    bool noCollision;
    cubePositionsAkhiShuffled[0] = cubePositionsAkhi[0];
    for (int i = 1; i < 9; i++) {
        noCollision = false;

        while (!noCollision)
        {
            int zTranslateNormal = linearRand(-1, 1);
            newCoordinate = cubePositionsAkhi[i];
            newCoordinate.z = zTranslateNormal;

            if (!(std::find(takenCoordinates.begin(), takenCoordinates.end(), (newCoordinate + objectCoordinate)) != takenCoordinates.end()))
            {
                noCollision = true;
                takenCoordinates.push_back(objectCoordinate + newCoordinate);
                objectCoordinate += newCoordinate;
                cubePositionsAkhiShuffled[i] = newCoordinate;

            }
        }
    }
}

void cubeList::drawPabloCube(mat4 model_Matrix, Shader shaderProgram, int drawMode, std::string matName)
{

    vec3 vecUsed;
    for (int i = 0; i < 9; i++) {



        if (!pabloShuffling) {
            vecUsed = cubePositionsPablo[i];
        }
        else
            vecUsed = cubePositionsPabloShuffled[i];
        model_Matrix = glm::translate(model_Matrix, vecUsed);
        shaderProgram.setMat4(matName.c_str(), (model_Matrix));
        glDrawArrays(drawMode, 0, 6 * 2 * 3);
    }



}
void cubeList::pabloShuffle() {

    std::vector<glm::vec3> takenCoordinates;
    glm::vec3 objectCoordinate = glm::vec3(0.0f, 0.0f, 0.0f);
    takenCoordinates.push_back(objectCoordinate);
    glm::vec3 newCoordinate;
    bool noCollision;
    cubePositionsPabloShuffled[0] = cubePositionsPablo[0];

    for (int i = 1; i < 9; i++) {
        noCollision = false;

        while (!noCollision)
        {
            int zTranslateNormal = linearRand(-1, 1);
            newCoordinate = cubePositionsPablo[i];
            newCoordinate.z = zTranslateNormal;

            if (!(std::find(takenCoordinates.begin(), takenCoordinates.end(), (newCoordinate + objectCoordinate)) != takenCoordinates.end()))
            {
                noCollision = true;
                takenCoordinates.push_back(objectCoordinate + newCoordinate);
                objectCoordinate += newCoordinate;
                //newPositionsPablo[i] = newCoordinate;

            }
        }
    }

    //Check for disjointed cubes
    glm::vec3 coord = takenCoordinates[0];
    bool first = true;
    int rand = linearRand(0, 1);
    float x, y, z;
    for (int i = 1; i < 9; i++) {
        x = coord.x;
        y = coord.y;
        z = coord.z;

        //Disjointed from previous cube
        if (takenCoordinates[i].z < z - 1 || takenCoordinates[i].z > z + 1 || takenCoordinates[i].x < x - 1 || takenCoordinates[i].x > x + 1 || takenCoordinates[i].y < y - 1 || takenCoordinates[i].y > y + 1)
        {
            if (rand == 1) {
                takenCoordinates[i].z = z + 1;
                takenCoordinates[i].x = x;
                takenCoordinates[i].y = y;
            }
            else if (first) {
                takenCoordinates[i].z = takenCoordinates[2].z;
                takenCoordinates[i].x = takenCoordinates[2].x - 1;
                takenCoordinates[i].y = takenCoordinates[2].y;
                first = false;
            }
            else {
                takenCoordinates[i].z = z + 1;
                takenCoordinates[i].x = x;
                takenCoordinates[i].y = y;
            }



        }

        coord = takenCoordinates[i];
    }

    //Form the new positions
    cubePositionsPabloShuffled[0] = takenCoordinates[0];
    for (int i = 1; i < 9; i++) {
        cubePositionsPabloShuffled[i] = takenCoordinates[i] - takenCoordinates[i - 1];
    }
}

void cubeList::pabloToggleShuffle() {
    pabloShuffling = !pabloShuffling;
}


void cubeList::drawAkhiOrPabloWall(mat4 model_Matrix, Shader shader, int drawMode, std::string matName, vec3 wallPositions[], int size) {


    glm::mat4 view_Matrix = glm::mat4(1.0f);
    glm::mat4 projection_Matrix = glm::mat4(1.0f);
    glm::mat4 MVP_MATRIX;
    for (int i = 0; i < size; i++) {
        model_Matrix = glm::translate(model_Matrix, wallPositions[i]);
        //model_Matrix = glm::rotate(model_Matrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //view_Matrix = glm::translate(view_Matrix, glm::vec3(0.0, 0.0, -5.0f));

        MVP_MATRIX = projection_Matrix * view_Matrix * model_Matrix;

        unsigned int MVP;

        shader.setMat4(matName.c_str(), MVP_MATRIX);
        glDrawArrays(drawMode, 0, 36);


    }
}