#include "glututility.h"

bool debug = true;
// define these for everything cause I'm sick of defining them
float black[] = { 0,0,0,1 };
float white[] = { 1,1,1,1 };
// from my hw6, calculates surface normals
void doanormal(float one[3], float two[3], float three[3]) {
    // this takes in 3 points then
    // sets the vector to point out
    // first, make a vector
    // this is one of our edges
    // using 0,1,2 is confusing
    int x = 0, y = 1, z = 2;
    double firstvector[3];
    firstvector[x] = one[x] - two[x];
    firstvector[y] = one[y] - two[y];
    firstvector[z] = one[z] - two[z];
    // then, make a vector
    // another of our edges
    // did three - two so that both vectors point
    // away from the point two
    // but not sure it actually matters
    double secondvector[3];
    secondvector[x] = three[x] - two[x];
    secondvector[y] = three[y] - two[y];
    secondvector[z] = three[z] - two[z];
    // then do a cross product
    double normal[3];
    normal[x] = firstvector[y] * secondvector[z] - firstvector[z] * secondvector[y];
    normal[y] = firstvector[z] * secondvector[x] - firstvector[x] * secondvector[z];
    normal[z] = firstvector[x] * secondvector[y] - firstvector[y] * secondvector[x];
    // then, set normal
    // mmm, math makes things so much easier
    glNormal3f(normal[x], normal[y], normal[z]);
}

// from my hw6 just convenience for colors cause I like rgb
// I forgot the openGL method and this is easier
// than finding it
void Color(float r, float g, float b) {
    glColor3f(r / 255, g / 255, b / 255);
}

// from my hw6 do a cylinder type thing
void Vertexflat(float thet, float height, float distance) {
    glVertex3f(Sin(thet)*distance, height, Cos(thet)*distance);
}

// Below this line is all class stuff
//----------------------------------------------------------------

/*
 *  Draw vertex in polar coordinates with normal
 */
void Vertex(double th, double ph)
{
    double x = Sin(th) * Cos(ph);
    double y = Cos(th) * Cos(ph);
    double z = Sin(ph);
    //  For a sphere at the origin, the position
    //  and normal vectors are the same
    glNormal3d(x, y, z);
    glVertex3d(x, y, z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
void ball(double x, double y, double z, double r)
{
    //  Save transformation
    glPushMatrix();
    //  Offset, scale and rotate
    glTranslated(x, y, z);
    glScaled(r, r, r);
    //  White ball with yellow specular
    float yellow[] = { 1.0,1.0,0.0,1.0 };
    float Emission[] = { 0.0,0.0,0.01 * 50,1.0 };
    glColor3f(1, 1, 1);
    glMaterialf(GL_FRONT, GL_SHININESS, .4);
    glMaterialfv(GL_FRONT, GL_SPECULAR, yellow);
    glMaterialfv(GL_FRONT, GL_EMISSION, Emission);
    //  Bands of latitude
    int inc = 15;
    for (int ph = -90; ph < 90; ph += inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (int th = 0; th <= 360; th += 2 * inc)
        {
            Vertex(th, ph);
            Vertex(th, ph + inc);
        }
        glEnd();
    }
    //  Undo transofrmations
    glPopMatrix();
}

//
//  Print message to stderr and exit
//
void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}

//
//  Check for OpenGL errors and print to stderr
//
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

//
//  Load texture from BMP file
//

//
//  Reverse n bytes
//
static void Reverse(void* x,const int n)
{
   int k;
   char* ch = (char*)x;
   for (k=0;k<n/2;k++)
   {
      char tmp = ch[k];
      ch[k] = ch[n-1-k];
      ch[n-1-k] = tmp;
   }
}

//
//  Set projection
//
void Project(enum camera cp, double fov, double asp, double dim)
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   switch (cp) {
   case perspective:
       gluPerspective(fov, asp, dim / 16, 16 * dim);
       break;
   case orthogonal:
       glOrtho(-asp * dim, asp * dim, -dim, +dim, -dim, +dim);
       break;
   case firstperson:
       gluPerspective(fov, asp, dim / 16, 16 * dim);
       break;
   default:
       break;
   }
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

//
//  Convenience routine to output raster text
//  Use VARARGS to make this more flexible
//

#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

//
//  Load texture from BMP file
//
unsigned int LoadTexBMP(const char* file)
{
   unsigned int   texture;    // Texture name
   FILE*          f;          // File pointer
   unsigned short magic;      // Image magic
   unsigned int   dx,dy,size; // Image dimensions
   unsigned short nbp,bpp;    // Planes and bits per pixel
   unsigned char* image;      // Image data
   unsigned int   off;        // Image offset
   unsigned int   k;          // Counter
   unsigned int   max;        // Maximum texture dimensions

   //  Open file
   f = fopen(file,"rb");
   if (!f) Fatal("Cannot open file %s\n",file);
   //  Check image magic
   if (fread(&magic,2,1,f)!=1) Fatal("Cannot read magic from %s\n",file);
   if (magic!=0x4D42 && magic!=0x424D) Fatal("Image magic not BMP in %s\n",file);
   //  Read header
   if (fseek(f,8,SEEK_CUR) || fread(&off,4,1,f)!=1 ||
       fseek(f,4,SEEK_CUR) || fread(&dx,4,1,f)!=1 || fread(&dy,4,1,f)!=1 ||
       fread(&nbp,2,1,f)!=1 || fread(&bpp,2,1,f)!=1 || fread(&k,4,1,f)!=1)
     Fatal("Cannot read header from %s\n",file);
   //  Reverse bytes on big endian hardware (detected by backwards magic)
   if (magic==0x424D)
   {
      Reverse(&off,4);
      Reverse(&dx,4);
      Reverse(&dy,4);
      Reverse(&nbp,2);
      Reverse(&bpp,2);
      Reverse(&k,4);
   }
   //  Check image parameters
   glGetIntegerv(GL_MAX_TEXTURE_SIZE,(int*)&max);
   if (dx<1 || dx>max) Fatal("%s image width %d out of range 1-%d\n",file,dx,max);
   if (dy<1 || dy>max) Fatal("%s image height %d out of range 1-%d\n",file,dy,max);
   if (nbp!=1)  Fatal("%s bit planes is not 1: %d\n",file,nbp);
   if (bpp!=24) Fatal("%s bits per pixel is not 24: %d\n",file,bpp);
   if (k!=0)    Fatal("%s compressed files not supported\n",file);
#ifndef GL_VERSION_2_0
   //  OpenGL 2.0 lifts the restriction that texture size must be a power of two
   for (k=1;k<dx;k*=2);
   if (k!=dx) Fatal("%s image width not a power of two: %d\n",file,dx);
   for (k=1;k<dy;k*=2);
   if (k!=dy) Fatal("%s image height not a power of two: %d\n",file,dy);
#endif

   //  Allocate image memory
   size = 3*dx*dy;
   image = (unsigned char*) malloc(size);
   if (!image) Fatal("Cannot allocate %d bytes of memory for image %s\n",size,file);
   //  Seek to and read image
   if (fseek(f,off,SEEK_SET) || fread(image,size,1,f)!=1) Fatal("Error reading data from image %s\n",file);
   fclose(f);
   //  Reverse colors (BGR -> RGB)
   for (k=0;k<size;k+=3)
   {
      unsigned char temp = image[k];
      image[k]   = image[k+2];
      image[k+2] = temp;
   }

   //  Sanity check
   ErrCheck("LoadTexBMP");
   //  Generate 2D texture
   glGenTextures(1,&texture);
   glBindTexture(GL_TEXTURE_2D,texture);
   //  Copy image
   glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,dx,dy,0,GL_RGB,GL_UNSIGNED_BYTE,image);
   if (glGetError()) Fatal("Error in glTexImage2D %s %dx%d\n",file,dx,dy);
   //  Scale linearly when image size doesn't match
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

   //  Free image memory
   free(image);
   //  Return texture name
   return texture;
}

//  Load an OBJ file
//  Vertex, Normal and Texture coordinates are supported
//  Materials are supported
//  Textures must be BMP files
//  Surfaces are not supported
//
//  WARNING:  This is a minimalist implementation of the OBJ file loader.  It
//  will only correctly load a small subset of possible OBJ files.  It is
//  intended to be a starting point to allow you to load models, but in order
//  for you to do this you will have to understand the OBJ format and most
//  likely will have to significantly enhance this code.
//
//  WARNING:  There are lots of really broken OBJ files on the internet.  Some
//  files may have correct surfaces, but the normals are complete junk and so
//  the lighting is totally broken.  So beware of which OBJ files you use.

//  Material structure
typedef struct
{
   char* name;                 //  Material name
   float Ka[4],Kd[4],Ks[4],Ns; //  Colors and shininess
   float d;                    //  Transparency
   int map;                    //  Texture
} mtl_t;

//  Material count and array
static int Nmtl=0;
static mtl_t* mtl=NULL;

//
//  Return true if CR or LF
//
static int CRLF(char ch)
{
   return ch == '\r' || ch == '\n';
}

//
//  Read line from file
//    Returns pointer to line or NULL on EOF
//
static int linelen=0;    //  Length of line
static char* line=NULL;  //  Internal storage for line
static char* readline(FILE* f)
{
   char ch;  //  Character read
   int k=0;  //  Character count
   while ((ch = fgetc(f)) != EOF)
   {
      //  Allocate more memory for long strings
      if (k>=linelen)
      {
         linelen += 8192;
         line = (char*)realloc(line,linelen);
         if (!line) Fatal("Out of memory in readline\n");
      }
      //  End of Line
      if (CRLF(ch))
      {
         // Eat extra CR or LF characters (if any)
         while ((ch = fgetc(f)) != EOF)
           if (!CRLF(ch)) break;
         //  Stick back the overrun
         if (ch != EOF) ungetc(ch,f);
         //  Bail
         break;
      }
      //  Pad character to line
      else
         line[k++] = ch;
   }
   //  Terminate line if anything was read
   if (k>0) line[k] = 0;
   //  Return pointer to line or NULL on EOF
   return k>0 ? line : NULL;
}

//
//  Read to next non-whitespace word
//  Note that this destroys line in the process
//
static char* getword(char** line)
{
   //  Skip leading whitespace
   while (**line && isspace(**line))
      (*line)++;
   if (!**line) return NULL;
   //  Start of word
   char* word = *line;
   //  Read until next whitespace
   while (**line && !isspace(**line))
      (*line)++;
   //  Mark end of word if not NULL
   if (**line)
   {
      **line = 0;
      (*line)++;
   }
   return word;
}

//
//  Read n floats
//
static void readfloat(char* line,int n,float x[])
{
   for (int i=0;i<n;i++)
   {
      char* str = getword(&line);
      if (!str)  Fatal("Premature EOL reading %d floats\n",n);
      if (sscanf(str,"%f",x+i)!=1) Fatal("Error reading float %d\n",i);
   }
}

//
//  Read coordinates
//    n is how many coordiantes to read
//    N is the coordinate index
//    M is the number of coordinates
//    x is the array
//    This function adds more memory as needed in 8192 work chunks
//
static void readcoord(char* line,int n,float* x[],int* N,int* M)
{
   //  Allocate memory if necessary
   if (*N+n > *M)
   {
      *M += 8192;
      *x = (float*)realloc(*x,(*M)*sizeof(float));
      if (!*x) Fatal("Cannot allocate memory\n");
   }
   //  Read n coordinates
   readfloat(line,n,(*x)+*N);
   (*N)+=n;
}

//
//  Read string conditionally
//     Line must start with skip string
//     After skip sting return first word
//     getword terminates the line
//
static char* readstr(char* line,const char* skip)
{
   //  Check for a match on the skip string
   while (*skip && *line && *skip==*line)
   {
      skip++;
      line++;
   }
   //  Skip must be NULL for a match
   if (*skip || !isspace(*line)) return NULL;
   //  Read string
   return getword(&line);
}

//
//  Load materials from file
//
static void LoadMaterial(const char* file)
{
   int k=-1;
   char* line;
   char* str;

   //  Open file or return with warning on error
   FILE* f = fopen(file,"r");
   if (!f)
   {
      fprintf(stderr,"Cannot open material file %s\n",file);
      return;
   }

   //  Read lines
   while ((line = readline(f)))
   {
      //  New material
      if ((str = readstr(line,"newmtl")))
      {
         int l = strlen(str);
         //  Allocate memory for structure
         k = Nmtl++;
         mtl = (mtl_t*)realloc(mtl,Nmtl*sizeof(mtl_t));
         //  Store name
         mtl[k].name = (char*)malloc(l+1);
         if (!mtl[k].name) Fatal("Cannot allocate %d for name\n",l+1);
         strcpy(mtl[k].name,str);
         //  Initialize materials
         mtl[k].Ka[0] = mtl[k].Ka[1] = mtl[k].Ka[2] = 0;   mtl[k].Ka[3] = 1;
         mtl[k].Kd[0] = mtl[k].Kd[1] = mtl[k].Kd[2] = 0;   mtl[k].Kd[3] = 1;
         mtl[k].Ks[0] = mtl[k].Ks[1] = mtl[k].Ks[2] = 0;   mtl[k].Ks[3] = 1;
         mtl[k].Ns  = 0;
         mtl[k].d   = 0;
         mtl[k].map = 0;
      }
      //  If no material short circuit here
      else if (k<0)
      {}
      //  Ambient color
      else if (line[0]=='K' && line[1]=='a')
         readfloat(line+2,3,mtl[k].Ka);
      //  Diffuse color
      else if (line[0]=='K' && line[1] == 'd')
         readfloat(line+2,3,mtl[k].Kd);
      //  Specular color
      else if (line[0]=='K' && line[1] == 's')
         readfloat(line+2,3,mtl[k].Ks);
      //  Material Shininess
      else if (line[0]=='N' && line[1]=='s')
      {
         readfloat(line+2,1,&mtl[k].Ns);
         if (mtl[k].Ns>128) mtl[k].Ns = 128;
      }
      //  Textures (must be BMP - will fail if not)
      else if ((str = readstr(line,"map_Kd")))
         mtl[k].map = LoadTexBMP(str);
      //  Ignore line if we get here
   }
   fclose(f);
}

//
//  Set material
//
static void SetMaterial(const char* name)
{
   //  Search materials for a matching name
   for (int k=0;k<Nmtl;k++)
      if (!strcmp(mtl[k].name,name))
      {
         //  Set material colors
         glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT  ,mtl[k].Ka);
         glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE  ,mtl[k].Kd);
         glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR ,mtl[k].Ks);
         glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,&mtl[k].Ns);
         //  Bind texture if specified
         if (mtl[k].map)
         {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,mtl[k].map);
         }
         else
            glDisable(GL_TEXTURE_2D);
         return;
      }
   //  No matches
   fprintf(stderr,"Unknown material %s\n",name);
}

//
//  Load OBJ file
//
int LoadOBJ(const char* file)
{
   int  Nv,Nn,Nt;  //  Number of vertex, normal and textures
   int  Mv,Mn,Mt;  //  Maximum vertex, normal and textures
   float* V;       //  Array of vertexes
   float* N;       //  Array of normals
   float* T;       //  Array if textures coordinates
   char*  line;    //  Line pointer
   char*  str;     //  String pointer

   //  Open file
   FILE* f = fopen(file,"r");
   if (!f) Fatal("Cannot open file %s\n",file);

   // Reset materials
   mtl = NULL;
   Nmtl = 0;

   //  Start new displaylist
   int list = glGenLists(1);
   glNewList(list,GL_COMPILE);
   //  Push attributes for textures
   glPushAttrib(GL_TEXTURE_BIT);

   //  Read vertexes and facets
   V  = N  = T  = NULL;
   Nv = Nn = Nt = 0;
   Mv = Mn = Mt = 0;
   while ((line = readline(f)))
   {
      //  Vertex coordinates (always 3)
      if (line[0]=='v' && line[1]==' ')
         readcoord(line+2,3,&V,&Nv,&Mv);
      //  Normal coordinates (always 3)
      else if (line[0]=='v' && line[1] == 'n')
         readcoord(line+2,3,&N,&Nn,&Mn);
      //  Texture coordinates (always 2)
      else if (line[0]=='v' && line[1] == 't')
         readcoord(line+2,2,&T,&Nt,&Mt);
      //  Read and draw facets
      else if (line[0]=='f')
      {
         line++;
         //  Read Vertex/Texture/Normal triplets
         glBegin(GL_POLYGON);
         while ((str = getword(&line)))
         {
            int Kv,Kt,Kn;
            //  Try Vertex/Texture/Normal triplet
            if (sscanf(str,"%d/%d/%d",&Kv,&Kt,&Kn)==3)
            {
               if (Kv<0 || Kv>Nv/3) Fatal("Vertex %d out of range 1-%d\n",Kv,Nv/3);
               if (Kn<0 || Kn>Nn/3) Fatal("Normal %d out of range 1-%d\n",Kn,Nn/3);
               if (Kt<0 || Kt>Nt/2) Fatal("Texture %d out of range 1-%d\n",Kt,Nt/2);
            }
            //  Try Vertex//Normal pairs
            else if (sscanf(str,"%d//%d",&Kv,&Kn)==2)
            {
               if (Kv<0 || Kv>Nv/3) Fatal("Vertex %d out of range 1-%d\n",Kv,Nv/3);
               if (Kn<0 || Kn>Nn/3) Fatal("Normal %d out of range 1-%d\n",Kn,Nn/3);
               Kt = 0;
            }
            //  Try Vertex index
            else if (sscanf(str,"%d",&Kv)==1)
            {
               if (Kv<0 || Kv>Nv/3) Fatal("Vertex %d out of range 1-%d\n",Kv,Nv/3);
               Kn = 0;
               Kt = 0;
            }
            //  This is an error
            else
               Fatal("Invalid facet %s\n",str);
            //  Draw vectors
            if (Kt) glTexCoord2fv(T+2*(Kt-1));
            if (Kn) glNormal3fv(N+3*(Kn-1));
            if (Kv) glVertex3fv(V+3*(Kv-1));
         }
         glEnd();
      }
      //  Use material
      else if ((str = readstr(line,"usemtl")))
         SetMaterial(str);
      //  Load materials
      else if ((str = readstr(line,"mtllib")))
         LoadMaterial(str);
      //  Skip this line
   }
   fclose(f);
   //  Pop attributes (textures)
   glPopAttrib();
   glEndList();

   //  Free materials
   for (int k=0;k<Nmtl;k++)
      free(mtl[k].name);
   free(mtl);

   //  Free arrays
   free(V);
   free(T);
   free(N);

   return list;
}