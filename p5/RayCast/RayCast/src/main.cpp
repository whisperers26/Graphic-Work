#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>


#include "Camera.h"

#include "Sphere.h"
#include "Box.h"
#include "Light.h"

#include "Text.h"

#include <fstream>
#include <iostream>

#define PI 2*std::acos(0)


const unsigned int CAMERA = 0;
const unsigned int LIGHT = 1;
const unsigned int SPHERE = 2;
const unsigned int BOX = 3;

using namespace std;
using namespace glm;

int g_winWidth  = 640;
int g_winHeight = 480;

Camera g_cam;
Text g_text;
unsigned char g_keyStates[256];

vec4 DirLeft = vec4(-1.0f, 0.0f, 0.0f, 0.0f);
vec4 DirRight = vec4(1.0f, 0.0f, 0.0f, 0.0f);
vec4 DirUp = vec4(0.0f, 1.0f, 0.0f, 0.0f);
vec4 DirDown = vec4(0.0f, -1.0f, 0.0f, 0.0f);
vec4 DirForward = vec4(0.0f, 0.0f, 1.0f, 0.0f);
vec4 DirBackward = vec4(0.0f, 0.0f, -1.0f, 0.0f);



const char dataFile[128] = "geoData/geo.txt";

unsigned int g_box_num;
Box* g_boxes;

unsigned int g_sphere_num;
Sphere * g_spheres;



// camera related
vec3 up;
float viewXL, viewXR, viewYL, viewYR; // viewport
float  ax, ay, az, bx, by, bz, cx, cy, cz;
float viewAngle, viewTan, xIntV, yIntV;

// light related
Light g_light;

// screen related
float* pixelR;
float* pixelG;
float* pixelB;

void Normalize(float* x, float* y, float* z)
{
    float  norm;
    norm = std::sqrt(*x * *x + *y * *y + *z * *z);
    if (norm != 0.0) {
        *x = *x / norm;
        *y = *y / norm;
        *z = *z / norm;
    }
}

// transformation to perspective
void Convert_M()
{
    cx = g_cam.lookat.x - g_cam.eye.x;
    cy = g_cam.lookat.y - g_cam.eye.y;
    cz = g_cam.lookat.z - g_cam.eye.z;
    Normalize(&cx, &cy, &cz);
    ax = cy * up.z - up.y * cz;
    ay = up.x * cz - cx * up.z;
    az = cx * up.y - up.x * cy;
    Normalize(&ax, &ay, &az);
    bx = ay * cz - cy * az;
    by = cx * az - ax * cz;
    bz = ax * cy - cx * ay;
}


// find the intersection point
void FindIntersection(vec3 cameye, float dx, float dy, float dz, float interval, vec3* intersectPoint)
{
    intersectPoint->x = cameye.x + dx * interval;
    intersectPoint->y = cameye.y + dy * interval;
    intersectPoint->z = cameye.z + dz * interval;
}

// sphere ray intersection
void Sphere_Ray(vec3 rayPos, float dx, float dy, float dz, vec3 spherePos, float radius, float* t1, float* t2) 
{
    float a, b, c, delta, xdiff, ydiff, zdiff;
    xdiff = rayPos.x - spherePos.x;
    ydiff = rayPos.y - spherePos.y;
    zdiff = rayPos.z - spherePos.z;
    a= xdiff * xdiff + ydiff * ydiff + zdiff * zdiff - radius * radius;
    b = 2.0f * (dx * xdiff + dy * ydiff + dz * zdiff);
    c = dx * dx + dy * dy + dz * dz;
    delta = b * b - 4.0f * a * c;
    if (delta < 0.0f) {
        *t1 = -1.0f;
        *t2 = -1.0f;
    }
    else if (delta == 0.0f) {
        *t1 = -b / (2.0 * c);
        *t2 = -1.0f;
    }
    else {
        delta = std::sqrt(delta);
        *t1 = (-b + delta) / (2.0 * c);
        *t2 = (-b - delta) / (2.0 * c);
    }
}

// box ray intersection (OBB)
vec4 Box_Ray(vec3 rayPos, float dx, float dy, float dz, vec3 cor_obb_min, vec3 cor_obb_max, mat4 rotationMatrix, float* t)
{
    float t_min = 0.0f;
    float t_max = 10000.0f;
    vec3 corTranslate = (cor_obb_min + cor_obb_max) / 2.0f;
    mat4 corTranslateMat = translate(mat4(), corTranslate);


    vec3 aabb_min = cor_obb_min - corTranslate;
    vec3 aabb_max = cor_obb_max - corTranslate;
    mat4 transformMatrix = inverse(inverse(corTranslateMat) * inverse(rotationMatrix));



    vec3 boxPosiiton(transformMatrix[3].x, transformMatrix[3].y, transformMatrix[3].z);

    vec3 diff = boxPosiiton - rayPos;
    vec3 lightDirection(dx, dy, dz);

    vec4 nullaxis(0, 0, 0, -1);
    vec4 itsaxis_min;

    // left right intersection (xaxis)
    {
        vec4 xaxis(transformMatrix[0].x, transformMatrix[0].y, transformMatrix[0].z, 0.0f);
        vec4 itsaxis1, itsaxis2;
        vec3 xaxis3 = xaxis.xyz;
        float e = dot(xaxis3, diff);
        float f = dot(lightDirection, xaxis3);
        if (fabs(f) > 0.00000001f) {
            float t1 = (e + aabb_min.x) / f; itsaxis1 = vec4(-xaxis.x, -xaxis.y, -xaxis.z, 0.0f); //left
            float t2 = (e + aabb_max.x) / f; itsaxis2 = vec4(xaxis.x, xaxis.y, xaxis.z, 0.0f); //right
            if (t1 > t2) {
                // keep t2 the biggest
                float w = t1; t1 = t2; t2 = w;
                vec4 itsaxistmp = itsaxis1; itsaxis1 = itsaxis2; itsaxis2 = itsaxistmp;
            }
            if (t2 < t_max) t_max = t2;
            if (t1 > t_min) {
                t_min = t1;
                itsaxis_min = itsaxis1;
            }
            if (t_max < t_min) {
                *t = -1.0f;
                return nullaxis;
            } 
        }
        else {
            if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f) {
                *t = -1.0f;
                return nullaxis;
            }
        }
    }
    

    // up and down (yaxis)
    {
        vec4 yaxis(transformMatrix[1].x, transformMatrix[1].y, transformMatrix[1].z, 0.0f);
        vec4 itsaxis1, itsaxis2;
        vec3 yaxis3 = yaxis.xyz;
        float e = dot(yaxis3, diff);
        float f = dot(lightDirection, yaxis3);
        if (fabs(f) > 0.00000001f) {
            float t1 = (e + aabb_min.y) / f; itsaxis1 = vec4(-yaxis.x, -yaxis.y, -yaxis.z, 0.0f); //down
            float t2 = (e + aabb_max.y) / f; itsaxis2 = vec4(yaxis.x, yaxis.y, yaxis.z, 0.0f); //up
            if (t1 > t2) {
                // keep t2 the biggest
                float w = t1; t1 = t2; t2 = w;
                vec4 itsaxistmp = itsaxis1; itsaxis1 = itsaxis2; itsaxis2 = itsaxistmp;
            }
            if (t2 < t_max) t_max = t2;
            if (t1 > t_min) {
                t_min = t1;
                itsaxis_min = itsaxis1;
            }
            if (t_max < t_min) { 
                *t = -1.0f;
                return nullaxis; 
            }
        }
        else {
            if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f) {
                *t = -1.0f;
                return nullaxis;
            }
        }
    }

    // back and front (zaxis)
    {
        vec4 zaxis(transformMatrix[2].x, transformMatrix[2].y, transformMatrix[2].z, 0.0f);
        vec4 itsaxis1, itsaxis2;
        vec3 zaxis3 = zaxis.xyz;
        float e = dot(zaxis3, diff);
        float f = dot(lightDirection, zaxis3);
        if (fabs(f) > 0.00000001f) {
            float t1 = (e + aabb_min.z) / f; itsaxis1 = vec4(-zaxis.x, -zaxis.y, -zaxis.z, 0.0f); //back
            float t2 = (e + aabb_max.z) / f; itsaxis2 = vec4(zaxis.x, zaxis.y, zaxis.z, 0.0f); //front
            if (t1 > t2) {
                // keep t2 the biggest
                float w = t1; t1 = t2; t2 = w;
                vec4 itsaxistmp = itsaxis1; itsaxis1 = itsaxis2; itsaxis2 = itsaxistmp;
            }
            if (t2 < t_max) t_max = t2;
            if (t1 > t_min) {
                t_min = t1;
                itsaxis_min = itsaxis1;
            }
            if (t_max < t_min) { 
                *t = -1.0f;
                return nullaxis; 
            }
        }
        else {
            if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f) {
                *t = -1.0f;
                return nullaxis;
            }
        }
    }

    *t = t_min;
    return itsaxis_min;
}

// shadow detection
int DetectShadow(vec3 pos)
{
    float t0, t1, t2;
    for (int i = 0; i < g_sphere_num; i++) {
        Sphere_Ray(pos, g_light.pos.x, g_light.pos.y, g_light.pos.z, g_spheres[i].pos, g_spheres[i].radius, &t1, &t2);
        if (t1 >= 0.001 || t2 >= 0.001) return 1;
    }
    for (int i = 0; i < g_box_num; i++) {
        Box_Ray(pos, g_light.pos.x, g_light.pos.y, g_light.pos.z, g_boxes[i].minPos, g_boxes[i].maxPos, g_boxes[i].rotMat, &t0);
        if (t0 >= 0.001) return 1;
    }
    return 0;
}

// compute color in specific 3d position
void ComputeColor(vec3 pos, float dx, float dy, float dz, vec3 ambColor, vec3 diffColor, vec3 phongColor, int powValue, int shadowTag, float *r, float *g, float *b)
{
    float  vx, vy, vz, rx, ry, rz;
    // first view vector
    vx = g_cam.eye.x - pos.x;
    vy = g_cam.eye.y - pos.y;
    vz = g_cam.eye.z - pos.z;
    Normalize(&vx, &vy, &vz);
    // reflection
    float dldot = dx * g_light.pos.x + dy * g_light.pos.y + dz * g_light.pos.z;
    rx = 2.0f * dldot * dx - g_light.pos.x;
    ry = 2.0f * dldot * dy - g_light.pos.y;
    rz = 2.0f * dldot * dz - g_light.pos.z;
    float vrdot = vx * rx + vy * ry + vz * rz;

    *r = ambColor.r;
    *g = ambColor.g;
    *b = ambColor.b;
    float powY;
    if (shadowTag <= 0 && dldot >= 0.0f) {
        *r = *r + diffColor.r * dldot * g_light.intensity;
        *g = *g + diffColor.g * dldot * g_light.intensity;
        *b = *b + diffColor.b * dldot * g_light.intensity;
        if (vrdot >= 0.0f) {
            powY = pow(vrdot, powValue);
            *r = *r + phongColor.r * powY * g_light.intensity;
            *g = *g + phongColor.g * powY * g_light.intensity;
            *b = *b + phongColor.b * powY * g_light.intensity;
        }
    }

    if (*r > 1.0) *r = 1.0;
    if (*g > 1.0) *g = 1.0;
    if (*b > 1.0) *b = 1.0;
}

// return pixel color
vec3 RayCast(vec3 cameye, float dx, float dy, float dz, int objType, int objInd)
{
    float t0, t1, t2, t_min, nx, ny, nz, r, g, b;
    vec3 itsPos, color;
    vec4 boxDir;
    int itsObjType, itsObjInd, shadowTag;

    itsPos.x = cameye.x;
    itsPos.y = cameye.y;
    itsPos.z = cameye.z;
    t_min = 10000.0f;
    t0 = -1.0f;
    t1 = -1.0f;
    t2 = -1.0f;
    shadowTag = 0;
    itsObjType = CAMERA;
    itsObjInd = 0;

    // sphere ray
    for (int i = 0; i < g_sphere_num; i++) {
        if (objType == SPHERE && objInd == i) continue;
        Sphere_Ray(cameye, dx, dy, dz, g_spheres[i].pos, g_spheres[i].radius, &t1, &t2);
        if (t1 >= 0.0f && t1 < t_min) {
            t_min = t1;
            itsObjType = SPHERE;
            itsObjInd = i;
            FindIntersection(cameye, dx, dy, dz, t1, &itsPos);
        }
        if (t2 >= 0.0f && t2 < t_min) {
            t_min = t2;
            itsObjType = SPHERE;
            itsObjInd = i;
            FindIntersection(cameye, dx, dy, dz, t2, &itsPos);
        }
    }

    //box ray
    for (int i = 0; i < g_box_num; i++) {
        if (objType == BOX && objInd == i) continue;
        Box_Ray(cameye, dx, dy, dz, g_boxes[i].minPos, g_boxes[i].maxPos, g_boxes[i].rotMat, &t0);
        if (t0 >= 0.0f && t0 < t_min) {
            t_min = t0;
            itsObjType = BOX;
            itsObjInd = i;
            boxDir = Box_Ray(cameye, dx, dy, dz, g_boxes[i].minPos, g_boxes[i].maxPos, g_boxes[i].rotMat, &t0);
            FindIntersection(cameye, dx, dy, dz, t0, &itsPos);
        }
    }

    // near plane
    if (fabs(cameye.x - itsPos.x) < 0.001 && fabs(cameye.y - itsPos.y) < 0.001 && fabs(cameye.z - itsPos.z) < 0.001) itsObjType = CAMERA;

    // compute the color of the pixel
    if (itsObjType <= CAMERA) {
        r = 0.0f;
        g = 0.0f;
        b = 0.0f;
    }
    // calculate normolized vector of plane
    else if (itsObjType == SPHERE) {
        nx = itsPos.x - g_spheres[itsObjInd].pos.x;
        ny = itsPos.y - g_spheres[itsObjInd].pos.y;
        nz = itsPos.z - g_spheres[itsObjInd].pos.z;
        Normalize(&nx, &ny, &nz);
        shadowTag = DetectShadow(itsPos);
        vec3 phongvec;
        phongvec.x = g_spheres[itsObjInd].phong;
        phongvec.y = g_spheres[itsObjInd].phong;
        phongvec.z = g_spheres[itsObjInd].phong;
        ComputeColor(itsPos, nx, ny, nz, g_spheres[itsObjInd].color * g_spheres[itsObjInd].ambient, g_spheres[itsObjInd].color * g_spheres[itsObjInd].diffuse,
            phongvec, 50, shadowTag, &r, &g, &b);
    }
    else if (itsObjType == BOX) {
        nx = boxDir.x;
        ny = boxDir.y;
        nz = boxDir.z;
        Normalize(&nx, &ny, &nz);
        shadowTag = DetectShadow(itsPos);
        vec3 phongvec;
        phongvec.x = g_boxes[itsObjInd].phong;
        phongvec.y = g_boxes[itsObjInd].phong;
        phongvec.z = g_boxes[itsObjInd].phong;
        ComputeColor(itsPos, nx, ny, nz, g_boxes[itsObjInd].color * g_boxes[itsObjInd].ambient, g_boxes[itsObjInd].color * g_boxes[itsObjInd].diffuse,
            phongvec, 50, shadowTag, &r, &g, &b);
    }

    vec3 outputColor;
    outputColor.r = r;
    outputColor.g = g;
    outputColor.b = b;
    return outputColor;
}

// generate rays
void GenerateRays() 
{
    float u, v, vx, vy, dx, dy, dz;
    vec3 outputColor;
    for (int i = 0; i < g_winWidth; i++) {
        for (int j = 0; j < g_winHeight; j++) {
            v= (float)j / g_winHeight;
            //view port
            vx = viewXL + i * xIntV;
            vy = viewYL + j * yIntV;
            dx = ax * vx * viewTan + bx * vy * viewTan + cx;
            dy = ay * vx * viewTan + by * vy * viewTan + cy;
            dz = az * vx * viewTan + bz * vy * viewTan + cz;
            outputColor = RayCast(g_cam.eye.xyz, dx, dy, dz, -1, -1);
            pixelR[i + g_winWidth * j] = outputColor.r;
            pixelG[i + g_winWidth * j] = outputColor.g;
            pixelB[i + g_winWidth * j] = outputColor.b;
        }
    }
}



void LoadConfigFile(const char* pFilePath)
{
    
    
    vec3 rot;
    fstream filestr;
    filestr.open (pFilePath, fstream::in);
    if(!filestr.is_open()){
        cout<<"'"<<pFilePath<<"'"<<", geo file does not exsit!"<<endl;
        filestr.close();
        return;
    }
    
    char objType[80];
    char attrType[80];
    unsigned int objState = -1;
    
    bool loop = true;
    while(loop){
        filestr>>objType;
        if(!strcmp(objType, "Camera:")) objState=CAMERA;
        else if(!strcmp(objType, "Light:")) objState = LIGHT;
        else if(!strcmp(objType, "Sphere:")) objState = SPHERE;
        else if(!strcmp(objType, "Box:")) objState = BOX;
        switch(objState){
            case CAMERA:
                float e_x, e_y, e_z;
                float l_x, l_y, l_z;
                float near_plane;
                
                for(int i=0; i<3; i++){
                    filestr>>attrType;
                    if(!strcmp(attrType, "eye:")){
                        filestr>>e_x;
                        filestr>>e_y;
                        filestr>>e_z;
                    }
                    else if(!strcmp(attrType, "lookat:")){
                        filestr>>l_x;
                        filestr>>l_y;
                        filestr>>l_z;
                    }
                    else if(!strcmp(attrType, "near_plane:")) filestr>>near_plane;
                }
                g_cam.set(e_x, e_y, e_z, l_x, l_y, l_z, g_winWidth, g_winHeight, 45.0f, near_plane, 1000.0f);
                break;
            case LIGHT:
                filestr>>attrType;
                if(!strcmp(attrType, "position:")){
                    filestr>>g_light.pos.x;
                    filestr>>g_light.pos.y;
                    filestr>>g_light.pos.z;
                }
                filestr>>attrType;
                if(!strcmp(attrType, "color:")){
                    filestr>>g_light.color.x;
                    filestr>>g_light.color.y;
                    filestr>>g_light.color.z;
                }
                filestr>>attrType;
                if(!strcmp(attrType, "intensity:")){
                    filestr>>g_light.intensity;
                }
                break;
            case SPHERE:
                filestr>>attrType;
                if(!strcmp(attrType, "num:"))
                    filestr>>g_sphere_num;
                if(g_sphere_num > 0) {
                    g_spheres = new Sphere[g_sphere_num];
                    
                    for(int i=0; i<g_sphere_num; i++){
                    filestr>>attrType;
                    if(!strcmp(attrType, "position:")){
                        filestr>>g_spheres[i].pos.x; filestr>>g_spheres[i].pos.y;  filestr>>g_spheres[i].pos.z;
                    }
                    filestr>>attrType;
                    if(!strcmp(attrType, "radius:")){
                        filestr>>g_spheres[i].radius;
                    }
                    filestr>>attrType;
                    if(!strcmp(attrType, "color:")){
                        filestr>>g_spheres[i].color.x; filestr>>g_spheres[i].color.y;  filestr>>g_spheres[i].color.z;
                    }
                    filestr>>attrType;
                    if(!strcmp(attrType, "ambient:"))	filestr>>g_spheres[i].ambient;
                    filestr>>attrType;
                    if(!strcmp(attrType, "diffuse:"))	filestr>>g_spheres[i].diffuse;
                    filestr>>attrType;
                    if(!strcmp(attrType, "phong:")) 	filestr>>g_spheres[i].phong;
                    
                }
                }
                break;
            case BOX:
                filestr>>attrType;
                if(!strcmp(attrType, "num:"))
                    filestr>>g_box_num;
                
                if(g_box_num > 0) {
                    g_boxes = new Box[g_box_num];
                    for(int i=0; i<g_box_num; i++){
                        filestr>>attrType;
                        if(!strcmp(attrType, "conner_position:")){
                            filestr>>g_boxes[i].minPos.x;
                            filestr>>g_boxes[i].minPos.y;
                            filestr>>g_boxes[i].minPos.z;
                            filestr>>g_boxes[i].maxPos.x;
                            filestr>>g_boxes[i].maxPos.y;
                            filestr>>g_boxes[i].maxPos.z;
                        }
                        
                        filestr>>attrType;
                        if(!strcmp(attrType, "color:")){
                            filestr>>g_boxes[i].color.x;
                            filestr>>g_boxes[i].color.y;
                            filestr>>g_boxes[i].color.z;
                        }
                        filestr>>attrType;
                        if(!strcmp(attrType, "rotate:")){
                            filestr>>rot.x;
                            filestr>>rot.y;
                            filestr>>rot.z;
							//Convert to radians
							rot.x *= 3.14159265f / 180.0f;
							rot.y *= 3.14159265f / 180.0f;
							rot.z *= 3.14159265f / 180.0f;

                            mat4 m (1.0f);
                            
                            
                            // rotation order is zyx
                            m = rotate(m, rot.z, vec3(0, 0, 1));
                            m = rotate(m, rot.y, vec3(0, 1, 0));
                            m = rotate(m, rot.x, vec3(1, 0, 0));
                            
                            //cout<<glm::to_string(m)<<endl;
                            
                            g_boxes[i].rotMat = m;
                            g_boxes[i].invRotMat = inverse(m);
                        }
                        filestr>>attrType;
                        if(!strcmp(attrType, "ambient:"))	filestr>>g_boxes[i].ambient;
                        filestr>>attrType;
                        if(!strcmp(attrType, "diffuse:"))	filestr>>g_boxes[i].diffuse;
                        filestr>>attrType;
                        if(!strcmp(attrType, "phong:"))	filestr>>g_boxes[i].phong;
                    }
                    loop = false;
                }
                break;
        }
    }
}



void setParameter()
{    
    LoadConfigFile(dataFile);
    
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    viewAngle = 90.0f;
    viewTan = tan((viewAngle * PI / 180.0f) / 2.0f);
    viewXL = -0.64f;
    viewXR = 0.64f;
    viewYL = -0.48f;
    viewYR = 0.48f;

    pixelR = new float[g_winWidth * g_winHeight];
    pixelG = new float[g_winWidth * g_winHeight];
    pixelB = new float[g_winWidth * g_winHeight];

    Convert_M();
    Normalize(&g_light.pos.x, &g_light.pos.y, &g_light.pos.z);
    xIntV = (viewXR - viewXL) / g_winWidth;
    yIntV = (viewYR - viewYL) / g_winHeight;
}

/****** GL callbacks ******/
void initialization()
{    
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 640.0, 0.0, 480.0);
	glMatrixMode (GL_MODELVIEW);
}



void display()
{
    //glFinish();
    glClear(GL_COLOR_BUFFER_BIT);
    
	//glEnable(GL_LIGHTING);
	//glLightfv(GL_LIGHT0, GL_POSITION, light0_pos); // commenting out this line to make object always lit up in front of the cam. 

    // adding stuff to draw ...

    
    // drae sphere and box
    /*for (int i=0; i<g_sphere_num; i++)
        g_spheres[i].Draw();
    for (int i=0; i<g_box_num; i++)
        g_boxes[i].Draw();*/

    // draw pixels
    float r, g, b;
    for (int t = 0; t < g_winHeight; t++) {
        for (int s = 0; s < g_winWidth; s++) {
            r = pixelR[s + g_winWidth * t];
            g = pixelG[s + g_winWidth * t];
            b = pixelB[s + g_winWidth * t];

            
            glColor3f(r, g, b);
            glBegin(GL_POINTS);
            glVertex2f(s, t);
            glEnd();
        }
    }

    glFlush();

    
}



int main(int argc, char **argv) 
{
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(640.0, 480.0);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Ray Casting");
	
	glewInit();
	
    setParameter();
	glutDisplayFunc(display);
    GenerateRays();

	//glutReshapeFunc(reshape);
	/*glutMouseFunc(mouse);
	glutMotionFunc(motion);
    glutKeyboardUpFunc(keyup);
    glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);*/

    initialization();

	
    
	
    glutMainLoop();
    return EXIT_SUCCESS;
}