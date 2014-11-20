/*
CSCI 480
Assignment 3 Raytracer

Name: <Your name here>
*/

#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <pic.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <cmath>
#include <utility>

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
#define WIDTH 320
#define HEIGHT 240

//the field of view of the camera
#define fov 60.0
#define PI 3.14159265


unsigned char buffer[HEIGHT][WIDTH][3];
double pixLocationArray[WIDTH][HEIGHT][2];
//tuple<double, double, double> camToPixVectorArray[WIDTH][HEIGHT];
double rayDirectionArray[WIDTH][HEIGHT][3];//camera To Pixel Vector
double zN = -1; //This is zNear
double xLeft, xRight, yBottom, yTop, xCenter, yCenter = 0;
double rayArray[WIDTH][HEIGHT];
  double origin = 0.0;

struct Ray
{
    double origin[3];
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

Triangle triangles[MAX_TRIANGLES];
Sphere spheres[MAX_SPHERES];
Light lights[MAX_LIGHTS];
double ambient_light[3];

int num_triangles=0;
int num_spheres=0;
int num_lights=0;

void calculateSceneDeminsions();
void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel(int x,int y,unsigned char r,unsigned char g,unsigned char b);


//converts angles in degrees to radians
double radinConversion(double angle){
    double result = (angle/2)*(PI/180);
    return result;
}
//calculates the corners of the 3 scene
void calculateSceneDeminsions(){
    double aspect = WIDTH/HEIGHT;
    //xLeft = -aspect*tan(radinConversion(fov));
    xLeft = -aspect*tan((fov)*(PI/180));
    xRight = aspect*tan((fov)*(PI/180));
    yBottom = -tan((fov)*(PI/180));
    yTop = tan((fov)*(PI/180));
    
    cout << "left x: " << xLeft << " right x: " << xRight << endl;
    cout << "bottom y: " << yBottom << " top y: " << yTop << endl;

}
//creates a grid of pixels
void createPixelGrid(){
    calculateSceneDeminsions();
    double pixelCenterX, pixelCenterY, magnitude, xNormalVec, yNormalVec, zNormalVec, pixelWidth, pixelHeight =0;
     pixelWidth = (abs(xLeft) + abs(xRight))/WIDTH;
     pixelHeight = (abs(yBottom) + abs(yTop))/HEIGHT;
    cout << " pixel width: " << pixelWidth << " pixel height: " << pixelHeight << endl;

    for(int i =0;i<WIDTH;i++){
        for (int j = 0; j<HEIGHT; j++) {
            
            pixelCenterX = xLeft + (i * pixelWidth) + (pixelWidth/2);
            pixelCenterY = yBottom+ (j * pixelHeight) + (pixelHeight/2);
            //cout << " pixel x center: " << pixelCenterX  << " pixel y center: " << pixelCenterY  << endl;

            pixLocationArray[i][j][0] = pixelCenterX;
            pixLocationArray[i][j][1] = pixelCenterY;
            //cout << pixLocationArray[i][j].first << "," << pixLocationArray[i][j].second << endl; // << " | " << pixVectorArray[x]
            magnitude = sqrt(pixelCenterX * pixelCenterX + pixelCenterY * pixelCenterY + zN*zN);
            
            xNormalVec = pixelCenterX/magnitude;
            yNormalVec = pixelCenterY/magnitude;
            zNormalVec = zN/magnitude;
            //pixVector
            //rayDirectionArray[i][j] = make_tuple(xNormalVec,yNormalVec,zNormalVec);
            rayDirectionArray[i][j][0] = xNormalVec;
            rayDirectionArray[i][j][1] = yNormalVec;
            rayDirectionArray[i][j][2] = zNormalVec;
            //cout << rayDirectionArray[i][j][0] << endl;
        }
    }
}

//p(t) funciton for the ray t is the t value, and the x and y are the x and y pixel positions on the screen
void generateRays(double t,int xPixel,int yPixel){
    double p[3];
  
    cout << rayDirectionArray[xPixel][yPixel][0] << endl;
    for(int k = 0; k<3; k++){
        p[k] = origin + rayDirectionArray[xPixel][yPixel][k]*t;
        cout << p[k] << "   ";
    }
    cout << endl;
}


void calculateSphereIntersection(int x, int y){
    
    double sphereCenter[3] = {spheres[0].position[0], spheres[0].position[1], spheres[0].position[2]};
//    cout << sphereCenter[2] << endl;
    double a,b,c,t0,t1 = 0.0;

    a = (rayDirectionArray[x][y][0] * rayDirectionArray[x][y][0]) + (rayDirectionArray[x][y][1]*rayDirectionArray[x][y][1]) + (rayDirectionArray[x][y][2]*rayDirectionArray[x][y][2]);
   
    b = 2*(rayDirectionArray[x][y][0]*(origin + spheres[0].position[0]) + rayDirectionArray[x][y][1]*(origin + spheres[0].position[1]) + rayDirectionArray[x][y][2]*(origin + spheres[0].position[2]));
    
    c = (origin + spheres[0].position[0])*(origin + spheres[0].position[0]) + (origin + spheres[0].position[1])*(origin + spheres[0].position[1]) + (origin + spheres[0].position[2])*(origin + spheres[0].position[2]) - (spheres[0].radius*spheres[0].radius);
//    cout << a << " " << b << "   "  << c << endl;
    if((b*b - 4*c) >0){
        t0 = c;
        t1 = -b + sqrt((b*b - 4*c)/2);
        //cout  << t0 << "    " << t1 << endl;
        if(t1 < t0){
//            cout << "sphere hit!" << endl;
            plot_pixel(x,y,x%256,y%256,(x+y)%256);
        }
        else if(t1 == t0){
//            cout << "sphere grazed" << endl;
            plot_pixel(x,y,x%256,y%256,(x+y)%256);
        }
//        else
//            cout << "sphere missed" << endl;
    }
//    else
        //cout << "The discriminant is negative!!! :(" << endl;
}


//MODIFY THIS FUNCTION
void draw_scene()
{
  unsigned int x,y;
    double xPos, yPos = 0;
    createPixelGrid();
    //generateRays(1, 319,239);
    
    
  //simple output
  for(x=0; x<WIDTH; x++)
  {
    glPointSize(2.0);  
    glBegin(GL_POINTS);
    for(y=0;y < HEIGHT;y++)
    {
//        double radAngle = radinConversion(fov);
//        cout << radAngle << endl;
//        xPos = (HEIGHT/WIDTH)*tan(radAngle/2);
//        yPos = tan(radAngle/2);
        calculateSphereIntersection(x,y);
        
    }
    glEnd();
    glFlush();
  }
  printf("Done!\n"); fflush(stdout);
}

void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
  glColor3f(((double)r)/256.f,((double)g)/256.f,((double)b)/256.f);
  glVertex2i(x,y);
}

void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
  buffer[HEIGHT-y-1][x][0]=r;
  buffer[HEIGHT-y-1][x][1]=g;
  buffer[HEIGHT-y-1][x][2]=b;
}

void plot_pixel(int x,int y,unsigned char r,unsigned char g, unsigned char b)
{
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
  init();
  glutMainLoop();
}
