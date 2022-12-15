#include <GL/glut.h>
#include <GL/freeglut.h>
#include <bits/stdc++.h>
#include <math.h>

#define pi 3.14159265359
#define FMAX 100

using namespace std;

float M = 2.4; // mass of cart
float m = 0.1; // mass of sphere
float g = 1; // gravity
float l = 0.3; // pole length

float s = 10;  // scaling factor

GLfloat c = 3; // shift down everything by c
GLfloat a = 0.1; // length of cart
GLfloat b = 1.5; // width of cart

// variables for restarting the game
int lives = 3;
bool restart = true;
int restart_time = 0;

// variables for the second part
float x1 = 0;
float x_dot1 = 0;
float theta1 = 0.1;
float theta_dot1 = 0;
float omega_dot1 = ((M+m)*g*tan(theta1) - m*l*theta_dot1*theta_dot1*sin(theta1))/((M+m)*l/cos(theta1) - m*l*cos(theta1));
float v_dot1 = (l*omega_dot1 - g*sin(theta1))/cos(theta1);
float F = 0;
bool ball_right1 = false;
bool ball_left1 = false;
float x_right1 = -10;
float x_left1 = 13;

// variables for the first part
bool ball_right=true;
bool ball_left=false;
float x_right = -13;
float x_left = 10;
float A[4][4] = {{0, 1, 0, 0},
                 {(M+m)*g/(M*l), 0, 0, 0},
                 {0, 0, 0, 1},
                 {-m*g/M, 0, 0, 0}};
float B[4][1] = {{0},
                 {1/(M*l)},
                 {0},
                 {1/M}};
float Q[4][4] = {{1, 0, 0, 0},
                 {0, 0, 0, 0},
                 {0, 0, 1, 0},
                 {0, 0, 0, 0}};
float R[1][1] = {{1}};
float K[1][4] = {{ 8.24487052, 4.35939931, -1, -3.25667235}};
float A_BK[4][4] = {{0,0,0,0},
                 {0,0,0,0},
                 {0,0,0,0},
                 {0,0,0,0}};


float theta = 0;  // setting up initial angle
float theta_dot = 0;  // setting up initial omega
float x = 0;
float x_dot = 0;


//////////////////////////////////prototypes/////////
void display();
void reshape(int, int);
void timer(int);
void mykey(int, int, int);


// driver
int main(int argc, char**argv)
{
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            A_BK[i][j] = A[i][j] - B[i][0]*K[0][j];
        }
    }


    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowPosition(0, 100);
    glutInitWindowSize(2000, 500);
    glutCreateWindow("LQR for Inverted Cartpole Problem");
    glutDisplayFunc(display);
    glutSpecialFunc(mykey);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);

    glClearColor(0,0,0, 1);
    glEnable(GL_DEPTH_TEST);

    glutMainLoop();
}


void draw_circle(double ori_x, double ori_y, float z, float radius)
{
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 360; i++) {
        double angle = 2 * 3.14 * i / 360;
        double x_c = cos(angle) * radius;
        double y_c = sin(angle) * radius;
        glVertex3f(ori_x + x_c, ori_y + y_c, -z);
    }
    glEnd();
}

void Restart()
{
    glColor3f(1,1,1);
    if(restart_time<60)
    {
        glRasterPos3f(0,0, -10);
        const unsigned char* text = reinterpret_cast<const unsigned char *>("3");
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, text);
        restart_time++;
    }
    else if(restart_time>=60 && restart_time<120)
    {
        glRasterPos3f(0,0, -10);
        const unsigned char* text = reinterpret_cast<const unsigned char *>("2");
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, text);
        restart_time++;
    }
    else if(restart_time>=120 && restart_time<180)
    {
        glRasterPos3f(0,0, -10);
        const unsigned char* text = reinterpret_cast<const unsigned char *>("1");
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, text);
        restart_time++;
    }
    else
    {
        restart_time=0;
        restart=false;
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if(restart==true)
    {
        Restart();
    }
    else{
        for(int i=0; i<lives; i++)  // draw lives
        {
            glColor3f(0,0,0);
            draw_circle(20+i*0.5, 5, 10, 0.2);
        }
        glRasterPos3f(18, 4.85, -10);

        const unsigned char* text = reinterpret_cast<const unsigned char *>("Lives:");
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, text);  // write lives
        glTranslatef(-10, 0, 0);
        glLineWidth(3.5);
        glBegin(GL_LINES);

        glColor3f(0.749, 0.749, 0.749);
        glVertex3f(s*(x+l*sin(theta)), s*l*cos(theta)-c, -10);
        glVertex3f(s*x, -c, -10);
        glEnd();   // draw first pendulum

        glColor3f(0.4,0.4,0.4);
        draw_circle(s*(x+l*sin(theta)), s*l*cos(theta)-c, 9.999, 0.2);  // draw first bob

        glBegin(GL_QUADS);

        glColor3f(1, 0.4588, 0.1019);
        glVertex3f(-150, -40, -49);
        glVertex3f(-150, 40, -49);
        glVertex3f(10, 40, -49);
        glVertex3f(10, -40, -49);

        //front
        glColor3f(0.2, 0.1019, 0);
        glVertex3f(b+s*x, -c, -9);
        glVertex3f(-b+s*x, -c, -9);
        glVertex3f(-b+s*x, -2*a-c, -9);
        glVertex3f(b+s*x, -2*a-c, -9);

        //back
        glColor3f(1,1,0);
        glVertex3f(b+s*x, -c, -11);
        glVertex3f(b+s*x, -2*a-c, -11);
        glVertex3f(-b+s*x, -2*a-c, -11);
        glVertex3f(-b+s*x, -c, -11);

        //left
        glColor3f(0.3019,0.1568,0);
        glVertex3f(-b+s*x, -c, -11);
        glVertex3f(-b+s*x, -c, -9);
        glVertex3f(-b+s*x, -2*a-c, -9);
        glVertex3f(-b+s*x, -2*a-c, -11);

        //right
        glColor3f(0.3019,0.1568,0);
        glVertex3f(b+s*x, -c, -11);
        glVertex3f(b+s*x, -c, -9);
        glVertex3f(b+s*x, -2*a-c, -9);
        glVertex3f(b+s*x, -2*a-c, -11);

        //up
        glColor3f(0.4, 0.2078, 0);
        glVertex3f(-b+s*x, -c, -11);
        glVertex3f(-b+s*x, -c, -9);
        glVertex3f(b+s*x, -c, -9);
        glVertex3f(b+s*x, -c, -11);

        //down
        glColor3f(0.8, 0.3, 0);
        glVertex3f(-b+s*x, -2*a-c, -11);
        glVertex3f(b+s*x, -2*a-c, -11);
        glVertex3f(b+s*x, -2*a-c, -9);
        glVertex3f(-b+s*x, -2*a-c, -9);            // first cart

        glColor3f(0.301960784, 0.298039216, 0.28627451);
        glVertex3f(-20, -2*a-c-0.4, -13);
        glVertex3f(10, -2*a-c-0.4, -13);
        glVertex3f(10, -2*a-c-0.4, -4);
        glVertex3f(-20, -2*a-c-0.4, -4);
        glEnd();                                     // first road

        //first wheel
        glColor3f(0,0,0);
        draw_circle(b+s*x-0.3, -2*a-c-0.2, 9.1, 0.2);
        draw_circle(-b+s*x+0.3, -2*a-c-0.2, 9.1, 0.2);
        draw_circle(b+s*x-0.3, -2*a-c-0.2, 10.9, 0.2);
        draw_circle(-b+s*x+0.3, -2*a-c-0.2, 10.9, 0.2);


        if(ball_right)
        {
            // ball left to right
            glColor3f(1,0,0);
            draw_circle(x_right, -a-c, 10, 0.1);
            if(x_right>(-b+s*x))
            {
                x_right=-13;
                ball_right=false;
                theta_dot-=1/(M*l);
                x_dot+=1/M;
                ball_left=true;
            }
        }
        else if(ball_left)
        {
            // ball right to left
            glColor3f(1,0,0);
            draw_circle(x_left, -a-c, 10, 0.1);
            if(x_left<(b+s*x))
            {
                x_left=10;
                ball_left=false;
                theta_dot+=1/(M*l);
                x_dot-=1/M;
                ball_right=true;
            }
        }

        glTranslatef(10, 0, 0);

        glTranslatef(10, 0, 0);

        glBegin(GL_QUADS);
        glColor3f(0.792156863, 0.968627451, 0.980392157);
        glVertex3f(-10, -40, -49);
        glVertex3f(-10, 40, -49);
        glVertex3f(150, 40, -49);
        glVertex3f(150, -40, -49);

        glColor3f(0.2,0.2,0.2);
        glVertex3f(-10, -2*a-c-0.4, -13);
        glVertex3f(20, -2*a-c-0.4, -13);
        glVertex3f(20, -2*a-c-0.4, -4);
        glVertex3f(-10, -2*a-c-0.4, -4);

        //front
        glColor3f(0.2, 0.1019, 0);
        glVertex3f(b+s*x1, -c, -9);
        glVertex3f(-b+s*x1, -c, -9);
        glVertex3f(-b+s*x1, -2*a-c, -9);
        glVertex3f(b+s*x1, -2*a-c, -9);

        //back
        glColor3f(1,1,0);
        glVertex3f(b+s*x1, -c, -11);
        glVertex3f(b+s*x1, -2*a-c, -11);
        glVertex3f(-b+s*x1, -2*a-c, -11);
        glVertex3f(-b+s*x1, -c, -11);

        //left
        glColor3f(0.3019,0.1568,0);
        glVertex3f(-b+s*x1, -c, -11);
        glVertex3f(-b+s*x1, -c, -9);
        glVertex3f(-b+s*x1, -2*a-c, -9);
        glVertex3f(-b+s*x1, -2*a-c, -11);

        //right
        glColor3f(0.3019,0.1568,0);
        glVertex3f(b+s*x1, -c, -11);
        glVertex3f(b+s*x1, -c, -9);
        glVertex3f(b+s*x1, -2*a-c, -9);
        glVertex3f(b+s*x1, -2*a-c, -11);

        //up
        glColor3f(0.4, 0.2078, 0);
        glVertex3f(-b+s*x1, -c, -11);
        glVertex3f(-b+s*x1, -c, -9);
        glVertex3f(b+s*x1, -c, -9);
        glVertex3f(b+s*x1, -c, -11);

        //down
        glColor3f(0.8, 0.3, 0);
        glVertex3f(-b+s*x1, -2*a-c, -11);
        glVertex3f(b+s*x1, -2*a-c, -11);
        glVertex3f(b+s*x1, -2*a-c, -9);
        glVertex3f(-b+s*x1, -2*a-c, -9);

        glEnd();

        //wheel
        glColor3f(0,0,0);
        draw_circle(b+s*x1-0.3, -2*a-c-0.2, 9.1, 0.2);
        draw_circle(-b+s*x1+0.3, -2*a-c-0.2, 9.1, 0.2);
        draw_circle(b+s*x1-0.3, -2*a-c-0.2, 10.9, 0.2);
        draw_circle(-b+s*x1+0.3, -2*a-c-0.2, 10.9, 0.2);

        glLineWidth(3.5);
        glBegin(GL_LINES);
        glColor3f(0.749, 0.749, 0.749);
        glVertex3f(s*(x1+l*sin(-theta1)), s*l*cos(-theta1)-c, -10);
        glVertex3f(s*x1, -c, -10);
        glEnd();


        glColor3f(0.4,0.4,0.4);
        draw_circle(s*(x1+l*sin(-theta1)), s*l*cos(-theta1)-c, 9.999, 0.2);


        if(ball_right1)
        {
            // ball left to right
            glColor3f(1,0,0);
            draw_circle(x_right1, -a-c, 10, 0.1);
            if(x_right1>(-b+s*x1))
            {
                x_right1=-10;
                ball_right1=false;
                F = FMAX;
            }
        }
        else if(ball_left1)
        {
            // ball right to left
            glColor3f(1,0,0);
            draw_circle(x_left1, -a-c, 10, 0.1);
            if(x_left1<(b+s*x1))
            {
                x_left1=13;
                ball_left1=false;
                F=-FMAX;
            }
        }

        glTranslatef(-10, 0, 0);
    }
    glutSwapBuffers();
}

void mykey(int key, int x, int y)
{
   if(key==GLUT_KEY_LEFT)
   {
       ball_left1 = true;
   }

   else if(key==GLUT_KEY_RIGHT)
   {
       ball_right1 = true;
   }
}

void reshape(int w, int h)
{
    glViewport(0,0,(GLsizei)w,(GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 4, 2, 50);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int)
{
    glutPostRedisplay();
    glutTimerFunc(1000/60, timer, 0);
    if(restart==false)
    {
        float X[4][1] = {{theta},
                        {theta_dot},
                        {x},
                        {x_dot}};

        float y[4][1] = {{0}, {0}, {0}, {0}};
        for(int i=0; i<4; i++)
        {
            for(int j=0; j<4; j++)
            {
                y[i][0]+=A_BK[i][j]*X[j][0];
            }
        }
        theta = theta + theta_dot/60;  // next theta = prev theta + theta_dot*timestep
        // next theta_dot = prev theta_dot + ang acceleration*timestep
        theta_dot = theta_dot + (y[1][0])/60;

        x = x + x_dot/60;  // next x = prev x + x_dot*timestep
        // next x_dot = prev x_dot + acceleration*timestep
        x_dot = x_dot + (y[3][0])/60;

        if(ball_right)
        {
            x_right+=0.1;
        }
        else if(ball_left)
        {
            x_left-=0.1;
        }


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        omega_dot1 = (F + (M+m)*g*tan(theta1) - m*l*theta_dot1*theta_dot1*sin(theta1))/((M+m)*l/cos(theta1) - m*l*cos(theta1));
        v_dot1 = (l*omega_dot1 - g*sin(theta1))/cos(theta1);

        theta_dot1 = theta_dot1 + omega_dot1/60;
        theta1 = theta1 + theta_dot1/60;
        x_dot1 = x_dot1 + v_dot1/60;
        x1 = x1 + x_dot1/60;
        if(theta1>pi/2 || theta1<-pi/2)
        {
            x1 = 0;
            x_dot1 = 0;
            theta1 = 0.1;
            theta_dot1 = 0;
            omega_dot1 = ((M+m)*g*tan(theta1) - m*l*theta_dot1*theta_dot1*sin(theta1))/((M+m)*l/cos(theta1) - m*l*cos(theta1));
            v_dot1 = (l*omega_dot1 - g*sin(theta1))/cos(theta1);
            F = 0;
            ball_right1 = false;
            ball_left1 = false;
            x_right1 = -11;
            x_left1 = 10;
            lives--;
            restart = true;
            if(lives==0)
                exit(0);
        }

        if(ball_right1)
        {
            x_right1+=1;
        }
        else if(ball_left1)
        {
            x_left1-=1;
        }

        F=0;

        if(x1>1.3 || x1<-0.85)
        {
            x1 = 0;
            x_dot1 = 0;
            theta1 = 0.1;
            theta_dot1 = 0;
            omega_dot1 = ((M+m)*g*tan(theta1) - m*l*theta_dot1*theta_dot1*sin(theta1))/((M+m)*l/cos(theta1) - m*l*cos(theta1));
            v_dot1 = (l*omega_dot1 - g*sin(theta1))/cos(theta1);
            F = 0;
            ball_right1 = false;
            ball_left1 = false;
            x_right1 = -10;
            x_left1 = 13;
            lives--;
            restart= true;
            if(lives==0)
                exit(0);
        }
    }
}
