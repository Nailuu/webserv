#pragma once

#include "Enum.hpp"

class MimeType: public Enum<std::string>
{
    private:
        static std::vector<MimeType*> _types;
        std::vector<std::string> _extensions;
    public:
        MimeType();
        MimeType(const std::string &key, const std::string &value, const std::string *extensions, size_t ext_count);
        ~MimeType();
        MimeType(const MimeType &other);
        MimeType operator=(const MimeType &other);
        static bool addTypes(void);
        static const MimeType &get(const std::string &key);
        static const MimeType &getByValue(const std::string &value);
        static const MimeType &getByExtension(const std::string &extension);
        static const MimeType TEXT_PLAIN;
        static const MimeType TEXT_HTML;
        static const MimeType IMAGE_JPEG;
        static const MimeType IMAGE_PNG;
        static const MimeType IMAGE_SVG;
        static const MimeType APPLICATION_JSON;
        static const MimeType APPLICATION_XML;
        static const MimeType APPLICATION_PDF;
        static const MimeType APPLICATION_ZIP;
        static const MimeType APPLICATION_OCTET_STREAM;
};
