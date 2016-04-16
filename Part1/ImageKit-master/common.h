#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <iostream>

using namespace Eigen;
using namespace std;

//Global Variables
float vppos_x = 0;
float vppos_y = 0;
float vppos_x_old = 0;
float vppos_y_old = 0;
float started_pos_x = 0;
float started_pos_y = 0;

float x_turn = 0;
float y_turn = 0;

float zoom = 1;
bool leftButtonPressed = false;
bool rightButtonPressed = false;
float linelength = 0;
int timercount = 0;
Canvas canvas;

float r = 1;
float l = -1;
float t = 1;
float b = -1;
float n = -1;
float f = -5;

Matrix4d morth;
Matrix4d mpr;
Matrix4d mprorth;

std::vector<vector<Vector3d> > Faces;

std::vector<Vector3d> Face1Verticies;
std::vector<Vector3d> Face2Verticies;
std::vector<Vector3d> Face3Verticies;
std::vector<Vector3d> Face4Verticies;
std::vector<Vector3d> Face5Verticies;
std::vector<Vector3d> Face6Verticies;

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
    //A key is pressed! print a message
    fprintf(stderr, "The \"%c\" key is pressed!\n", keychar);
}

void OnTimer()
{
    if(leftButtonPressed){
        if(vppos_x > vppos_x_old){
            x_turn += .05;
            vppos_x_old = vppos_x;
        }else if(vppos_x < vppos_x_old){
            x_turn -= .05;
            vppos_x_old = vppos_x;
        }else{}

        if(vppos_y > vppos_y_old){
            y_turn += .05;
            vppos_y = vppos_y_old;
        }else if(vppos_y < vppos_y_old){
            y_turn -= .05;
            vppos_y = vppos_y_old;
        }else{}
    }

    if(rightButtonPressed){
        if(vppos_y > vppos_y_old){
            zoom += .05;
        }else if(vppos_y < vppos_y_old){
            zoom -= .05;
        }else{}
    }

}

void linkCallBacks(){
    //Link the call backs
    canvas.SetMouseMove(MouseMove);
    canvas.SetMouseButton(MouseButton);
    canvas.SetKeyPress(KeyPress);
    canvas.SetTimer(0.03, OnTimer);//trigger OnTimer every 0.1 second
}

void initMatrices(){
    morth(0,0) = 2/(r-l);
    morth(0,1) = 0;
    morth(0,2) = 0;
    morth(0,3) = -((r+l)/(r-l));

    morth(1,0) = 0;
    morth(1,1) = 2/(t-b);
    morth(1,2) = 0;
    morth(1,3) = -((t+b)/(t-b));

    morth(2,0) = 0;
    morth(2,1) = 0;
    morth(2,2) = 2/(n-f);
    morth(2,3) = -((n+f)/(n-f));

    morth(3,0) = 0;
    morth(3,1) = 0;
    morth(3,2) = 0;
    morth(3,3) = 1;

    mpr(0,0) = 1;
    mpr(0,1) = 0;
    mpr(0,2) = 0;
    mpr(0,3) = 0;

    mpr(1,0) = 0;
    mpr(1,1) = 1;
    mpr(1,2) = 0;
    mpr(1,3) = 0;

    mpr(2,0) = 0;
    mpr(2,1) = 0;
    mpr(2,2) = (n+f)/n;
    mpr(2,3) = (-f);

    mpr(3,0) = 0;
    mpr(3,1) = 0;
    mpr(3,2) = 1/n;
    mpr(3,3) = 0;

    mprorth = mpr*morth;
}

void initCube(){
    //Create Cube faces
    //Front
    Face1Verticies.push_back(Vector3d(0.5,-0.5,0.5));
    Face1Verticies.push_back(Vector3d(-0.5,-0.5,0.5));
    Face1Verticies.push_back(Vector3d(-0.5,0.5,0.5));
    Face1Verticies.push_back(Vector3d(0.5,0.5,0.5));

    //Back
    Face2Verticies.push_back(Vector3d(0.5,-0.5,-0.5));
    Face2Verticies.push_back(Vector3d(-0.5,-0.5,-0.5));
    Face2Verticies.push_back(Vector3d(-0.5,0.5,-0.5));
    Face2Verticies.push_back(Vector3d(0.5,0.5,-0.5));

    //Right
    Face3Verticies.push_back(Vector3d(0.5,-0.5,-0.5));
    Face3Verticies.push_back(Vector3d(0.5,-0.5,0.5));
    Face3Verticies.push_back(Vector3d(0.5,0.5,0.5));
    Face3Verticies.push_back(Vector3d(0.5,0.5,-0.5));

    //Left
    Face4Verticies.push_back(Vector3d(-0.5,-0.5,-0.5));
    Face4Verticies.push_back(Vector3d(-0.5,-0.5,0.5));
    Face4Verticies.push_back(Vector3d(-0.5,0.5,0.5));
    Face4Verticies.push_back(Vector3d(-0.5,0.5,-0.5));

    //Top
    Face5Verticies.push_back(Vector3d(0.5,0.5,-0.5));
    Face5Verticies.push_back(Vector3d(0.5,0.5,0.5));
    Face5Verticies.push_back(Vector3d(-0.5,0.5,0.5));
    Face5Verticies.push_back(Vector3d(-0.5,0.5,-0.5));

    //Bottom
    Face6Verticies.push_back(Vector3d(0.5,-0.5,-0.5));
    Face6Verticies.push_back(Vector3d(0.5,-0.5,0.5));
    Face6Verticies.push_back(Vector3d(-0.5,-0.5,0.5));
    Face6Verticies.push_back(Vector3d(-0.5,-0.5,-0.5));

    //Push Faces onto cube
    Faces.push_back(Face1Verticies);
    Faces.push_back(Face2Verticies);
    Faces.push_back(Face3Verticies);
    Faces.push_back(Face4Verticies);
    Faces.push_back(Face5Verticies);
    Faces.push_back(Face6Verticies);
}
