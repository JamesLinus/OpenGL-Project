#include "Canvas.h"
#include <math.h>
#include <deque>

#include <glm/glm.hpp>

using namespace glm;

#include <glm/gtc/matrix_transform.hpp>

#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))

unsigned int width = 512;
unsigned int height = 512;

vec3 v0(0,0,1);
vec3 v1(1,0,0);
vec3 v2(0,1,0);
vec3 v3(-1,0,0);
vec3 v4(0,-1,0);
vec3 v5(0,0,-1);

int recursiondepth = 5;

float sphereHold[600000] = {0};
std::deque<float> verthold;

Canvas canvas;

/*
 * Four steps:
 *  - Geometry
 *  - VBO - VAO  (buffer array)
 *  - Shaders
 *  - Drawing
 *
 *
 * */

const GLfloat spherepoints[] = {
    //S0
    1,0,0,
    0,0,-1,
    0,1,0,
    //S1
    0,1,0,
    0,0,-1,
    -1,0,0,
    //S2
    -1,0,0,
    0,0,-1,
    0,-1,0,
    //S3
    0,-1,0,
    0,0,-1,
    1,0,0,
    //N0
    1,0,0,
    0,0,1,
    0,-1,0,
    //N1
    0,-1,0,
    0,0,1,
    -1,0,0,
    //N2
    -1,0,0,
    0,0,1,
    0,1,0,
    //N3
    0,1,0,
    0,0,1,
    1,0,0

};

void vertpush(vec3 in){
    verthold.push_back(in.x);
    verthold.push_back(in.y);
    verthold.push_back(in.z);
}

void subdivide(int depth, int currsize){
    if( depth == 0 ) return;
    for(int i = 0; i < currsize; i+=9){
        vec3 v0(verthold[0],verthold[1],verthold[2]);
        vec3 v1(verthold[3],verthold[4],verthold[5]);
        vec3 v2(verthold[6],verthold[7],verthold[8]);
        vec3 w0 = normalize(v1 + v2);
        vec3 w1 = normalize(v0 + v2);
        vec3 w2 = normalize(v0 + v1);

        verthold.pop_front();
        verthold.pop_front();
        verthold.pop_front();
        verthold.pop_front();
        verthold.pop_front();
        verthold.pop_front();
        verthold.pop_front();
        verthold.pop_front();
        verthold.pop_front();

        vertpush(v0);
        vertpush(w2);
        vertpush(w1);
        vertpush(v1);
        vertpush(w0);
        vertpush(w2);
        vertpush(v2);
        vertpush(w1);
        vertpush(w0);
        vertpush(w0);
        vertpush(w1);
        vertpush(w2);

    }
    currsize = verthold.size();
    subdivide(depth-1, currsize);
}

//Shaders    Vertex shader and fragment shaders
const char * vshader_square = "\
#version 330 core \n \
in vec3 vpoint;\
uniform mat4 trans;\
uniform float rotation;\
uniform mat4 MVP;\
out vec3 n;\
out vec3 v;\
\
void main(){ \
    v=vpoint;\
    vec3 origin = vec3(0,0,0);\
    n = -normalize(v-origin);\
    gl_Position = MVP * vec4(vpoint, 1) * trans; \
}";

const char * fshader_square = "\
#version 330 core \n \
in vec3 v;\
in vec3 n;\
in vec3 e;\
uniform vec3 eye;\
uniform vec3 Light;\
out vec3 color; \
void main(){\
    vec3 diffuse = vec3(.7,.4,.2); \
    vec3 ambient = diffuse * 0.1;\
    vec3 specular = vec3(.4,.4,.4);\
    vec3 L = normalize(Light - v);\
    float ndotl = dot(n, L);\
    if(ndotl > 0){\
        vec3 eyedir = normalize(eye - v);\
        vec3 h = normalize(L + eyedir);\
        float hdotn = dot(h, n);\
        float specular_term = pow(hdotn, 36);\
        color = ambient + diffuse * ndotl + specular * specular_term;\
     }else{\
        color = ambient;\
     }\
}";

float Rotation = 0;
float RotationSpeed = 0.02;

GLuint VertexArrayID = 0;

GLuint ProgramID = 0;

GLuint RotationBindingID = 0;
GLuint MVPBindingID = 0;
GLuint LightBindingID = 0;
GLuint EyeBindingID = 0;
GLuint TransBindingID = 0;

mat4x4 MVP;
mat4x4 Trans;
mat4x4 SecondCube;
float Light[3];
float Eye[3];

void InitializeMVP(){
    mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
    mat4 camera = glm::lookAt(vec3(0,0,3), vec3(0,0,0), vec3(0,1,0));
    mat4 model = mat4(1.0f);
    MVP = projection * camera * model;
}

float vppos_x = 0;
float vppos_y = 0;
float vppos_x_old = 0;
float vppos_y_old = 0;
float started_pos_x = 0;
float started_pos_y = 0;

float zoom = 4;
bool leftButtonPressed = false;
bool rightButtonPressed = false;

float x_turn = 0;
float y_turn = 0;

void updateMVP(){

    Eye[0] = sin(x_turn)*3;
    Eye[1] = sin(y_turn);
    Eye[2] = cos(x_turn)*3;

    vec3 e(sin(x_turn)*cos(y_turn),sin(y_turn),cos(x_turn)*cos(y_turn));
    vec3 up(0,cos(y_turn),0);

    mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, .1f, 50.0f);

    mat4 camera = glm::lookAt(e, vec3(0,0,0), up);

    mat4 model = mat4(1.0f);
    model[3][3] *= zoom;

    MVP = projection * camera * model;
}

void InitializeSecondCube(){
    vec3 e(0,0,-3);
    vec3 up(0,1,0);

    mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, .1f, 50.0f);

    mat4 camera = glm::lookAt(e, vec3(0,0,0), up);

    mat4 model = mat4(1.0f);
    Trans = mat4(1.0f);

    model[3][3] *= 2.0f;

    SecondCube = projection * camera * model;
}

void InitializeGL()
{
    //Vertex Array Object
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    //Vertex Buffer Object
    GLuint vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(sphereHold),
                 sphereHold,
                 GL_STATIC_DRAW);

    //Shaders
    ProgramID = compile_shaders(vshader_square, fshader_square);
    glUseProgram(ProgramID);
    GLuint vpoint_id = glGetAttribLocation(ProgramID, "vpoint");
    glEnableVertexAttribArray(vpoint_id);
    glVertexAttribPointer(vpoint_id,
                          3,
                          GL_FLOAT,
                          false,
                          0,
                          0);

    RotationBindingID = glGetUniformLocation(ProgramID, "rotation");
    MVPBindingID = glGetUniformLocation(ProgramID, "MVP");
    TransBindingID = glGetUniformLocation(ProgramID, "trans");
    LightBindingID = glGetUniformLocation(ProgramID, "light");
    EyeBindingID = glGetUniformLocation(ProgramID, "eye");

    glEnable(GL_DEPTH_TEST);

}

void MouseMove(double x, double y)
{
    //the pointer has moved
       vppos_x = (float)(x) / 256 - 1;
       vppos_y = 1 - (float)(y) / 256;
}

bool mouseDown = false;

void MouseButton(MouseButtons mouseButton, bool press)
{
    //What to do with the keys?
        if (mouseButton == LeftButton)
        {
            if (press == true){ leftButtonPressed = true;
                if(!mouseDown){
                    started_pos_x = vppos_x;
                    started_pos_y = vppos_y;
                    vppos_x_old = vppos_x;
                    vppos_y_old = vppos_y;
                    mouseDown = true;
                }
            }
            else{
                leftButtonPressed = false;
                mouseDown = false;
            }
        }

        if(mouseButton == RightButton){
            if (press == true) rightButtonPressed = true;
            else rightButtonPressed = false;
        }
}

void KeyPress(char keychar)
{

}

void updateTrans(float in){
    Trans[0][2] = 3;
    Trans[2][3] = 0;
    Trans[0][0] = cos(in);
    Trans[0][2] = sin(in);
    Trans[2][0] = -sin(in);
    Trans[2][2] = cos(in);
}

void OnPaint()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Context
    glUseProgram(ProgramID);
    glBindVertexArray(VertexArrayID);

    //Draw
    glUniform1f(RotationBindingID, Rotation);
    updateMVP();
    glUniformMatrix4fv(MVPBindingID, 1, GL_FALSE, &MVP[0][0] );
    Trans = mat4(1.0f);
    glUniformMatrix4fv(TransBindingID, 1, GL_FALSE, &Trans[0][0] );
    glUniform3fv(EyeBindingID, 1, Eye);
    glUniform3fv(LightBindingID, 1, Light);

    //Draw Sphere
    glDrawArrays(GL_TRIANGLES, 0, verthold.size());

    //Draw Second Sphere
    updateTrans(Rotation);

    glUniformMatrix4fv(TransBindingID, 1, GL_FALSE, &Trans[0][0] );
    glUniformMatrix4fv(MVPBindingID, 1, GL_FALSE, &SecondCube[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, verthold.size());

    //Clean Up
    glUseProgram(0);
    glBindVertexArray(0);
}

void OnTimer()
{
    Rotation += RotationSpeed;
    if(leftButtonPressed){
            if(vppos_x > vppos_x_old){
                x_turn -= .05;
                vppos_x_old = vppos_x;
            }else if(vppos_x < vppos_x_old){
                x_turn += .05;
                vppos_x_old = vppos_x;
            }else{}

            if(vppos_y > vppos_y_old){
                y_turn -= 0.05;
                vppos_y = vppos_y_old;
            }else if(vppos_y < vppos_y_old){
                y_turn += 0.05;
                vppos_y = vppos_y_old;
            }else{}
        }

        if(rightButtonPressed){
            if(vppos_y > vppos_y_old){
                zoom -= .05;
            }else if(vppos_y < vppos_y_old){
                zoom += .05;
            }else{}
        }
}

void InitializeLight(){
    Light[0] = 1;
    Light[1] = 1;
    Light[2] = 5;
    Eye[0] = 0;
    Eye[1] = 0;
    Eye[2] = 3;
}

void InitializeQueue(){
    for(int i = 0; i < ARRAY_SIZE(spherepoints); i++){
        verthold.push_back(spherepoints[i]);
    }
}

void createVpoint(){
    for(int i = 0; i < verthold.size(); i ++){
        sphereHold[i] = verthold[i];
    }
}

int main(int, char **){

    //Link the call backs
    canvas.SetMouseMove(MouseMove);
    canvas.SetMouseButton(MouseButton);
    canvas.SetKeyPress(KeyPress);
    canvas.SetOnPaint(OnPaint);
    canvas.SetTimer(0.05, OnTimer);
    //Show Window
    canvas.Initialize(width, height, "TysonBulmer");
    //Do our initialization
    InitializeQueue();
    subdivide(recursiondepth,ARRAY_SIZE(spherepoints));
    createVpoint();
    InitializeMVP();
    InitializeSecondCube();
    InitializeLight();
    InitializeGL();
    canvas.Show();

    return 0;
}

