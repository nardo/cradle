// The math library - copyright KAGR LLC, portions copyright Team Nutshell. This library is derived largely from nml - the C++ Nutshell Mathematics Library for Computer Graphics (https://team-nutshell.github.io/nml/). The use of this sorce code is governed by the MIT software license described in the "license.txt" file in this directory.

static inline uint32 RandomInt(uint32 range)
{
   return uint32((uint32(rand()) / (float32(RAND_MAX) + 1)) * range);
}

static inline uint32 RandomBool()
{
   return (uint32(rand()) / (float32(RAND_MAX) + 1)) > 0.5f;
}

