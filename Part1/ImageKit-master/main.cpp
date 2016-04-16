#include "Canvas.h"
#include <math.h>
#include "common.h"

unsigned int width = 512;
unsigned int height = 512;

void drawFace(Matrix4d m, std::vector<Vector3d> face){
    for(int i = 0; i < 4; i++){
        Vector3d a = face[i];
        Vector3d b = face[(i+1)%4];

        MatrixXd p(4,1);
            p(0,0) = a[0];
            p(1,0) = a[1];
            p(2,0) = a[2];
            p(3,0) = 1;

        MatrixXd q(4,1);
            q(0,0) = b[0];
            q(1,0) = b[1];
            q(2,0) = b[2];
            q(3,0) = 1;

        MatrixXd mp = m * p;
        MatrixXd mq = m * q;

        float kat = mp(3,0);
        float mew = mq(3,0);

        canvas.AddLine(mp(0,0)/(kat*zoom), mp(1,0)/(kat*zoom), mq(0,0)/(mew*zoom), mq(1,0)/(mew*zoom));
    }
}

void drawCube(Matrix4d m){
    for(int i = 0; i < 6; i++){
        drawFace(m, Faces[i]);
    }
}

Vector3d e(0,0,3);

void OnPaint()
{
    canvas.Clear();
    //DrawCross(vppos_x, vppos_y);

    /*Matrix3d Rot;
    Rot << cos(x_turn), 0, sin(x_turn),
                    0,1,0,
                    -sin(x_turn), 0, cos(x_turn);

    e = Rot * e;
    e.normalize();
    e *= 12;*/

    Vector3d e(sin(x_turn)*cos(y_turn),sin(y_turn),cos(x_turn)*cos(y_turn));
    Vector3d lookAt(0,0,0);
    Vector3d g = lookAt - e;
    Vector3d up(0,cos(y_turn),0);

    Vector3d w = -1 * g.normalized();
    Vector3d upCrossw = up.cross(w);
    Vector3d u = upCrossw.normalized();
    Vector3d v = w.cross(u);

    Matrix4d mv;
        mv(0,0) = u[0];
        mv(0,1) = u[1];
        mv(0,2) = u[2];
        mv(0,3) = 0;

        mv(1,0) = v[0];
        mv(1,1) = v[1];
        mv(1,2) = v[2];
        mv(1,3) = 0;

        mv(2,0) = w[0];
        mv(2,1) = w[1];
        mv(2,2) = w[2];
        mv(2,3) = 0;

        mv(3,0) = 0;
        mv(3,1) = 0;
        mv(3,2) = 0;
        mv(3,3) = 1;

    Matrix4d mv1;
        mv1(0,0) = 1;
        mv1(0,1) = 0;
        mv1(0,2) = 0;
        mv1(0,3) = -(e[0]);

        mv1(1,0) = 0;
        mv1(1,1) = 1;
        mv1(1,2) = 0;
        mv1(1,3) = -(e[1]);

        mv1(2,0) = 0;
        mv1(2,1) = 0;
        mv1(2,2) = 1;
        mv1(2,3) = -(e[2]);

        mv1(3,0) = 0;
        mv1(3,1) = 0;
        mv1(3,2) = 0;
        mv1(3,3) = 1;

    mv = mv * mv1;

    Matrix4d m = mprorth * mv;
    drawCube(m);
}

int main(int, char **){

    initCube();

    initMatrices();

    linkCallBacks();
    canvas.SetOnPaint(OnPaint);

    //Show Window
    canvas.Show(width, height, "Canvas Demo");
    return 0;
}
