/* ***************************************************************************************
* FILE:          StrUtf8.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  StrUtf8.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef _StrUtf8__H
#define _StrUtf8__H

//lint -sem(hmibase::util::cStrUtf8::Init,initializer)
#include <string>

typedef unsigned char  tU8;

namespace hmibase {
namespace util {

#define tUniCode32 unsigned long
typedef size_t cUtf8StringIter;

#define _STRUTF8MAXLEN 20000

inline const tU8* IMP_ASCII_STR(const char* p)
{
   return (const tU8*)p;
}


class cStrUtf8
{
   public:
      // Constructors
      cStrUtf8();
      cStrUtf8(const cStrUtf8& stringSrc);
      cStrUtf8(const tU8* pszUtf8);
      cStrUtf8(const cStrUtf8& stringSrc, int nCharacters);
      explicit cStrUtf8(tUniCode32 code);
      explicit cStrUtf8(const std::string& str);

      virtual ~cStrUtf8();

      // Attributes & Operations, access to implementation
      size_t Length() const
      {
         return pMgt()->_nStrLen;
      }
      size_t AllocLength() const
      {
         return pMgt()->_nAllocLen;
      }
      bool IsEmpty() const
      {
         return pMgt()->_nStrLen == 0;
      }
      int  Characters() const;
      void FreeUnusedSpace();
      size_t GetByteLengthOfUniCodeChar(tUniCode32 code);

      tUniCode32 at(int CharacterPos) const;
      tUniCode32 at(cUtf8StringIter iter) const;
      tUniCode32 operator[](int CharacterPos) const; // same as GetAt
      tUniCode32 operator[](cUtf8StringIter iter) const; // same as GetAt

      // data getter
      const char* c_str() const
      {
         return (const char*)_pUtf8String;
      }
      const tU8* data() const
      {
         return _pUtf8String;
      }
      operator const tU8* () const
      {
         return (const tU8*)_pUtf8String;
      }
      operator const std::string() const
      {
         return std::string((const char*)_pUtf8String);
      }

      // set functions
      void Clear();

      // Assignment
      void Format(const char* lpszFormat, ...);  // sprintf
      void SetHex(const tU8* pszUtf8, bool bWithBlanks = true);
      void SetUtf8(const tU8* pszUtf8);
      void SetUtf16(const unsigned short* puw16BStr);

      // overloaded assignment
      cStrUtf8& operator=(const cStrUtf8& stringSrc);
      const cStrUtf8& operator=(const std::string& stringSrc);
      const cStrUtf8& operator=(const tU8* pszUtf8);

      // append, concatenation
      const cStrUtf8& operator+=(const cStrUtf8& string);
      const cStrUtf8& operator+=(const tU8* pszUtf8);
      const cStrUtf8& operator+=(tUniCode32 code);

      friend cStrUtf8 operator+(const cStrUtf8& string1, const cStrUtf8& string2);
      friend cStrUtf8 operator+(const cStrUtf8& string, tUniCode32 code);
      friend cStrUtf8 operator+(tUniCode32 code, const cStrUtf8& string);
      friend cStrUtf8 operator+(const cStrUtf8& string, const tU8* pszUtf8);
      friend cStrUtf8 operator+(const tU8* pszUtf8, const cStrUtf8& string);

      void Append(const tU8* pszUtf8)
      {
         ConcatInPlace(cStrUtf8::u8StrLen(pszUtf8), pszUtf8);
      }
      void Append(const cStrUtf8& str)
      {
         ConcatInPlace(str.Length(), str.data());
      }
      void Append(const cStrUtf8& strSrc, int nCharacters)
      {
         *this += strSrc.Left(nCharacters);
      }

      // string comparison
      bool bIsEqual(const cStrUtf8& str) const
      {
         return Compare(str) == 0;
      }
      bool bIsEqual(const tU8* pszStr) const
      {
         return Compare(pszStr) == 0;
      }
      int Compare(const cStrUtf8& oOtherString) const;
      int Compare(const tU8* pszUtf8) const;

      // std::sort(&sArray[0],&sArray[n], cStrUtf8::StrUtf8SortFunc)
      static bool StrUtf8SortFunc(const cStrUtf8& a, const cStrUtf8& b);

      // Iterators
      cUtf8StringIter itBegin() const;
      cUtf8StringIter itEnd() const;
      cUtf8StringIter itNext(cUtf8StringIter iter) const;
      cUtf8StringIter itPrev(cUtf8StringIter iter) const;
      cUtf8StringIter itSeek(int nCharacters) const;

      // constructed external string objects ....
      cStrUtf8 substr(cUtf8StringIter iter = 0, int nCharacters = _STRUTF8MAXLEN) const;
      cStrUtf8 substr(int CharacterPos = 0, int nCharacters = _STRUTF8MAXLEN) const;
      cStrUtf8 Left(int nCharacters) const;
      cStrUtf8 Mid(cUtf8StringIter iter, int nCharacters) const;
      cStrUtf8 Mid(int CharacterPos, int nCharacters) const;
      cStrUtf8 Right(int nCharacters) const;

      // upper/lower conversion
      void MakeUpper();
      void MakeLower();

      // searching (return starting index, or -1 if not found)
      cUtf8StringIter itFind(tUniCode32 code) const;
      cUtf8StringIter itFind(tUniCode32 code, cUtf8StringIter iter = 0) const;
      cUtf8StringIter itFind(const cStrUtf8& oSearch, int nCharacters) const;

      // insert, delete
      void DeleteLastCharacter();
      void DeleteCharacter(cUtf8StringIter itPosition);
      void Insert(cUtf8StringIter itPosition, const cStrUtf8& str);
      void SetAt(cUtf8StringIter itPosition, tUniCode32 code);
      void SetAt(int CharacterPos, tUniCode32 code);
      // trimming whitespace
      void TrimRight();
      void TrimLeft();

      void MakeCopyBeforeModify(size_t newlen); // GetBuffer, Realloc
      bool CheckValid() const;
      bool FixString();

   public:
      struct cDataHelper
      {
         long	  _nRefCnt;
         size_t  _nStrLen;
         size_t  _nAllocLen;
         // tU8  _data[/*_nAllocLen*/]
         cDataHelper() : _nRefCnt(-1), _nStrLen(0), _nAllocLen(0) {}
         tU8* data()
         {
            return (tU8*)(this + 1);
         }
      };

      typedef enum
      {
         DEFAULT = 0,
         GER = 1,
         ENG_US = 2,
         ESP_LAT = 3,
         FRA_CAN = 4,
         POR = 5,
         ITA = 6,
         CZE = 7,
         DUT = 8,
         TUR = 9,
         RUS = 10,
         ENG = 11,
         FRA = 12,
         ESP = 13,
         DAN = 14,
         SWE = 15,
         FIN = 16,
         NOR = 17,
         POL = 18,
         SLO = 19,
         HUN = 20,
         GRE = 21,
         BRA = 22,
         ARA = 23,
         THA = 24,
         AUS = 25
      } tenLanguage;

      static bool setLanguageForSort(cStrUtf8::tenLanguage language);
      static tenLanguage _lang;
      static unsigned fact_LATIN, fact_RUS, fact_GRE, fact_THA, fact_ARA;

      long getRefCnt() const
      {
         return pMgt()->_nRefCnt;
      }

   protected:
      // implementation helpers
      void Init();
      cStrUtf8::cDataHelper* pMgt() const
      {
         cStrUtf8::cDataHelper* p = (cStrUtf8::cDataHelper*)_pUtf8String;
         return (p - 1);
      }
   private:
      cDataHelper* AllocMem(size_t nLen);
      void AllocBeforeWrite(size_t nLen);
      void MakeCopyBeforeModify();
      void CreateCopyFromThis(cStrUtf8& dest, cUtf8StringIter nStart, size_t nLength) const;
      void AssignCopy(size_t nSrcLen, const tU8* lpszSrcData);
      void AssignStringPtr(tU8* s);
      void ConcatCopy(size_t nSrc1Len, const tU8* lpszSrc1Data, size_t nSrc2Len, const tU8* lpszSrc2Data);
      void ConcatInPlace(size_t nSrcLen, const tU8* lpszSrcData);
      cUtf8StringIter StrStrIter(const cStrUtf8& oSearch, size_t nCharacters) const;
      tUniCode32 GetWeightedValueFromUniCode(tUniCode32 code) const;
      void Release();
      void UTF8_LOCK() {}
      void UTF8_UNLOCK() {}
   private:
      static bool Release(cStrUtf8::cDataHelper* pData);
      static size_t u8StrLen(const tU8* pszUtf8);
   private:
      tU8* _pUtf8String;   // ptr to ref counted string data
};


// Compare helpers
inline bool  operator==(const cStrUtf8& s1, const cStrUtf8& s2)
{
   return s1.Compare(s2) == 0;
}


inline bool  operator==(const cStrUtf8& s1, const tU8* s2)
{
   return s1.Compare(s2) == 0;
}


inline bool  operator==(const tU8* s1, const cStrUtf8& s2)
{
   return s2.Compare(s1) == 0;
}


inline bool  operator!=(const cStrUtf8& s1, const cStrUtf8& s2)
{
   return s1.Compare(s2) != 0;
}


inline bool  operator!=(const cStrUtf8& s1, const tU8* s2)
{
   return s1.Compare(s2) != 0;
}


inline bool  operator!=(const tU8* s1, const cStrUtf8& s2)
{
   return s2.Compare(s1) != 0;
}


inline bool  operator<(const cStrUtf8& s1, const cStrUtf8& s2)
{
   return s1.Compare(s2) < 0;
}


inline bool  operator<(const cStrUtf8& s1, const tU8* s2)
{
   return s1.Compare(s2) < 0;
}


inline bool  operator<(const tU8* s1, const cStrUtf8& s2)
{
   return s2.Compare(s1) > 0;
}


inline bool  operator>(const cStrUtf8& s1, const cStrUtf8& s2)
{
   return s1.Compare(s2) > 0;
}


inline bool  operator>(const cStrUtf8& s1, const tU8* s2)
{
   return s1.Compare(s2) > 0;
}


inline bool  operator>(const tU8* s1, const cStrUtf8& s2)
{
   return s2.Compare(s1) < 0;
}


inline bool  operator<=(const cStrUtf8& s1, const cStrUtf8& s2)
{
   return s1.Compare(s2) <= 0;
}


inline bool  operator<=(const cStrUtf8& s1, const tU8* s2)
{
   return s1.Compare(s2) <= 0;
}


inline bool  operator<=(const tU8* s1, const cStrUtf8& s2)
{
   return s2.Compare(s1) >= 0;
}


inline bool  operator>=(const cStrUtf8& s1, const cStrUtf8& s2)
{
   return s1.Compare(s2) >= 0;
}


inline bool  operator>=(const cStrUtf8& s1, const tU8* s2)
{
   return s1.Compare(s2) >= 0;
}


inline bool  operator>=(const tU8* s1, const cStrUtf8& s2)
{
   return s2.Compare(s1) <= 0;
}


}
}


#endif
