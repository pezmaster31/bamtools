#ifndef IBAMIODEVICE_H
#define IBAMIODEVICE_H

#include <api/api_global.h>
#include <string>

namespace BamTools {

class API_EXPORT IBamIODevice {

    // enums
    public: enum OpenMode { NotOpen = 0
                          , ReadOnly
                          , WriteOnly
                          };

    // ctor & dtor
    public:
        IBamIODevice(void);
        virtual ~IBamIODevice(void);

    // IBamIODevice interface
    public:

        // pure virtuals
        virtual void Close(void) =0;
        virtual bool IsRandomAccess(void) const =0;
        virtual bool Open(const OpenMode mode) =0;
        virtual size_t Read(char* data, const unsigned int numBytes) =0;
        virtual bool Seek(const int64_t& position) =0;
        virtual int64_t Tell(void) const =0;
        virtual size_t Write(const char* data, const unsigned int numBytes) =0;

        // default implementation provided
        virtual std::string ErrorString(void);
        virtual bool IsOpen(void) const;
        virtual OpenMode Mode(void) const;

    // internal methods
    protected:
        void SetErrorString(const std::string& errorString);

    // data members
    protected:
        OpenMode    m_mode;
        std::string m_errorString;
};

inline
IBamIODevice::IBamIODevice(void)
    : m_mode(IBamIODevice::NotOpen)
{ }

inline
IBamIODevice::~IBamIODevice(void) { }

inline
std::string IBamIODevice::ErrorString(void) {
    std::string e = m_errorString;
    m_errorString.clear();
    return e;
}

inline
bool IBamIODevice::IsOpen(void) const {
    return ( m_mode != IBamIODevice::NotOpen );
}

inline
IBamIODevice::OpenMode IBamIODevice::Mode(void) const {
    return m_mode;
}

inline
void IBamIODevice::SetErrorString(const std::string& errorString) {
    m_errorString = errorString;
}

} // namespace BamTools

#endif // IBAMIODEVICE_H
