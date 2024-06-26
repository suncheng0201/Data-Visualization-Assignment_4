/*
 *		This Code Was Created By Jeff Molofee 2000
 *		Modified by Shawn T. to handle (%3.2f, num) parameters.
 *		A HUGE Thanks To Fredric Echols For Cleaning Up
 *		And Optimizing The Base Code, Making It More Flexible!
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit My Site At nehe.gamedev.net
 */

#include <windows.h>		// Header File For Windows
#include <math.h>			// Header File For Windows Math Library
#include <stdio.h>			// Header File For Standard Input/Output
#include <stdarg.h>			// Header File For Variable Argument Routines
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library
#include <mmsystem.h>    	// PlaySound()
#pragma comment (lib, "winmm.lib") //PlaySound


HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

GLuint	base;				// Base Display List For The Font Set
GLfloat	rot;				// Used To Rotate The Text

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default
bool light;//光源的开关
bool lp;//按下L键了么
bool fp;//按下F键了么

GLfloat xrot;//x旋转
GLfloat yrot;//y旋转
GLfloat xspeed;//x旋转速度
GLfloat yspeed;//y旋转速度
GLfloat z=-2.0f;//深入屏幕的距离

GLfloat LightAmbient[]={0.5f,0.5f,0.5f,1.0f};//环境光参数
GLfloat LightDiffuse[]={1.0f,1.0f,1.0f,1.0f};//漫射光参数（都为设为最大值）
GLfloat LightPosition[]={0.0f,0.0f,5.0f,1.0f};//记录光源位置

GLuint filter;//滤波类型
GLuint texture[3];//3种纹理存储空间

GLYPHMETRICSFLOAT gmf[256];	// Storage For Information About Our Outline Font Characters

AUX_RGBImageRec *LoadBMP(char *Filename){
FILE *File=NULL;
if(!Filename)
{return NULL;}
File=fopen(Filename,"r");//尝试打开文件
if(File){fclose(File); 
return auxDIBImageLoad(Filename);}
return NULL;
}

int LoadGLTextures(){
bool Status=FALSE; 
AUX_RGBImageRec *TextureImage[4];//创建纹理的存储空间
memset(TextureImage,0,sizeof(void *)*4);//将指针设为空
if(TextureImage[0]=LoadBMP("C:\\CC1.bmp")){
	Status=TRUE;
	glGenTextures(1,&texture[0]);//创建纹理

	//创建Nearest滤波贴图（低质量贴图）
	glBindTexture(GL_TEXTURE_2D,texture[0]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);//绘制的图片比贴图小
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);//绘制的图片比贴图大
	glTexImage2D(GL_TEXTURE_2D,0,3,TextureImage[0]->sizeX,TextureImage[0]->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,TextureImage[0]->data);

	//创建线性滤波纹理
	glBindTexture(GL_TEXTURE_2D,texture[0]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,3,TextureImage[0]->sizeX,TextureImage[0]->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,TextureImage[0]->data);

	//创建MipMapped纹理(可以绕过对纹理图片宽度和高度的限制)
	glBindTexture(GL_TEXTURE_2D,texture[0]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D,3,TextureImage[0]->sizeX,TextureImage[0]->sizeY,GL_RGB,GL_UNSIGNED_BYTE,TextureImage[0]->data);
}

if(TextureImage[1]=LoadBMP("C:\\CC2.bmp")){
	Status=TRUE;
	glGenTextures(1,&texture[1]);//创建纹理

	//创建Nearest滤波贴图（低质量贴图）
	glBindTexture(GL_TEXTURE_2D,texture[1]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);//绘制的图片比贴图小
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);//绘制的图片比贴图大
	glTexImage2D(GL_TEXTURE_2D,0,3,TextureImage[1]->sizeX,TextureImage[1]->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,TextureImage[1]->data);

	//创建线性滤波纹理
	glBindTexture(GL_TEXTURE_2D,texture[1]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,3,TextureImage[1]->sizeX,TextureImage[1]->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,TextureImage[1]->data);

	//创建MipMapped纹理(可以绕过对纹理图片宽度和高度的限制)
	glBindTexture(GL_TEXTURE_2D,texture[1]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D,3,TextureImage[1]->sizeX,TextureImage[1]->sizeY,GL_RGB,GL_UNSIGNED_BYTE,TextureImage[1]->data);
}

if(TextureImage[2]=LoadBMP("C:\\CC3.bmp")){
	Status=TRUE;
	glGenTextures(1,&texture[2]);//创建纹理

	//创建Nearest滤波贴图（低质量贴图）
	glBindTexture(GL_TEXTURE_2D,texture[2]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);//绘制的图片比贴图小
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);//绘制的图片比贴图大
	glTexImage2D(GL_TEXTURE_2D,0,3,TextureImage[2]->sizeX,TextureImage[2]->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,TextureImage[2]->data);

	//创建线性滤波纹理
	glBindTexture(GL_TEXTURE_2D,texture[2]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,3,TextureImage[2]->sizeX,TextureImage[2]->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,TextureImage[2]->data);

	//创建MipMapped纹理(可以绕过对纹理图片宽度和高度的限制)
	glBindTexture(GL_TEXTURE_2D,texture[2]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D,3,TextureImage[2]->sizeX,TextureImage[2]->sizeY,GL_RGB,GL_UNSIGNED_BYTE,TextureImage[2]->data);
}
if(TextureImage[3]=LoadBMP("C:\\CC4.bmp")){
	Status=TRUE;
	glGenTextures(1,&texture[3]);//创建纹理

	//创建Nearest滤波贴图（低质量贴图）
	glBindTexture(GL_TEXTURE_2D,texture[3]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);//绘制的图片比贴图小
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);//绘制的图片比贴图大
	glTexImage2D(GL_TEXTURE_2D,0,3,TextureImage[3]->sizeX,TextureImage[3]->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,TextureImage[3]->data);

	//创建线性滤波纹理
	glBindTexture(GL_TEXTURE_2D,texture[3]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,3,TextureImage[3]->sizeX,TextureImage[3]->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,TextureImage[3]->data);

	//创建MipMapped纹理(可以绕过对纹理图片宽度和高度的限制)
	glBindTexture(GL_TEXTURE_2D,texture[3]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D,3,TextureImage[3]->sizeX,TextureImage[3]->sizeY,GL_RGB,GL_UNSIGNED_BYTE,TextureImage[3]->data);
}
	if(TextureImage[0]){
		if(TextureImage[0]->data){
	free(TextureImage[0]->data);}
	free(TextureImage[0]);}

	if(TextureImage[1]){
		if(TextureImage[1]->data){
	free(TextureImage[1]->data);}
	free(TextureImage[1]);}

	if(TextureImage[2]){
		if(TextureImage[2]->data){
	free(TextureImage[2]->data);}
	free(TextureImage[2]);}

	if(TextureImage[3]){
		if(TextureImage[3]->data){
	free(TextureImage[3]->data);}
	free(TextureImage[3]);}

	return Status;
}

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

GLvoid BuildFont(GLvoid)								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID

	base = glGenLists(256);								// Storage For 256 Characters

	font = CreateFont(	-12,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Comic Sans MS");				// Font Name

	SelectObject(hDC, font);							// Selects The Font We Created

	wglUseFontOutlines(	hDC,							// Select The Current DC
						0,								// Starting Character
						255,							// Number Of Display Lists To Build
						base,							// Starting Display Lists
						0.0f,							// Deviation From The True Outlines
						0.2f,							// Font Thickness In The Z Direction
						WGL_FONT_POLYGONS,				// Use Polygons, Not Lines
						gmf);							// Address Of Buffer To Recieve Data
}

GLvoid KillFont(GLvoid)									// Delete The Font
{
  glDeleteLists(base, 256);								// Delete All 256 Characters
}

GLvoid glPrint(const char *fmt, ...)					// Custom GL "Print" Routine
{
	float		length=0;								// Used To Find The Length Of The Text
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	for (unsigned int loop=0;loop<strlen(text);loop++)	// Loop To Find Text Length
	{
		length+=gmf[text[loop]].gmfCellIncX;			// Increase Length By Each Characters Width
	}

	glTranslatef(-length/2,0.0f,0.0f);					// Center Our Text On The Screen

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base);									// Sets The Base Character to 0
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	if(!LoadGLTextures()){return false;}//载入位图
	glEnable(GL_TEXTURE_2D);//启动2D纹理映射

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glEnable(GL_LIGHT0);								// Enable Default Light (Quick And Dirty)
	glEnable(GL_LIGHTING);								// Enable Lighting
	glEnable(GL_COLOR_MATERIAL);						// Enable Coloring Of Material
	
	glLightfv(GL_LIGHT1,GL_AMBIENT,LightAmbient);//设置环境光
	glLightfv(GL_LIGHT1,GL_DIFFUSE,LightDiffuse);//设置漫射光
	glLightfv(GL_LIGHT1,GL_POSITION,LightPosition); // 设置光源位置
	glEnable(GL_LIGHT1);//启用一号光源


	BuildFont();										// Build The Font

	return TRUE;										// Initialization Went OK
}

int BackGround(GLvoid)
{
	glLoadIdentity();
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);	
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);					// Type Of Blending To Perform
	glColor3f( 1.0f,1.0f,1.0f);
	glTranslatef(0.0f,0.0f,0.5f);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTranslatef(0.0f, 0.0f, -30.0f);
	glBegin(GL_QUADS);
	glNormal3f( 0.0f, 0.0f, 0.5f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-18.0f, -10.0f,  10.0f);	
	glTexCoord2f(1.0f, 0.0f); glVertex3f( 18.0f, -10.0f,  10.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( 18.0f,  10.0f,  10.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-18.0f,  10.0f,  10.0f);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glEnd();
	return true;
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(0.0f,2.0f,-10.0f);						// Move One Unit Into The Screen
	glRotatef(0.0f,1.0f,0.0f,0.0f);						// Rotate On The X Axis
	glRotatef(rot*1.5f,0.0f,1.0f,0.0f);					// Rotate On The Y Axis
	glRotatef(0.0f,0.0f,0.0f,1.0f);					// Rotate On The Z Axis
 	glPrint("Merry Christmas!",rot/50);						// Print GL Text To The Screen
	rot+=0.03f;											// Increase The Rotation Variable



	glTranslatef(-2.8f,-3.2f,0.0f);//中心沿着z轴移动
	glRotatef(xrot,0.03f,0.0f,0.0f);//绕x轴旋转
	glRotatef(yrot,0.0f,0.03f,0.0f);//绕y轴旋转

	glBindTexture(GL_TEXTURE_2D,texture[0]);//贴第一张图片

	glBegin(GL_QUADS);//开始绘制四边形
			//法线指向观察者

			//（前侧面）
			glNormal3f(0.0f,0.0f,1.0f);
			glTexCoord2f(0.0f,0.0f);
			glVertex3f(-1.5f,-1.5f,1.5f);
			glTexCoord2f(1.0f,0.0f);
			glVertex3f(1.5f,-1.5f,1.5f);
			glTexCoord2f(1.0f,1.0f);
			glVertex3f(1.5f,1.5f,1.5f);
			glTexCoord2f(0.0f,1.0f);
			glVertex3f(-1.5f,1.5f,1.5f);

			//（后侧面）
			glNormal3f(0.0f,0.0f,-1.0f);
			glTexCoord2f(1.0f,0.0f);
			glVertex3f(-1.5f,-1.5f,-1.5f);
			glTexCoord2f(1.0f,1.0f);
			glVertex3f(-1.5f,1.5f,-1.5f);
			glTexCoord2f(0.0f,1.0f);
			glVertex3f(1.5f,1.5f,-1.5f);
			glTexCoord2f(0.0f,0.0f);
			glVertex3f(1.5f,-1.5f,-1.5f);

			glEnd();

			glBindTexture( GL_TEXTURE_2D, texture[1] );
			glBegin( GL_QUADS ); 
			//（上面）
			glNormal3f(0.0f,1.0f,0.0f);
			glTexCoord2f(0.0f,1.0f);
			glVertex3f(-1.5f,1.5f,-1.5f);
			glTexCoord2f(0.0f,0.0f);
			glVertex3f(-1.5f,1.5f,1.5f);
			glTexCoord2f(1.0f,0.0f);
			glVertex3f(1.5f,1.5f,1.5f);
			glTexCoord2f(1.0f,1.0f);
			glVertex3f(1.5f,1.5f,-1.5f);
		
			//（下面）
			glNormal3f(0.0f,-1.0f,0.0f);
			glTexCoord2f(1.0f,1.0f);
			glVertex3f(-1.5f,-1.5f,-1.5f);
			glTexCoord2f(0.0f,1.0f);
			glVertex3f(1.5f,-1.5f,-1.5f);
			glTexCoord2f(0.0f,0.0f);
			glVertex3f(1.5f,-1.5f,1.5f);
			glTexCoord2f(1.0f,0.0f);
			glVertex3f(-1.5f,-1.5f,1.5f);
 
			glEnd();

			glBindTexture( GL_TEXTURE_2D, texture[2] );
			glBegin( GL_QUADS ); 
			//（左面）
			glNormal3f(-1.0f,0.0f,0.0f);
			glTexCoord2f(0.0f,0.0f);
			glVertex3f(-1.5f,-1.5f,-1.5f);
			glTexCoord2f(1.0f,0.0f);
			glVertex3f(-1.5f,-1.5f,1.5f);
			glTexCoord2f(1.0f,1.0f);
			glVertex3f(-1.5f,1.5f,1.5f);
			glTexCoord2f(0.0f,1.0f);
			glVertex3f(-1.5f,1.5f,-1.5f);
		
			//（右面）
			glNormal3f(1.0f,0.0f,0.0f);
			glTexCoord2f(1.0f,0.0f);
			glVertex3f(1.5f,-1.5f,-1.5f);
			glTexCoord2f(1.0f,1.0f);
			glVertex3f(1.5f,1.5f,-1.5f);
			glTexCoord2f(0.0f,1.0f);
			glVertex3f(1.5f,1.5f,1.5f);
			glTexCoord2f(0.0f,0.0f);
			glVertex3f(1.5f,-1.5f,1.5f);
			
			glEnd();

	xrot+=0.03f;
	yrot+=0.03f;

		BackGround();
	return TRUE;										// Everything Went OK
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}

	KillFont();
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			
			PlaySound(("C:\\We Wish You A Merry Christmas.wav"),NULL,SND_LOOP | SND_ASYNC);	

			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

		case WM_SIZE:								// Resize The OpenGL Window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop

	// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;							// Windowed Mode
	}

	// Create Our OpenGL Window
	if (!CreateGLWindow("Suncheng's Christmas Card",640,480,16,fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if ((active && !DrawGLScene()) || keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
			{
				done=TRUE;							// ESC or DrawGLScene Signalled A Quit
			}
			else									// Not Time To Quit, Update Screen
			{
				DrawGLScene();
				SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
			}

			if (keys[VK_F1])						// Is F1 Being Pressed?
			{
				keys[VK_F1]=FALSE;					// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow("Suncheng's Christmas Card",640,480,16,fullscreen))
				{
					return 0;						// Quit If Window Was Not Created
				}
			}
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}
