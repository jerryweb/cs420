/*
CSCI 480
Assignment 3 Raytracer

Name: Jerry Webb
*/

#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <pic.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
//#include <Vec3.h>

#define MAX_TRIANGLES 2000
#define MAX_SPHERES 10
#define MAX_LIGHTS 10

using namespace std;

char *filename=0;

//different display modes
#define MODE_DISPLAY 1
#define MODE_JPEG 2
int mode=MODE_DISPLAY;

//you may want to make these smaller for debugging purposes
#define WIDTH 320 // instead of 640
#define HEIGHT 240 // instead of 480
#define PI 3.14159265



//the field of view of the camera
#define fov 60.0

unsigned char buffer[HEIGHT][WIDTH][3];

struct Ray
{
    double startPoint[3];
    double direction[3];
    
};
struct Vertex
{
  double position[3];
  double color_diffuse[3];
  double color_specular[3];
  double normal[3];
  double shininess;
};

typedef struct _Triangle
{
  struct Vertex v[3];
} Triangle;

typedef struct _Sphere
{
  double position[3];
  double color_diffuse[3];
  double color_specular[3];
    double shininess;
  double radius;
} Sphere;

typedef struct _Light
{
  double position[3];
  double color[3];
} Light;


Vertex topLeftCornor;
Vertex topRightCornor;
Vertex bottomLeftCornor;
Vertex bottomRightCornor;
Vertex centerRay;
Ray testRay;
Vertex upVector;
Triangle triangles[MAX_TRIANGLES];
Sphere spheres[MAX_SPHERES];
Light lights[MAX_LIGHTS];
double ambient_light[3];

double origin[3] = {0,0,0};
double point[3];

int num_triangles=0;
int num_spheres=1;
int num_lights=0;



void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void calculateGridCornors();
void calculateSphere();
double radianConversion(double angle);
void vectorNomalization(double vector[]);


//This function is used to genrate and calculate the original rays for each pixel in the screen
Ray createRay(double xPixelPos,double yPixelPos, double t){
    double w[3];
    double u,v; // the u and v vectors for the camera position
    double aspect = WIDTH/HEIGHT; // aspect ration W/H
    double radianAngle =  radianConversion(fov);
    
    u = -aspect*t + (aspect*t + (aspect*t))*((xPixelPos + 0.5)/WIDTH);
//    u = -topLeftCornor.position[0] + (topLeftCornor.position[0] - (-topLeftCornor.position[0]))*((xPixelPos + 0.5)/WIDTH);
    v = -aspect*radianAngle + (aspect*radianAngle  + aspect*radianAngle)*((yPixelPos +0.5)/HEIGHT);
    
    cout << u << " " << v << endl;
    Ray newRay;
    newRay.direction[0] = u; //{xPixelPos,yPixelPos,zPixelPos};
    newRay.direction[1] = v;
    newRay.direction[2] = -1;
    newRay.startPoint[0] = 0; // x position of the origin
    newRay.startPoint[1] = 0; // y position of the origin
    newRay.startPoint[2] = 0; // z position of the origin
    //this normalizes the vector to get the direction
    for(int i =0; i<3; i++){
        w[i] = (newRay.startPoint[i] - newRay.direction[i])/((newRay.startPoint[i] - newRay.direction[i]) * (newRay.startPoint[i] -newRay.direction[i]));
    }
    
    double magnitude = sqrt(newRay.direction[0]*newRay.direction[0] + newRay.direction[1]*newRay.direction[1] + newRay.direction[2]*newRay.direction[2]);
    for(int i =0; i<2; i++){
        newRay.direction[i] = newRay.direction[i]/magnitude;
        newRay.direction[i] = newRay.startPoint[i] + newRay.direction[i];//*t;
    }
    printf ("magnitude: %lf   ray point:  %lf , %lf , %lf .\n", magnitude, newRay.direction[0], newRay.direction[1], newRay.direction[2]);
    return newRay;
}



//this function calculates p(t) of the ray
void calculateRay(Ray* ray, double t){
    Ray rayInput = *ray;
    //printf ("ray point:  %lf , %lf , %lf .\n", rayInput.direction[0], rayInput.direction[1], rayInput.direction[2]);
    
    for(int i =0; i<3; i++){
        //rayInput.direction[i] = rayInput.direction[i]/magnitude;
        rayInput.direction[i] = rayInput.startPoint[i] + rayInput.direction[i]*t;
    }
    // printf ("ray point:  %lf , %lf , %lf .\n",rayInput.direction[0], rayInput.direction[1], rayInput.direction[2]);

}
//this function is calculating a sphere-ray intersection
void calculateSphere(Ray ray){
    
    double implicitSurface = (spheres[1].position[0] - spheres[1].position[0]) + (spheres[1].position[1] - spheres[1].position[1]) +(spheres[1].position[2] - spheres[1].position[2]) - (spheres[1].radius*spheres[1].radius);
    //t0 and t1
    double a,b,c, t0, t1 = 0;
    
    //This calculates a, b, and c for the equation at^2 + bt + c = 0
        a = ray.direction[0]*ray.direction[0] + ray.direction[1]*ray.direction[1] + ray.direction[2]*ray.direction[2];
    
        b = 2*(ray.direction[0] *(ray.startPoint[0] - spheres[1].position[0]) + ray.direction[1]*(ray.startPoint[1] -  spheres[1].position[1]) + ray.direction[2] * (ray.startPoint[2] -  spheres[1].position[2]));
    
        c = (ray.startPoint[0] - spheres[1].position[0])*(ray.startPoint[0] - spheres[1].position[0]) + (ray.startPoint[1] -  spheres[1].position[1])*(ray.startPoint[1] -  spheres[1].position[1])
        + (ray.startPoint[2] -  spheres[1].position[2])*(ray.startPoint[2] -  spheres[1].position[2]) - (spheres[1].radius * spheres[1].radius);
    
    //quadratic function the first part checks to see if b^2 - 4c is negative
    if((b*b - 4*c) < 0)
        cout << "b^2 - 4c is negative!!!" << endl;
    else {
        t0 = (-b + sqrt(b*b - 4*c))/2;
        t1 = (-b - sqrt(b*b - 4*c))/2;
        if((t0 >0) && (t1 > 0))
            cout << "min(" << t0 << "," << t1 << ")" << endl;
    };
    
    //cout << implicitSurface << endl;
    cout << a << b << c << endl;
}


//MODIFY THIS FUNCTION
void draw_scene()
{
  unsigned int x,y;
    int halfWIDTH = WIDTH/2;
    int halfHEIGHT = HEIGHT/2;
    //calculateGridCornors();
    Ray monkeyRay;
    //simple output
    int nX = 0;
    int nY = 0;
  for(x=0; x<WIDTH; x++)
  {
//    nX = x;
//    if(x < halfWIDTH)
//        nX = -x;

    glPointSize(2.0);
    glBegin(GL_POINTS);
    for(y=0;y < HEIGHT;y++)
    {
//        nY = y;
//        if(y < halfHEIGHT)
//            nY = -y;
        //plot_pixel(x,y,x%256,y%256,(x+y)%256);
        monkeyRay = createRay(x, y, 2);
    }
    
    glEnd();
    glFlush();
  }
    
    calculateRay(&monkeyRay, 10);
   // calculateSphere(monkeyRay);
  printf("We're done here!\n");
    fflush(stdout);
}

//converts degrees to radians
double radianConversion(double angle){
    double result;
    result = tan (angle*PI/180);
    printf ("The tangent of %lf degrees is %lf.\n", angle, result );
    return result;
}

//this calculates the grid corners of the scene
void calculateGridCornors()
{
    double x,y,z,a,radianAngle;
    a = WIDTH/HEIGHT;
    radianAngle =  radianConversion(fov);
    
    //the ray that is sent right through the center pixel of the scene
    centerRay.position[0] = 0;
    centerRay.position[1] = 0;
    centerRay.position[2] = -1;
    
    //top left cornor postion of the grid
    x = -a*radianAngle;//tan(radianAngle/2);
    y = radianAngle;//tan(radianAngle/2);
    z = -1;
    topLeftCornor.position[0] = x;
    topLeftCornor.position[1] = y;
    topLeftCornor.position[2] = z;
    cout << x << ", " << y << ", " << z << endl;
    double magnitude = sqrt(topLeftCornor.position[0]*topLeftCornor.position[0] + topLeftCornor.position[1]*topLeftCornor.position[1] + topLeftCornor.position[2]*topLeftCornor.position[2]);
    for(int i =0; i<2; i++)
        topLeftCornor.position[i] = topLeftCornor.position[i]/magnitude;
    // printf ("magnitude: %lf   ray point:  %lf , %lf , %lf .\n", magnitude, topLeftCornor.position[0], topLeftCornor.position[1], topLeftCornor.position[2]);

    
    //top right cornor postion of the grid
    x = a*radianAngle;//tan(radianAngle/2);
    y = radianAngle;//tan(radianAngle/2);
    z = -1;
    topRightCornor.position[0] = x;
    topRightCornor.position[1] = y;
    topRightCornor.position[2] = z;
    cout << x << ", " << y << ", " << z << endl;
    magnitude = sqrt(topRightCornor.position[0]*topRightCornor.position[0] + topRightCornor.position[1]*topRightCornor.position[1] + topRightCornor.position[2]*topRightCornor.position[2]);
    for(int i =0; i<2; i++)
        topRightCornor.position[i] = topRightCornor.position[i]/magnitude;
    // printf ("magnitude: %lf   ray point:  %lf , %lf , %lf .\n", magnitude, topRightCornor.position[0], topRightCornor.position[1], topRightCornor.position[2]);

    
    //bottom left cornor
    x = -a*radianAngle;//tan(radianAngle/2);
    y = -radianAngle;//tan(radianAngle/2);
    z = -1;
    bottomLeftCornor.position[0] = x;
    bottomLeftCornor.position[1] = y;
    bottomLeftCornor.position[2] = z;
    cout << x << ", " << y << ", " << z << endl;
    magnitude = sqrt(bottomLeftCornor.position[0]*bottomLeftCornor.position[0] + bottomLeftCornor.position[1]*bottomLeftCornor.position[1] + bottomLeftCornor.position[2]*bottomLeftCornor.position[2]);
    for(int i =0; i<2; i++)
        bottomLeftCornor.position[i] = bottomLeftCornor.position[i]/magnitude;
    // printf ("magnitude: %lf   ray point:  %lf , %lf , %lf .\n", magnitude, bottomLeftCornor.position[0], bottomLeftCornor.position[1], bottomLeftCornor.position[2]);

    //bottom right cornor
    x = a*radianAngle;//tan(radianAngle/2);
    y = -radianAngle;//tan(radianAngle/2);
    z = -1;
    bottomRightCornor.position[0] = x;
    bottomRightCornor.position[1] = y;
    bottomRightCornor.position[2] = z;
    cout << x << ", " << y << ", " << z << endl;
    magnitude = sqrt(bottomRightCornor.position[0]*bottomRightCornor.position[0] + bottomRightCornor.position[1]*bottomRightCornor.position[1] + bottomRightCornor.position[2]*bottomRightCornor.position[2]);
    for(int i =0; i<2; i++)
        bottomRightCornor.position[i] = bottomRightCornor.position[i]/magnitude;
      // printf ("magnitude: %lf   ray point:  %lf , %lf , %lf .\n", magnitude, bottomRightCornor.position[0], bottomRightCornor.position[1], bottomRightCornor.position[2]);

}

void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
  glColor3f(((double)r)/256.f,((double)g)/256.f,((double)b)/256.f);
    glVertex2i(x,y);
//    cout << x  << " " << y << endl;
}

void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
  buffer[HEIGHT-y-1][x][0]=r;
  buffer[HEIGHT-y-1][x][1]=g;
  buffer[HEIGHT-y-1][x][2]=b;
}

void plot_pixel(int x,int y,unsigned char r,unsigned char g, unsigned char b)
{
//    cout << x << " " << y << endl;
  plot_pixel_display(x,y,r,g,b);
  if(mode == MODE_JPEG)
      plot_pixel_jpeg(x,y,r,g,b);
}

void save_jpg()
{
  Pic *in = NULL;

  in = pic_alloc(640, 480, 3, NULL);
  printf("Saving JPEG file: %s\n", filename);

  memcpy(in->pix,buffer,3*WIDTH*HEIGHT);
  if (jpeg_write(filename, in))
    printf("File saved Successfully\n");
  else
    printf("Error in Saving\n");

  pic_free(in);      

}

void parse_check(char *expected,char *found)
{
  if(strcasecmp(expected,found))
    {
      char error[100];
      printf("Expected '%s ' found '%s '\n",expected,found);
      printf("Parse error, abnormal abortion\n");
      exit(0);
    }

}

void parse_doubles(FILE*file, char *check, double p[3])
{
  char str[100];
  fscanf(file,"%s",str);
  parse_check(check,str);
  fscanf(file,"%lf %lf %lf",&p[0],&p[1],&p[2]);
  printf("%s %lf %lf %lf\n",check,p[0],p[1],p[2]);
}

void parse_rad(FILE*file,double *r)
{
  char str[100];
  fscanf(file,"%s",str);
  parse_check("rad:",str);
  fscanf(file,"%lf",r);
  printf("rad: %f\n",*r);
}

void parse_shi(FILE*file,double *shi)
{
  char s[100];
  fscanf(file,"%s",s);
  parse_check("shi:",s);
  fscanf(file,"%lf",shi);
  printf("shi: %f\n",*shi);
}


int loadScene(char *argv)
{
  FILE *file = fopen(argv,"r");
  int number_of_objects;
  char type[50];
  int i;
  Triangle t;
  Sphere s;
  Light l;
  fscanf(file,"%i",&number_of_objects);

  printf("number of objects: %i\n",number_of_objects);
  char str[200];

  parse_doubles(file,"amb:",ambient_light);

  for(i=0;i < number_of_objects;i++)
    {
      fscanf(file,"%s\n",type);
      printf("%s\n",type);
      if(strcasecmp(type,"triangle")==0)
	{

	  printf("found triangle\n");
	  int j;

	  for(j=0;j < 3;j++)
	    {
	      parse_doubles(file,"pos:",t.v[j].position);
	      parse_doubles(file,"nor:",t.v[j].normal);
	      parse_doubles(file,"dif:",t.v[j].color_diffuse);
	      parse_doubles(file,"spe:",t.v[j].color_specular);
	      parse_shi(file,&t.v[j].shininess);
	    }

	  if(num_triangles == MAX_TRIANGLES)
	    {
	      printf("too many triangles, you should increase MAX_TRIANGLES!\n");
	      exit(0);
	    }
	  triangles[num_triangles++] = t;
	}
      else if(strcasecmp(type,"sphere")==0)
	{
	  printf("found sphere\n");

	  parse_doubles(file,"pos:",s.position);
	  parse_rad(file,&s.radius);
	  parse_doubles(file,"dif:",s.color_diffuse);
	  parse_doubles(file,"spe:",s.color_specular);
	  parse_shi(file,&s.shininess);

	  if(num_spheres == MAX_SPHERES)
	    {
	      printf("too many spheres, you should increase MAX_SPHERES!\n");
	      exit(0);
	    }
	  spheres[num_spheres++] = s;
	}
      else if(strcasecmp(type,"light")==0)
	{
	  printf("found light\n");
	  parse_doubles(file,"pos:",l.position);
	  parse_doubles(file,"col:",l.color);

	  if(num_lights == MAX_LIGHTS)
	    {
	      printf("too many lights, you should increase MAX_LIGHTS!\n");
	      exit(0);
	    }
	  lights[num_lights++] = l;
	}
      else
	{
	  printf("unknown type in scene description:\n%s\n",type);
	  exit(0);
	}
    }
  return 0;
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
//
//    glLoadIdentity();
//    caculatePixelGrid();
//    glutSwapBuffers();
}

void init()
{
  glMatrixMode(GL_PROJECTION);
  glOrtho(0,WIDTH,0,HEIGHT,1,-1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void idle()
{
  //hack to make it only draw once
  static int once=0;
  if(!once)
  {
      draw_scene();
      if(mode == MODE_JPEG)
	save_jpg();
    }
  once=1;
}

int main (int argc, char ** argv)
{
  if (argc<2 || argc > 3)
  {  
    printf ("usage: %s <scenefile> [jpegname]\n", argv[0]);
    exit(0);
  }
  if(argc == 3)
    {
      mode = MODE_JPEG;
      filename = argv[2];
    }
  else if(argc == 2)
    mode = MODE_DISPLAY;

  glutInit(&argc,argv);
  loadScene(argv[1]);

  glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
  glutInitWindowPosition(0,0);
  glutInitWindowSize(WIDTH,HEIGHT);
  int window = glutCreateWindow("Ray Tracer");
  glutDisplayFunc(display);
  glutIdleFunc(idle);
//    caculatePixelGrid();
    
  init();
  glutMainLoop();
}
