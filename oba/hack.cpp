#define WIN32_LEAN_AND_MEAN
#include "windows.h"

//
// Crap to make the Lib link with the new VS8.
//
namespace std
{
   //
   // From old VS7 xstring.
   //
   class _CRTIMP2 _String_base
   {
    public:
      void _Xlen() const;
      void _Xran() const;
   };

   //
   // From old VS7 string.cpp
   //
   _CRTIMP2 void _String_base::_Xlen() const
   {
      throw("string too long");
   }

   _CRTIMP2 void _String_base::_Xran(void) const
   {
      throw("invalid string position");
   }

  //--------------------

  // that stuff  is strange ;} it dont hack anything ;{
  //
  template<class _Elem,   class _Traits>
   class _CRTIMP2 basic_streambuf
   {   
  };

  template<class _Elem>
   struct _CRTIMP2 char_traits
  {
  };

  class _CRTIMP2 ios_base
  {
   public:
   void _Addstd();
  };

  template<class _Elem,
   class _Traits>
  class _CRTIMP2 basic_ios : public ios_base
  {
    public:
    typedef basic_streambuf<char, char_traits<char> > _Mysb;
    static void init(_Mysb *_Strbuf = 0,   bool _Isstd = false)
    {
       if (_Isstd) _Addstd();   // special handling for standard streams
       else throw("init stuff.. ;}~");
    }
  };

   _CRTIMP2 void ios_base::_Addstd(void)
   {
      throw("_Addstd stuff.. ;}~");
   } 

  /*
  "void std::ios_base::_Addstd(void)" referenced in function
  "std::basic_ios<char,struct std::char_traits<char> >::init(class std::basic_streambuf<char,struct std::char_traits<char> > *,bool)"
  */
  //basic_streambuf<char,char_traits<char> > * x;
  //basic_ios<char,char_traits<char> > b;
  //b.init(x);
}
