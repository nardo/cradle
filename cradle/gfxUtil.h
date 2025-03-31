// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

#define CREATE_SQUARE_TEXTURE 1

static ColorI &getBitmapModulation()
{
    static ColorI gfxBitmapModulation(255,255,255,255);
    return gfxBitmapModulation;
}

static void setBitmapModulation( const ColorI &modColor )
{
    ColorI &bm = getBitmapModulation();
    bm.set(modColor);
}

// Save the current framebuffer RGB to the given bmp file:
bool screenShot(const char * fileName)
{
   glFinish();

   int32 width = A.UI.windowWidth;
   int32 height = A.UI.windowHeight;

   uint8 * pixels = new uint8 [width * height * 4];

   // Just read in as RGBA so can ignore pixel packing:
   glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    SDL_Surface *outSurface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGB24);

   uint8 * p24 = (uint8 *) outSurface->pixels;
   uint8 * p32 = pixels;

   for(int32 y = 0; y < height; y++)
   {
      for(int32 x = 0; x < width; x++)
      {
         // BGR->RGB
         p24[0] = p32[2];
         p24[1] = p32[1];
         p24[2] = p32[0];
         p24 += 3;
         p32 += 4;
      }
   }
    
   bool result = false;
    IMG_SavePNG(outSurface, fileName);

   delete [] pixels;
    SDL_DestroySurface(outSurface);
   return result;
}


typedef float32 (tRampCallback)(float32 dist, float32 radius, float32 f);
/**
 * Linear falloff.
 *
 * @param dist
 * @param radius
 * @param f      Falloff start distance.
 *
 * @return
 */
static float32 linearRampCallback(float32 dist, float32 radius, float32 f)
{
   if(dist <= f)
      return 1.f;

   return 1.f - ((dist - f) / (radius - f));
}

/**
 * Sawtooth falloff.
 *
 * @param dist
 * @param radius
 * @param f      ramps from 0,1 over 0->dist, and again from 1,0
 *               over dist->radius
 *
 * @return
 */
static float32 sawtoothRampCallback(float32 dist, float32 radius, float32 f)
{
   if(dist <= f)
      return dist / f;

   return 1.f - ((dist - f) / (radius - f));
}

/**
 * Cos falloff.
 *
 * @param dist
 * @param radius
 * @param f      Falloff start distance.
 *
 * @return
 */
static float32 cosRampCallback(float32 dist, float32 radius, float32 f)
{
   return cos((FloatPi/2.f) * ((dist - f) / (radius - f)));
}

/**
 * Exp falloff.
 *
 * @param dist
 * @param radius
 * @param f      Ramp is from exp(-f)/e to exp(1)/e
 *
 * @return
 */
static float32 expRampCallback(float32 dist, float32 radius, float32 f)
{
   float32 ratio = dist / radius;
   return 1.f - (exp(-f + ratio * (f + 1.f)) / 2.718281828);
}


/**
 * For Particle System.
 *
 * @param dist
 * @param radius
 * @param f      Falloff start distance.
 *
 * @return
 */
static float32 particleRampCallback(float32 dist, float32 radius, float32 f)
{
   if(dist <= f)
      return 1.f;

   return (1.f - (dist / radius))*0.35f;
}

static float32 trailRampCallback(float32 dist, float32 radius, float32 f)
{
   if(dist <= f)
      return 1.f;

   return (1.f - ((dist - f) / (radius - f)))*0.35f;
}


static float32 repairLineRampCallback(float32 dist, float32 radius, float32 f)
{
   if(dist <= f)
      return 1.f;

    return (1.f - (dist / radius))*0.35f;
}

static float32 repairCircleRampCallback(float32 dist, float32 radius, float32 f)
{
   if(dist <= f)
      return 1.f;

   return (1.f - (dist / radius))*0.65f;
}


//------------------------------
// 8-way symmetric pixel helper:
//------------------------------
#define putpixel(x, y, dim, val) { \
   uint32 cen = dim / 2; \
   bmp[(cen + x) + (cen + y) * dim] = \
   bmp[(cen-1 - x) + (cen + y) * dim] = \
   bmp[(cen + x) + (cen-1 - y) * dim] = \
   bmp[(cen-1 - x) + (cen-1 - y) * dim] = \
   bmp[(cen + y) + (cen + x) * dim] = \
   bmp[(cen-1 - y) + (cen + x) * dim] = \
   bmp[(cen + y) + (cen-1 - x) * dim] = \
   bmp[(cen-1 - y) + (cen-1 - x) * dim] = val; }

/**
 * Creates a float buffer of dim*dim containing the appropriate
 * falloff intensities.
 *
 * @param dim
 * @param factor
 * @param callback
 * @param circle   If set creates (dim,dim) buffer, otherwise a
 *                 (dim,1) buffer.
 *
 * @return
 */
static float32 * constructFalloffIntensities(uint32 dim, float32 factor, tRampCallback callback, bool circle)
{
#if (CREATE_SQUARE_TEXTURE == 1)
   uint32 size = (dim * dim);
#else
   uint32 size = circle ? (dim * dim) : dim;
#endif

   float32 * bmp = new float32[size];
   memset(bmp, 0, sizeof(float32) * size);

   int32 cen = (dim/2);

   // Create a wedge and use symmetry to fill in circle:
   if(circle)
   {
      int32 x = cen - 1;
      int32 y = 0;
   
      float32 t = 0.f;
   
      while(x > y)
      {
         float32 ys = float32(y*y);
         float32 len = sqrt((cen-1) * (cen-1) - ys);
   
         float32 d = mCeil(len) - len;
         if(d < t)
            x--;
   
         for(int32 ax = y; ax < x; ax++)
         {
            float32 dist = sqrt(float32(ax*ax + y*y));
            float32 val = callback(dist, cen, factor);
            putpixel(ax, y, dim, val);
         }
   
         float32 dist = sqrt(float32(x*x + y*y));
         float32 val = callback(dist, cen, factor);
         putpixel(x, y, dim, val);
   
         t = d;
         y++;
      }
   }
   else
   {
      // Create a single row (symmetric about center):
      for(int32 i = 0; i < cen; i++)
      {
         float32 dist = sqrt(float32(i*i));
         float32 val = callback(dist, cen, factor);

         bmp[cen - i] = val;
         bmp[cen + i] = val;
      }

#if (CREATE_SQUARE_TEXTURE == 1)
      // Copy the row:
      for(int32 i = 1; i < dim; i++)
        memcpy(&bmp[dim*i], &bmp[0], dim * sizeof(float32));
#endif
   }

   return bmp;
}
#undef putpixel

/**
 * Creates an RGBA falloff texture of given dimension.
 *
 * @param dim
 * @param factor
 * @param callback
 * @param circle   If set creates (dim,dim) texture, otherwise a (dim,1) texture.
 *
 * @return
 */
static TextureHandle createFalloffTextureRGBA(uint32 dim, float32 factor, tRampCallback callback, bool circle, bool rampColor = false)
{
   float32 * intensities = constructFalloffIntensities(dim, factor, callback, circle);

#if (CREATE_SQUARE_TEXTURE == 1)
   int32 numPixels = (dim * dim);
#else
   int32 numPixels = circle ? (dim * dim) : dim;
#endif

   ColorI * bmp = new ColorI[numPixels];

   if(rampColor)
   {
      for(int32 i = numPixels - 1; i >= 0; i--)
      {
         float32 val = 255.f * intensities[i];
         bmp[i].set(val, val, val, val);
      }
   }
   else
   {
      for(int32 i = numPixels - 1; i >= 0; i--)
         bmp[i].set(255,255,255,255.f * intensities[i]);
   }

#if (CREATE_SQUARE_TEXTURE == 1)
    TextureHandle texture = A.TX.createTexture(dim, dim, GL_RGBA, GL_UNSIGNED_BYTE, &bmp[0]);
#else
    TextureHandle texture = A.TX.createTexture(dim, circle ? dim : 1, GL_RGBA, GL_UNSIGNED_BYTE, &bmp[0]);
#endif

   delete [] bmp;
   delete [] intensities;

   return texture;
}
