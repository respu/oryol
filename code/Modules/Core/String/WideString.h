#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::WideString
    @brief wide-character version of String class 
    
    An immutable, shared wchar_t string class. Depending on platform,
    this means UTF-16 (in Windows) or UTF-32 (everywhere else).
    
    See String class for details.
    
    NOTE: usually one should work with UTF-8 string, and only convert 
    from and to WideString when needed. Use the StringUtil class to 
    convert between UTF encodings.
*/
#include <atomic>
#include <string>
#include "Core/Types.h"
#include "Core/Assert.h"

namespace Oryol {
namespace Core {
        
class StringAtom;
        
class WideString {
public:
    /// default constructor
    WideString();
    /// construct from wchar_t string (allocates!)
    WideString(const wchar_t* str);
    /// construct from raw wchar_t sequence
    WideString(const wchar_t* ptr, int32 numChars);
    /// construct from std::wstring (allocates!)
    WideString(const std::wstring& str);
    
    /// copy constructor (does not allocate)
    WideString(const WideString& rhs);
    /// move constructor (does not allocate)
    WideString(WideString&& rhs);
    
    /// assign from wchar_t string (allocates!)
    void operator=(const wchar_t* cstr);
    /// assign from std::wstring (allocates!)
    void operator=(const std::wstring& str);
    /// copy-assign from other WideString (does not allocate)
    void operator=(const WideString& rhs);
    /// move-assign from other WideString (does not allocate)
    void operator=(WideString&& rhs);
    
    /// equality operator
    bool operator==(const WideString& rhs) const;
    /// inequality operator
    bool operator!=(const WideString& rhs) const;
    /// less-than operator
    bool operator<(const WideString& rhs) const;
    /// greater-than operator
    bool operator>(const WideString& rhs) const;
    /// less-equal operator
    bool operator<=(const WideString& rhs) const;
    /// greater-equal operator
    bool operator>=(const WideString& rhs) const;
    
    /// assign from raw byte sequence, if numChars is 0 -> until first 0
    void Assign(const wchar_t* ptr, int32 numChars);
    /// get as C-String, will always return a valid ptr, even if String is empty
    const wchar_t* AsCStr() const;
    /// get as std::wstring (slow)
    std::wstring AsStdWString() const;
    
    /// get string length in number of wchar_t
    int32 Length() const;
    /// get byte length (Length() * sizeof(wchar_t)
    int32 ByteLength() const;
    /// return true if contains a non-empty string
    bool IsValid() const;
    /// return true if empty
    bool Empty() const;
    /// clear content
    void Clear();
    /// get the refcount of this string
    int32 RefCount() const;
    
private:
    /// shared string data header, this is followed by the actual string
    struct StringData {
        #if ORYOL_HAS_ATOMIC
        std::atomic<int32> refCount{0};
        #else
        int32 refCount{0};
        #endif
        int32 length;
    };
        
    /// create new string data block, len is number of characters (excluding 0 terminator
    void create(const wchar_t* ptr, int32 len);
    /// destroy shared string data block
    void destroy();
    /// increment refcount
    void addRef();
    /// decrement refcount, call destroy if 0
    void release();
    
    StringData* data;
    const wchar_t* strPtr;
    static const wchar_t* emptyString;
};

//------------------------------------------------------------------------------
bool operator==(const WideString& s0, const wchar_t* s1);
bool operator!=(const WideString& s0, const wchar_t* s1);
bool operator<(const WideString& s0, const wchar_t* s1);
bool operator>(const WideString& s0, const wchar_t* s1);
bool operator<=(const WideString& s0, const wchar_t* s1);
bool operator>=(const WideString& s0, const wchar_t* s1);
bool operator==(const wchar_t* s0, const WideString& s1);
bool operator!=(const wchar_t* s0, const WideString& s1);
bool operator<(const wchar_t* s0, const WideString& s1);
bool operator>(const wchar_t* s0, const WideString& s1);
bool operator<=(const wchar_t* s0, const WideString& s1);
bool operator>=(const wchar_t* s0, const WideString& s1);
bool operator==(const WideString& s0, const std::wstring& s1);
bool operator!=(const WideString& s0, const std::wstring& s1);
bool operator<(const WideString& s0, const std::wstring& s1);
bool operator>(const WideString& s0, const std::wstring& s1);
bool operator<=(const WideString& s0, const std::wstring& s1);
bool operator>=(const WideString& s0, const std::wstring& s1);
bool operator==(const std::wstring& s0, const WideString& s1);
bool operator!=(const std::wstring& s0, const WideString& s1);
bool operator<(const std::wstring& s0, const WideString& s1);
bool operator>(const std::wstring& s0, const WideString& s1);
bool operator<=(const std::wstring& s0, const WideString& s1);
bool operator>=(const std::wstring& s0, const WideString& s1);

} // namespace Core
} // namespace Oryol
