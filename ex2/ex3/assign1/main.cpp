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
#define NEARZERO 0.00001


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

void calculateCROSS(double determinantVAL[3], double vec1[3], double vec2[3]){
    // double outputVec[3] = 0;
    
    determinantVAL[0] = (vec1[1]*vec2[2]) - (vec1[2]*vec2[1]);
    determinantVAL[1] = (vec1[2]*vec2[0]) - (vec1[0]*vec2[2]);
    determinantVAL[2] = (vec1[0]*vec2[1]) - (vec1[1]*vec2[0]);
}

double calculateDOT(double vec1[3], double vec2[3]){
  double tempOut = vec1[0]*vec2[0] + vec1[1]*vec2[1] + vec1[2]*vec2[2];
  return tempOut;
}

//converts angles in degrees to radians
double radinConversion(double angle){
    double result = (angle/2)*(PI/180);
    return result;
}
//calculates the corners of the 3 scene
void calculateSceneDeminsions(){
    double aspect = WIDTH/HEIGHT;
    //xLeft = -aspect*tan(radinConversion(fov));
    xLeft = -aspect*tan((fov/2 )*(PI/180));
    xRight = aspect*tan((fov/2 )*(PI/180));
    yBottom = -tan((fov/2 )*(PI/180));
    yTop = tan((fov/2 )*(PI/180));
    
//    cout << "left x: " << xLeft << " right x: " << xRight << endl;
//    cout << "bottom y: " << yBottom << " top y: " << yTop << endl;

}
//creates a grid of pixels
void createPixelGrid(){
    calculateSceneDeminsions();
    double pixelCenterX, pixelCenterY, magnitude, xNormalVec, yNormalVec, zNormalVec, pixelWidth, pixelHeight =0;
     pixelWidth = (abs(xLeft) + abs(xRight))/WIDTH;
     pixelHeight = (abs(yBottom) + abs(yTop))/HEIGHT;
//    cout << " pixel width: " << pixelWidth << " pixel height: " << pixelHeight << endl;

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

//This will check to see if any of the rays intersects with the sphere in the scene
void calculateSphereIntersection(int x, int y){

    double a,b,c,t0,t1 = 0.0;

    a = (rayDirectionArray[x][y][0] * rayDirectionArray[x][y][0]) + (rayDirectionArray[x][y][1]*rayDirectionArray[x][y][1]) + (rayDirectionArray[x][y][2]*rayDirectionArray[x][y][2]);
   
    b = 2*(rayDirectionArray[x][y][0]*(origin + spheres[0].position[0]) + rayDirectionArray[x][y][1]*(origin + spheres[0].position[1]) + rayDirectionArray[x][y][2]*(origin + spheres[0].position[2]));
    
    c = (origin + spheres[0].position[0])*(origin + spheres[0].position[0]) + (origin + spheres[0].position[1])*(origin + spheres[0].position[1]) + (origin + spheres[0].position[2])*(origin + spheres[0].position[2]) - (spheres[0].radius*spheres[0].radius);
//    cout << a << " " << b << "   "  << c << endl;
    if((b*b - 4*c) >0){
        t0 = c;
        t1 = -b + sqrt((b*b - 4*c)/2);
        if(t1 < t0){
            plot_pixel(x,y,x%256,y%256,(x+y)%256);
        }
        else if(t1 == t0){
            plot_pixel(x,y,x%256,y%256,(x+y)%256);
        }
    }

}

void calculateTriangleIntersection(int x, int y)
{
//    double t =0;
    //T is the t vector
    double p[3], q[3], T[3], t, u, v, determinant, inverse_determinant, tiOutput, area, Alpha, Beta, Gamma = 0; // those are the soon to be computed normal vector points for the plane in which the triangle is located
    double A[3], B[3], C[3], edge1[3], edge2[3], Normal[3], triangleArea[3], planeVector[3];
        for(int i = 0;i<3;i++)
            A[i] = triangles[0].v[0].position[i];
        for(int j = 0;j<3;j++)
            B[j] = triangles[0].v[1].position[j];
        for(int k =0; k<3;k++)
            C[k] = triangles[0].v[2].position[k];
        for(int l = 0; l<3; l++){
            edge1[l] = (A[l] - B[l]);
            edge2[l] = (A[l] - C[l]);
        }
    
    //to find the determinant we must find the cross product

    calculateCROSS(&planeVector[3], &rayDirectionArray[x][y][3], &edge2[3]);

    //now we compute the dot product, and if the determinant is ~ 0 then it lies within the triangle
    determinant = calculateDOT(&edge1[3], &planeVector[3]);
    if(determinant > -NEARZERO && determinant < NEARZERO){
        cout << " the ray is not inside the triangle" << endl;
        return;
    }
    else
        inverse_determinant = 1/determinant;
    
    
    //find distance from ray origin to A vector
    for(int i = 0; i<3; i++)//tests for CULLING
        T[i] = origin - A[i];
    
    //find U by calculating the dot prouct and multiplying it by the inverse determinant
    u = calculateDOT(&T[3], &planeVector[3]) * inverse_determinant;
    //test u value to see if the intersection is not within the triangle
    
    if(u < 0 || u >1){
        return;
    }
    //This is how to comput the v vector
    calculateCROSS(&q[3], &T[3], &edge1[3]);
        
    v = calculateDOT(&rayDirectionArray[x][y][3], &q[3]) * inverse_determinant;
   
    if(v < 0 || u +v > 1){
        return;
    }
        
    t = calculateDOT(edge2, &q[3]) * inverse_determinant;
        
        
    if(t > NEARZERO){
        tiOutput = t;
        cout << t << endl;
    }
    
//    double mag = sqrt(planeVector[0]*planeVector[0] + planeVector[1]*planeVector[1] + planeVector[2]*planeVector[2]);
////    double firstVecMagnitude = sqrt(edge1[0]*edge1[0] + edge1[1]*edge1[1] + edge1[2]*edge1[2]);
////    double secondVecMagnitude = sqrt(edge2[0]*edge2[0] + edge2[1]*edge2[1] + edge2[2]*edge2[2]);
//
//    if(mag > 0){
//        for(int u = 0;u<3; u++)
//            Normal[u] = planeVector[u]/mag;
////        cout << A[0] << " " << A[1] << " " << A[2] << endl;
////        cout << B[0] << " " << B[1] << " " << B[2] << endl;
////        cout << edge1[0] << " " << edge1[1] << " " << edge1[2] << endl;
////        cout << C[0] << " " << C[1] << " " << C[2] << endl;
////        cout << edge2[0] << " " << edge2[1] << " " << edge2[2] << endl;
////
////        cout << Normal[0] << " " << Normal[1] << " " << Normal[2] << endl;
//        
//    }
//    else
//        cout << "naw" << endl;
////
////       cout << Normal[0] << " " << Normal[1] << " " << Normal[2] << endl;
//    //Find the normal vector to the plane
//    double implicit = Normal[0]*(origin + rayDirectionArray[x][y][0]*t) + Normal[1]*(origin  + rayDirectionArray[x][y][1]*t) + Normal[2]*(origin  + rayDirectionArray[x][y][2]*t);
//    t = -(Normal[0]*origin + Normal[1]*origin + Normal[2]*origin + 1)/(Normal[0]*rayDirectionArray[x][y][0] + Normal[1]*rayDirectionArray[x][y][1] + Normal[2]*rayDirectionArray[x][y][2]);
//    
////    cout << t << endl;
//    
//       triangleArea[0] = abs((edge1[1]*edge2[2]) - (edge1[2]*edge2[1]))/2;
//       triangleArea[1] = abs((edge1[2]*edge2[0]) - (edge1[0]*edge2[2]))/2;
//       triangleArea[2] = abs((edge1[0]*edge2[1]) - (edge1[1]*edge2[0]))/2;
//    
//    //Normal[0] = lengthX;///magnitude;
////    Normal[1] = lengthY;///magnitude;
////    Normal[2] = lengthZ;///magnitude;
//   
//    cout << triangleArea[0] << " " << triangleArea[1] << " " << triangleArea[2] << endl;
    
//    for(int g = 0;g<3;g++)
//        cout << A[g] << " " << B[g] << " " << C[g] << endl;
//    cout << area << endl;
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
        calculateSphereIntersection(x,y);
        calculateTriangleIntersection(x, y);

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

void init(){
  glMatrixMode(GL_PROJECTION);
  glOrtho(0,WIDTH,0,HEIGHT,1,-1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void idle(){
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

int main (int argc, char ** argv){
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
