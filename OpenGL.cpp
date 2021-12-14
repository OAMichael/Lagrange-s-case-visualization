#include <GL/glut.h>
#include <sys/types.h>
#include <signal.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

#define XY 1
#define XZ 2
#define YZ 3
#define KEY_ENTER 13
#define KEY_ESCAPE 27
#define KEY_SPACE  32


struct Color {
    GLdouble r;
    GLdouble g;
    GLdouble b;
    GLdouble alpha;

    Color() {}
    Color(GLdouble _r, GLdouble _g, GLdouble _b, GLdouble _alpha) : r{_r}, g{_g}, b{_b}, alpha{_alpha} {}; 
};


struct Point {
    double x;
    double y;
    double z;

    Color P_Color;

    Point() {}
    Point(double _x, double _y, double _z) : x{_x}, y{_y}, z{_z} {};
};


struct Vector {
    double x;
    double y;
    double z;

    double GetLenght() const 
    {
        return sqrt(x * x + y * y + z * z);
    }

    Vector() {}
    Vector(double _x, double _y, double _z) : x{_x}, y{_y}, z{_z} {};
};


struct PhaseVector {
    
    GLdouble theta;
    GLdouble psi;
    GLdouble phi;
    GLdouble p;
    GLdouble q;
    GLdouble r;

    PhaseVector() {}
    PhaseVector(GLdouble _theta, GLdouble _psi, GLdouble _phi, GLdouble _p, GLdouble _q, GLdouble _r) : theta{_theta}, psi{_psi}, phi{_phi},
                                                                                                        p{_p}, q{_q}, r{_r} {}


    PhaseVector& operator=(const PhaseVector& a)
    {
        theta = a.theta;
        psi   = a.psi;
        phi   = a.phi;
        p     = a.p;
        q     = a.q;
        r     = a.r;

        return *this;
    }
};


const PhaseVector operator+(const PhaseVector& a, const PhaseVector& b)
{
    return PhaseVector( a.theta + b.theta,
                        a.psi   + b.psi,
                        a.phi   + b.phi,
                        a.p     + b.p,
                        a.q     + b.q,
                        a.r     + b.r);
}

const PhaseVector operator*(const GLdouble& k, const PhaseVector& a)
{
    return PhaseVector( k * a.theta,
                        k * a.psi,
                        k * a.phi,
                        k * a.p,
                        k * a.q,
                        k * a.r );
}

std::ostream& operator<<(std::ostream& out, const PhaseVector& Vec) 
{
    std::cout << "theta = " << Vec.theta << "\npsi = " << Vec.psi << "\nphi = " << Vec.phi <<
                 "\np = " << Vec.p << "\nq = " << Vec.q << "\nr = " << Vec.r << "\n\n";

    return out;
}


void Init(void);
void Timer(int);
void DrawVector(Vector& Vec, Point& Start, GLfloat Thickness, Color& Col);
void DrawSphere(double r, int lats, int longs);
void DrawCircle(Point& Center, double Radius, unsigned int Axis, GLfloat Thickness, Color& Inner_Col, Color& Outer_Col);
void DrawCircum(Point& Center, double Radius, unsigned int Axis, GLfloat Thickness, Color& Col);
void display(void);
void Keyboard(unsigned char Typped, int x, int y);
void SpecKeyboard(int key, int x, int y);
void Mouse(int Button, int State, int x, int y);
void Reshape(int w, int h);
void AssignInitials(int argc, char* argv[]);
const PhaseVector F(const PhaseVector& Vec);

//====================================================INITIAL CONDITIONS====================================================

GLdouble M = 0.5;               // Mass
GLdouble L = 2;                 // Distance to center of mass
GLdouble A = 15;                // A moment of inertia
GLdouble C = 30;                // C moment of inertia
GLdouble theta_0 = M_PI / 3.0;  // Initial theta
GLdouble psi_0 = 0;             // Initial psi
GLdouble phi_0 = 0.0;           // Initial phi
GLdouble theta_dot_0 = 0.1;     // Initial theta_dot
GLdouble psi_dot_0 = 0.75;      // Initial psi dot
GLdouble phi_dot_0 = 0.75;      // Initial phi_dot

//========================================================CONSTANTS=========================================================

const GLdouble g = 9.81;        // Gravitational acceleration

//===================================================SIMULATION PARAMETERS==================================================

GLdouble Time = 0;
GLdouble dt = 1 / 100.0;
GLfloat  RotAngleX = 10.0;
GLfloat  RotAngleY = 0.0;
GLfloat  RotAngleZ = 0.0;
GLfloat  Scale = 1;

//=====================================================BODY'S PARAMETERS====================================================

GLdouble MainSphereR = 1.0;
GLdouble theta_min = M_PI;
GLdouble theta_max = 0;
PhaseVector State{};

//==========================================================================================================================

std::vector<Point> Trace;
using VecIter = typename std::vector<Point>::iterator;


int main(int argc, char* argv[])
{
    AssignInitials(argc, argv);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    glutInitWindowPosition(400, 50);
    glutInitWindowSize(900, 900);
     
    glutCreateWindow("Lagrange's case");
    Init();

    glutDisplayFunc(display);
    glutReshapeFunc(Reshape);

    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(SpecKeyboard);
    glutMouseFunc(Mouse);
    
    glutTimerFunc(1000, Timer, 0);
    glutMainLoop();

    
    return 0;
}


const PhaseVector F(const PhaseVector& Vec)
{
    PhaseVector x;

    x.theta = Vec.p * cos(Vec.phi) - Vec.q * sin(Vec.phi);

    x.psi = (Vec.p * sin(Vec.phi) + Vec.q * cos(Vec.phi)) / sin(Vec.theta);

    x.phi = Vec.r - (Vec.p * sin(Vec.phi) + Vec.q * cos(Vec.phi)) / tan(Vec.theta);

    x.p =  M * g * L * sin(Vec.theta) * cos(Vec.phi) / A + (A - C) / A * Vec.q * Vec.r;

    x.q = -M * g * L * sin(Vec.theta) * sin(Vec.phi) / A - (A - C) / A * Vec.p * Vec.r;

    x.r = 0.0;

    return x;
}


void Keyboard(unsigned char Typped, int x, int y)
{
    switch(Typped)
    {
        case 'q':
        case KEY_ESCAPE:        kill(0, SIGKILL);             exit(0);

        case KEY_SPACE:         Trace.clear();      break;

        case KEY_ENTER:         RotAngleX = 10;
                                RotAngleY = 0;
                                RotAngleZ = 0;
                                Scale = 1;          break;

        case 'a':               RotAngleY += 1.0;   break;

        case 'd':               RotAngleY -= 1.0;   break;

        case 'w':               RotAngleX += 1.0;   break;

        case 's':               RotAngleX -= 1.0;   break;
    }

    return;
}


void SpecKeyboard(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_UP:       RotAngleX += 1.0;   break;

        case GLUT_KEY_DOWN:     RotAngleX -= 1.0;   break;

        case GLUT_KEY_LEFT:     RotAngleY += 1.0;   break;

        case GLUT_KEY_RIGHT:    RotAngleY -= 1.0;   break;
    }

    return;
}


void Mouse(int Button, int State, int x, int y)
{
    switch(Button)
    {
        case 3:     Scale *= 1.1;   break; 
        
        case 4:     Scale /= 1.1;   break;
    }

    return;
}


void DrawSphere(double r, int lats, int longs)
{
    int i = 0, 
        j = 0;

    for(i = 0; i <= lats; ++i) 
    {
        double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
        double z0  =  sin(lat0);
        double zr0 =  cos(lat0);

        double lat1 = M_PI * (-0.5 + (double) i / lats);
        double z1 =  sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);

        for(j = 0; j <= longs; ++j) 
        {
            double lng = 2 * M_PI * (double) (j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);

            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(r * x * zr0, r * y * zr0, r * z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(r * x * zr1, r * y * zr1, r * z1);
        }
        glEnd();
    }

    return;
}


void Init(void)
{
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_POINT_SMOOTH);

    return;
}

void display(void)
{
    glClearColor(1, 1, 1, 0.1); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glScalef(0.7, 0.7, 0.7);

    glRotatef(RotAngleX, 1, 0, 0);
    glRotatef(RotAngleY, 0, 1, 0);
    glRotatef(RotAngleZ, 0, 0, 1);

    glScalef(Scale, Scale, Scale);

    glColor3f(0, 0, 0);

    DrawSphere(MainSphereR, 100, 100);
    glColor3f(1, 1, 1);
    DrawSphere(MainSphereR * 0.99, 100, 100);

    glColor3f(0, 0, 0);
    Point p1{0, MainSphereR * cos(theta_max), 0};
    Point p2{0, MainSphereR * cos(theta_min), 0};

    double R1 = sqrt(MainSphereR * MainSphereR - p1.y * p1.y);
    double R2 = sqrt(MainSphereR * MainSphereR - p2.y * p2.y);
    Point Center{0, 0, 0};

    Color C1{0.2, 0.8, 0.2, 0.9};
    Color Outer{0, 0, 0, 0};
    Color C2{0.2, 0.8, 0.2, 0.9};

    glPointSize(9);
    glBegin(GL_POINTS);
        glVertex3f(0, 0, 0);
    glEnd();

    DrawCircle(p1, R1, XZ, 5.0, C1, Outer);
    if(p1.y > 0)
    {
        glBegin(GL_POINTS);
            glVertex3f(0, p1.y, 0);
        glEnd();
    }

    glBegin(GL_LINES);
        for(int i = 0; i < 16; ++i)
        {
            glVertex3f(0, p1.y, 0);
            glVertex3f(R1 * cos(M_PI * i / 8.0), p1.y, R1 * sin(M_PI * i / 8.0));
        }
    glEnd(); 

    glColor3f(1, 0, 0);
    
    glPointSize(3);
    glBegin(GL_POINTS);
        for(VecIter Iter = Trace.begin(), IterEnd = Trace.end(); Iter != IterEnd; ++Iter)
            glVertex3f((*Iter).x, (*Iter).y, (*Iter).z);

    glEnd();
    glPointSize(1);

    if(Trace.size())
    {
        Vector e_3{ Trace.front().x, 
                    Trace.front().y,
                    Trace.front().z };

        Color Red{1, 0, 0, 0};

        DrawVector(e_3, Center, 5, Red);
    }

    DrawCircle(p2, R2, XZ, 5.0, C2, Outer);
    if(p2.y > 0)
    {
        glPointSize(9);
        glBegin(GL_POINTS);
            glVertex3f(0, p2.y, 0);
        glEnd();
        glPointSize(1);
    }

    glBegin(GL_LINES);
        for(int i = 0; i < 16; ++i)
        {
            glVertex3f(0, p2.y, 0);
            glVertex3f(R2 * cos(M_PI * i / 8.0), p2.y, R2 * sin(M_PI * i / 8.0));
        }
    glEnd(); 

    Vector i_3{0, 1, 0};
    Color Black{0, 0, 0, 0}; 

    DrawVector(i_3, Center, 5, Black);

    glutSwapBuffers();

    return;
}


void DrawCircum(Point& Center, double Radius, unsigned int Axis, GLfloat Thickness, Color& Col)
{

    glLineWidth(Thickness);
    glColor3f(Col.r, Col.g, Col.b);
    glBegin(GL_LINE_STRIP);

        for(int i = 0; i < 8000; ++i)
        {
            switch(Axis)
            {
                case XY: 
                    glVertex3f(Center.x + Radius * cos(M_PI * i / 4000.0), Center.y + Radius * sin(M_PI * i / 4000.0), Center.z);
                    break;

                case XZ:
                    glVertex3f(Center.x + Radius * cos(M_PI * i / 4000.0), Center.y, Center.z + Radius * sin(M_PI * i / 4000.0));
                    break;

                case YZ:
                    glVertex3f(Center.x, Center.y + Radius * cos(M_PI * i / 4000.0), Center.z + Radius * sin(M_PI * i / 4000.0));
                    break;

                default: exit(EXIT_FAILURE);
            }
        }

    glEnd();
    
    glLineWidth(1.0f);
    glColor3f(0, 0, 0);

    return;
}

void DrawCircle(Point& Center, double Radius, unsigned int Axis, GLfloat Thickness, Color& Inner_Col, Color& Outer_Col)
{
    glColor4f(Inner_Col.r, Inner_Col.g, Inner_Col.b, Inner_Col.alpha);
    glBegin(GL_TRIANGLE_FAN);

        for(int i = 0; i < 8000; ++i)
        {
            switch(Axis)
            {
                case XY: 
                    glVertex3f(Center.x + Radius * cos(M_PI * i / 4000.0), Center.y + Radius * sin(M_PI * i / 4000.0), Center.z);
                    break;

                case XZ:
                    glVertex3f(Center.x + Radius * cos(M_PI * i / 4000.0), Center.y, Center.z + Radius * sin(M_PI * i / 4000.0));
                    break;

                case YZ:
                    glVertex3f(Center.x, Center.y + Radius * cos(M_PI * i / 4000.0), Center.z + Radius * sin(M_PI * i / 4000.0));
                    break;

                default: exit(EXIT_FAILURE);
            }
        }

    glEnd();

    DrawCircum(Center, Radius, Axis, Thickness, Outer_Col);

    return;
}


void Timer(int)
{
    glutPostRedisplay();
    glutTimerFunc(1000/60, Timer, 0);

    if(abs(State.p * cos(State.phi) - State.q * sin(State.phi)) < 1e-2)
    {
        if(State.theta > theta_max)
            theta_max = State.theta;

        if(State.theta < theta_min)
            theta_min = State.theta;
    }

//=================================================PROCEEDING THE QUANTITIES================================================

    // Using Runge-Kutta's method
    PhaseVector k1 = dt * F(State);
    PhaseVector k2 = dt * F(State + 0.5 * k1);
    PhaseVector k3 = dt * F(State + 0.5 * k2);
    PhaseVector k4 = dt * F(State + k3);

    State = State + 0.1666666667 * (k1 + 2 * k2 + 2 * k3 + k4);

    Time += dt;


    // Using Euler method
    //State = State + dt * F(State);

    std::ofstream Data;
    Data.open("data.txt", std::ios::app | std::ios::out);
    Data << Time << " " << State.theta << " " << State.psi << " " << State.phi << " " << State.p << " " << State.q << " " << State.r << std::endl;
    Data.close();

    std::cout << "State:\n" << State;

//==========================================================================================================================


    Point A{ MainSphereR * sin(State.theta) * cos(State.psi),
             MainSphereR * cos(State.theta),
             MainSphereR * sin(State.theta) * sin(State.psi) };

    Trace.insert(Trace.begin(), A);

    return;
}


void Reshape(int Width, int Height)
{
    double hScale = static_cast<double>(Height) / 900.0;
    double wScale = static_cast<double>(Width)  / 900.0;
    glOrtho(0, wScale, 0, hScale, -1, 1);

    return;
}


void DrawVector(Vector& Vec, Point& Start, GLfloat Thickness, Color& Col)
{
    glLineWidth(Thickness);
    glBegin(GL_LINES);
        glVertex3f(Start.x, Start.y, Start.z);
        glVertex3f(Start.x + Vec.x, Start.y + Vec.y, Start.z + Vec.z);
    glEnd();
    glLineWidth(1.0f);


    glPushMatrix();
        glTranslatef(0.97 * Vec.x, 0.97 * Vec.y, 0.97 * Vec.z);
        glRotatef(-180.0 / M_PI * acos(Vec.z / Vec.GetLenght()), Vec.y * 0.05, -Vec.x * 0.05, 0);
        glutSolidCone(0.02, 0.05, 100, 100);

    glPopMatrix();

    return;
}

void AssignInitials(int argc, char* argv[])
{
    if(argc > 1)
    {
        M = atof(argv[1]);
        L = atof(argv[2]);
        A = atof(argv[3]);
        C = atof(argv[4]);
        theta_0 = atof(argv[5]);
        psi_0 = atof(argv[6]);
        phi_0 = atof(argv[7]);
        theta_dot_0 = atof(argv[8]);
        psi_dot_0 = atof(argv[9]);
        phi_dot_0 = atof(argv[10]);
    }

    State.theta = theta_0;
    State.psi   = psi_0;
    State.phi   = phi_0;
    State.p = psi_dot_0 * sin(theta_0) * sin(phi_0) + theta_dot_0 * cos(phi_0);
    State.q = psi_dot_0 * sin(theta_0) * cos(phi_0) - theta_dot_0 * sin(phi_0);
    State.r = psi_dot_0 * cos(theta_0) + phi_dot_0;

    std::ofstream Data0;
    Data0.open("data.txt", std::ios::trunc | std::ios::out);
    Data0 << Time << " " << State.theta << " " << State.psi << " " << State.phi << " " << State.p << " " << State.q << " " << State.r << std::endl;
    Data0.close();

    return;
}