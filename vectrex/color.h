// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

struct Color
{
    float r, g, b;
    
    Color(const Color &c) { r = c.r; g = c.g; b = c.b; }
    Color(float red = 1, float green = 1, float blue = 1) { r = red; g = green; b = blue; }
    void read(const char **argv) { r = (float) atof(argv[0]); g = (float) atof(argv[1]); b = (float) atof(argv[2]); }
    
    void interp(float t, const Color &c1, const Color &c2)
    {
        float oneMinusT = 1.0f - t;
        r = c1.r * t + c2.r * oneMinusT;
        g = c1.g * t + c2.g * oneMinusT;
        b = c1.b * t + c2.b * oneMinusT;
    }
    void set(float _r, float _g, float _b) { r = _r; g = _g; b = _b; }
    void set(const Color &c) { r = c.r; g = c.g; b = c.b; }
    
    Color operator+(const Color &c) const { return Color (r + c.r, g + c.g, b + c.b); }
    Color operator-(const Color &c) const { return Color (r - c.r, g - c.g, b - c.b); }
    Color operator-() const { return Color(-r, -g, -b); }
    Color &operator+=(const Color &c) { r += c.r; g += c.g; b += c.b; return *this; }
    Color &operator-=(const Color &c) { r -= c.r; g -= c.g; b -= c.b; return *this; }
    
    Color operator*(const float f) { return Color (r * f, g * f, b * f); }
    Color &operator*=(const float f) { r *= f; g *= f; b *= f; return *this; }
};

class ColorI
{
  public:
   uint8 red;
   uint8 green;
   uint8 blue;
   uint8 alpha;

  public:
   ColorI() { }
   ColorI(const ColorI& in_rCopy)
    {
       set(in_rCopy);
    }
    ColorI(const Color &_c, float32 _alpha = 1)
    {
        set(_c, _alpha);
    }

   ColorI(const uint8 in_r,
          const uint8 in_g,
          const uint8 in_b,
          const uint8 in_a = uint8(255))
    {
       set(in_r, in_g, in_b, in_a);
    }

   void set(const uint8 in_r,
            const uint8 in_g,
            const uint8 in_b,
            const uint8 in_a = uint8(255))
    {
       red   = in_r;
       green = in_g;
       blue  = in_b;
       alpha = in_a;
    }
    void set(const ColorI &in_rCopy)
    {
        red   = in_rCopy.red;
        green = in_rCopy.green;
        blue  = in_rCopy.blue;
        alpha = in_rCopy.alpha;
    }
    void set(const Color &_c, float32 _alpha = 1)
    {
        red = _c.r * 255;
        green = _c.g * 255;
        blue = _c.b * 255;
        alpha = _alpha * 255;
    }

   ColorI& operator*=(const float32 in_mul)
    {
       red   = uint8((float32(red)   * in_mul) + 0.5f);
       green = uint8((float32(green) * in_mul) + 0.5f);
       blue  = uint8((float32(blue)  * in_mul) + 0.5f);
       alpha = uint8((float32(alpha) * in_mul) + 0.5f);

       return *this;
    }

   ColorI  operator*(const float32 in_mul) const
    {
       ColorI temp(*this);
       temp *= in_mul;
       return temp;
    }

   ColorI  operator+(const ColorI& in_rAdd) const
    {
       ColorI tmp;

       tmp.red   = red   + in_rAdd.red;
       tmp.green = green + in_rAdd.green;
       tmp.blue  = blue  + in_rAdd.blue;
       tmp.alpha = alpha + in_rAdd.alpha;

       return tmp;
    }

   ColorI&  operator+=(const ColorI& in_rAdd)
    {
       red   += in_rAdd.red;
       green += in_rAdd.green;
       blue  += in_rAdd.blue;
       alpha += in_rAdd.alpha;

       return *this;
    }

   ColorI& operator*=(const int32 in_mul)
    {
       red   = red    * in_mul;
       green = green  * in_mul;
       blue  = blue   * in_mul;
       alpha = alpha  * in_mul;

       return *this;
    }

   ColorI& operator/=(const int32 in_mul)
    {
       red   = red    / in_mul;
       green = green  / in_mul;
       blue  = blue   / in_mul;
       alpha = alpha  / in_mul;

       return *this;
    }

   ColorI  operator*(const int32 in_mul) const
    {
       ColorI temp(*this);
       temp *= in_mul;
       return temp;
    }

   ColorI  operator/(const int32 in_mul) const
    {
       ColorI temp(*this);
       temp /= in_mul;
       return temp;
    }

   bool operator==(const ColorI& in_Cmp) const
    {
        return (red == in_Cmp.red && green == in_Cmp.green && blue == in_Cmp.blue && alpha == in_Cmp.alpha);
    }

   bool operator!=(const ColorI& in_Cmp) const
    {
        return (red != in_Cmp.red || green != in_Cmp.green || blue != in_Cmp.blue || alpha != in_Cmp.alpha);
    }

   void interpolate(const ColorI& in_rC1,
                    const ColorI& in_rC2,
                    const float32  in_factor)
    {
       float32 f2= 1.0f - in_factor;
       red   = uint8(((float32(in_rC1.red)   * f2) + (float32(in_rC2.red)   * in_factor)) + 0.5f);
       green = uint8(((float32(in_rC1.green) * f2) + (float32(in_rC2.green) * in_factor)) + 0.5f);
       blue  = uint8(((float32(in_rC1.blue)  * f2) + (float32(in_rC2.blue)  * in_factor)) + 0.5f);
       alpha = uint8(((float32(in_rC1.alpha) * f2) + (float32(in_rC2.alpha) * in_factor)) + 0.5f);
    }


   uint32 getARGBPack() const
    {
       return (uint32(alpha) << 24) |
              (uint32(red)   << 16) |
              (uint32(green) <<  8) |
              (uint32(blue)  <<  0);
    }

   uint32 getRGBAPack() const
    {
       return ( uint32( red )   <<  0 ) |
              ( uint32( green ) <<  8 ) |
              ( uint32( blue )  << 16 ) |
              ( uint32( alpha ) << 24 );
    }

   uint32 getABGRPack() const
    {
       return (uint32(alpha) << 24) |
              (uint32(blue)  << 16) |
              (uint32(green) <<  8) |
              (uint32(red)   <<  0);
    }


   uint32 getBGRPack() const
    {
       return (uint32(blue)  << 16) |
              (uint32(green) <<  8) |
              (uint32(red)   <<  0);
    }

   uint32 getRGBPack() const
    {
       return (uint32(red)   << 16) |
              (uint32(green) <<  8) |
              (uint32(blue)  <<  0);
    }


   uint32 getRGBEndian() const
    {
    #if defined(BIG_ENDIAN)
          return(getRGBPack());
    #else
          return(getBGRPack());
    #endif
    }

   uint32 getARGBEndian() const
    {
    #if defined(BIG_ENDIAN)
       return(getABGRPack());
    #else
       return(getARGBPack());
    #endif
    }


   uint16 get565()  const
    {
       return uint16((uint16(red   >> 3) << 11) |
                  (uint16(green >> 2) <<  5) |
                  (uint16(blue  >> 3) <<  0));
    }

   uint16 get4444() const
    {
       return uint16(uint16(uint16(alpha >> 4) << 12) |
                  uint16(uint16(red   >> 4) <<  8) |
                  uint16(uint16(green >> 4) <<  4) |
                  uint16(uint16(blue  >> 4) <<  0));
    }

   operator Color() const
    {
       const float32 inv255 = 1.0f / 255.0f;

       return Color(float32(red)   * inv255,
                     float32(green) * inv255,
                     float32(blue)  * inv255);
    }


   operator const uint8*() const { return &red; }
};
