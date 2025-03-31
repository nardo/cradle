
/// Represents a word of bits.
///
/// A helper class to perform operations on a set of 32 bits.
class bit_set
{
private:
   uint32 _bits;

public:
   /// Default constructor initializes this bit set to all zeros.
   bit_set() { _bits = 0; }

   /// Copy constructor.
   bit_set(const bit_set& copy) { _bits = copy._bits; }

   /// Construct from an input uint32.
   bit_set(const uint32 in_mask) { _bits = in_mask; }

   /// Returns the uint32 representation of the bit set.
   operator uint32() const { return _bits; }

   /// Returns the uint32 representation of the bit set.
   uint32 get_mask() const { return _bits; }

   /// Sets all the bits in the bit set to 1
   void set() { _bits  = 0xFFFFFFFFUL; }

   /// Sets all the bits in the bit set that are set in m.
   void set(const uint32 m) { _bits |= m; }
    void set_index(uint32 index) { set(1 << index); }

   /// For each bit set in s, sets or clears that bit in this, depending on whether b is true or false.
   void set(bit_set s, bool b) { _bits = (_bits&~(s._bits))|(b?s._bits:0); }

   /// Clears all the bits in the bit set to 0.
   void clear() { _bits  = 0; }

   /// Clears all the bits in the bit set that are set in m.
   void clear(const uint32 m) { _bits &= ~m; }
    
    /// Clears the bit at the specified index
    void clear_index(uint32 index) { _bits &= ~(1 << index); }

   /// Flips all the bits in the bit set that are set in m.
   void toggle(const uint32 m) { _bits ^= m; }

   /// Test if the passed bits are set.
   bool test(const uint32 m) const { return (_bits & m) != 0; }
    
    bool test_index(const uint32 index) const { return (_bits & (1 << index)) != 0; }

   /// Test if the passed bits and only the passed bits are set.
   bool test_strict(const uint32 m) const { return (_bits & m) == m; }

   bit_set& operator =(const uint32 m) { _bits  = m;  return *this; }
   bit_set& operator|=(const uint32 m) { _bits |= m; return *this; }
   bit_set& operator&=(const uint32 m) { _bits &= m; return *this; }
   bit_set& operator^=(const uint32 m) { _bits ^= m; return *this; }

   bit_set operator|(const uint32 m) const { return bit_set(_bits | m); }
   bit_set operator&(const uint32 m) const { return bit_set(_bits & m); }
   bit_set operator^(const uint32 m) const { return bit_set(_bits ^ m); }
};
