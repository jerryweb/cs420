
/*
 CSCI 480 Computer Graphics
 Assignment 1: Height Fields
 C++ starter code
 */

#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <pic.h>
#include <jpeglib.h>

int g_iMenuId;

int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;

CONTROLSTATE g_ControlState = ROTATE;

/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

/* see <your pic directory>/pic.h for type Pic */
Pic * g_pHeightData;

/* Write a screenshot to the specified filename */
void saveScreenshot (char *filename)
{
    int i, j;
    Pic *in = NULL;
    
    if (filename == NULL)
        return;
    
    /* Allocate a picture buffer */
    in = pic_alloc(640, 480, 3, NULL);
    
    printf("File to save to: %s\n", filename);
    
    for (i=479; i>=0; i--) {
        glReadPixels(0, 479-i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
                     &in->pix[i*in->nx*in->bpp]);
    }
    
    if (jpeg_write(filename, in))
        printf("File saved Successfully\n");
    else
        printf("Error in Saving\n");
    
    pic_free(in);
}


void myinit()
{
    /* setup gl view here */
    glClearColor(1.0, 1.0, 0.8, 1.0);
    glShadeModel(GL_SMOOTH);
}

void setupCamera(){
    
}



void reshape(int w, int h){
    
    GLfloat aspect = (GLfloat) w / (GLfloat) h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h) /* aspect <= 1 */
        glOrtho(-2.0, 2.0, -2.0/aspect, 2.0/aspect, -10.0, 10.0);
    else /* aspect > 1 */
        glOrtho(-2.0*aspect, 2.0*aspect, -2.0, 2.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    
    
    //create image size
//    glViewport(0, 0, w, h);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
    
    //set the camera
    //glFrustum(-0.1, 0.1, -float(h)/(10.0*float(w)), float(h)/(10.0*float(w)), 0.5, 1000.0);
    glFrustum(-0.3,0.3 , -640, 640, 0.5, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void triangle(){
    
    glRotated(45.0,8.0,0.0,-1.0);
    
    glBegin(GL_TRIANGLES);
    
	glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-0.25, -0.25, 0.50);
    //glColor3f(0.0, 0.0, 1.0);
    glVertex3f(-0.25, 0.25, 0.50);
    //glColor3f(0.0, 0.0, 0.0);
    glVertex3f(0.25, 0.25, 0.0);
    
    glEnd();
    

}
void shapeList(){
    GLuint triangleList = glGenLists(1); //going to create 4 tirangles
        glNewList(triangleList, GL_COMPILE);
            glColor3f(1.0, 0.0, 0.0);
            glBegin(GL_TRIANGLES);
    //glColor3f(1.0, 1.0, 1.0);
    glVertex3f(-0.5, -0.5, 1.0);
    //glColor3f(0.0, 0.0, 1.0);
    glVertex3f(-0.5, 0.5, 1.0);
    //glColor3f(0.0, 0.0, 0.0);
    glVertex3f(0.5, 0.5, 0.0);
    //glColor3f(1.0, 1.0, 0.0);
    //glVertex3f(0.5, -0.5, 0.0);
            glEnd();
        glEndList();
        glCallList(triangleList);
    
    
}

void polygon(){
    
    
    glScaled(.5, .5, 1.0);
    glTranslated(.5, .25, 0);
    glRotated(40.0,0.0,90.0,1.0);
    
    glBegin(GL_POLYGON);
    
    
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(-0.5, -0.5, 1.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(-0.5, 0.5, 1.0);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(0.5, 0.5, 0.0);
    glColor3f(1.0, 1.0, 0.0);
    glVertex3f(0.5, -0.5, 0.0);
    
    glEnd();
}

//void cube(){
    GLfloat vertices[8][3] = {{-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0},
        {1.0, 1.0, -1.0}, {-1.0, 1.0, -1.0}, {-1.0, -1.0, 1.0},
        {1.0, -1.0, 1.0}, {1.0, 1.0, 1.0}, {-1.0, 1.0, 1.0}};
    
    GLfloat colors[8][3] = {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0},
        {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0},
        {1.0, 0.0, 1.0}, {1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}};
    
    
//}

void face(int a, int b, int c, int d)
{
    glBegin(GL_POLYGON);
    glColor3fv(colors[a]);
    glVertex3fv(vertices[a]);
    glColor3fv(colors[b]);
    glVertex3fv(vertices[b]);
    glColor3fv(colors[c]);
    glVertex3fv(vertices[c]);
    glColor3fv(colors[d]);
    glVertex3fv(vertices[d]);
    glEnd();
}

void colorcube(void)
{
    face(0,3,2,1);
    face(2,3,7,6);
    face(0,4,7,3);
    face(1,2,6,5);
    face(4,5,6,7);
    face(0,1,5,4);
}

GLfloat theta[3] = {0.0, 0.0, 0.0};

GLfloat delta = 2.0;
GLint axis = 2;
void spinCube()
{
    
    theta[axis] += delta;
    if (theta[axis] > 360.0) theta[axis] -= 360.0;
    
    glutPostRedisplay();
}

void display()
{
    /* draw 1x1 cube about origin */
    /* replace this code with your height field implementation */
    /* you may also want to precede it with your
     rotation/translation/scaling */
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    setupCamera();
    
    //cube();
    
    glLoadIdentity();
    glRotatef(theta[0], 1.0, 0.0, 0.0);
    glRotatef(theta[1], 0.0, 1.0, 0.0);
    glRotatef(theta[2], 0.0, 0.0, 1.0);
    colorcube();
//    triangle();
//    glLoadIdentity(); // reset transformation
//    polygon();
    //glLoadIdentity();
    //shapeList();

    glutSwapBuffers(); // double buffer flush
}

void menufunc(int value)
{
    switch (value)
    {
        case 0:
            exit(0);
            break;
    }
}

void doIdle()
{
    /* do some stuff... */
    
    /* make the screen update */
    glutPostRedisplay();
}

void mouseForCubeSpin(int btn, int state, int x, int y)
{
    if ((btn==GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
        axis = 0;
    if ((btn==GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN))
        axis = 1;
    if ((btn==GLUT_RIGHT_BUTTON)&& (state == GLUT_DOWN))
        axis = 2; 
}

/* converts mouse drags into information about
 rotation/translation/scaling */
void mousedrag(int x, int y)
{
    int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};
    
    switch (g_ControlState)
    {
        case TRANSLATE:
            if (g_iLeftMouseButton)
            {
                g_vLandTranslate[0] += vMouseDelta[0]*0.01;
                g_vLandTranslate[1] -= vMouseDelta[1]*0.01;
            }
            if (g_iMiddleMouseButton)
            {
                g_vLandTranslate[2] += vMouseDelta[1]*0.01;
            }
            break;
        case ROTATE:
            if (g_iLeftMouseButton)
            {
                g_vLandRotate[0] += vMouseDelta[1];
                g_vLandRotate[1] += vMouseDelta[0];
            }
            if (g_iMiddleMouseButton)
            {
                g_vLandRotate[2] += vMouseDelta[1];
            }
            break;
        case SCALE:
            if (g_iLeftMouseButton)
            {
                g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
                g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
            }
            if (g_iMiddleMouseButton)
            {
                g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
            }
            break;
    }
    g_vMousePos[0] = x;
    g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
    g_vMousePos[0] = x;
    g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{
    
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            g_iLeftMouseButton = (state==GLUT_DOWN);
            break;
        case GLUT_MIDDLE_BUTTON:
            g_iMiddleMouseButton = (state==GLUT_DOWN);
            break;
        case GLUT_RIGHT_BUTTON:
            g_iRightMouseButton = (state==GLUT_DOWN);
            break;
    }
    
    switch(glutGetModifiers())
    {
        case GLUT_ACTIVE_CTRL:
            g_ControlState = TRANSLATE;
            break;
        case GLUT_ACTIVE_SHIFT:
            g_ControlState = SCALE;
            break;
        default:
            g_ControlState = ROTATE;
            break;
    }
    
    g_vMousePos[0] = x;
    g_vMousePos[1] = y;
}

int main (int argc, char ** argv)
{
//    if (argc<2)
//    {
//        printf ("usage: %s heightfield.jpg\n", argv[0]);
//        exit(1);
//    }
//    
//    g_pHeightData = jpeg_read(argv[1], NULL);
//    if (!g_pHeightData)
//    {
//        printf ("error reading %s.\n", argv[1]);
//        exit(1);
//    }
    
    glutInit(&argc,argv);
    
    // request double buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    
    // set window size
    glutInitWindowSize(640, 480);
    
    // set window position
    glutInitWindowPosition(0, 0);
    
    // creates a window
    glutCreateWindow("excersize1");
    /*
     create a window here..should be double buffered and use depth testing
     
     the code past here will segfault if you don't have a window set up....
     replace the exit once you add those calls.
     */
    //exit(0);
    
    /* tells glut to use a particular display function to redraw */
    glutDisplayFunc(display);
    
    /* allow the user to quit using the right mouse button menu */
    g_iMenuId = glutCreateMenu(menufunc);
    glutSetMenu(g_iMenuId);
    glutAddMenuEntry("Quit",0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    /* replace with any animate code */
    //glutIdleFunc(doIdle);
    glutIdleFunc(spinCube);
    
    
    /* callback for mouse drags */
    glutMotionFunc(mousedrag);
    /* callback for idle mouse movement */
    glutPassiveMotionFunc(mouseidle);
    /* callback for mouse button changes */
    //glutMouseFunc(mousebutton);
    glutMouseFunc(mouseForCubeSpin);
    
    /* do initialization */
    myinit();
    glutReshapeFunc(reshape);
    
    glEnable(GL_DEPTH_TEST);
    
    glutMainLoop();
    return(0);
}
