#include "MimeType.hpp"
#include <stdexcept>
#include <iostream>

std::vector<MimeType *> MimeType::_types;

const std::string TEXT_PLAIN_EXT[] = {"txt"};
const std::string TEXT_HTML_EXT[] = {"html"};
const std::string IMAGE_JPEG_EXT[] = {"jpg", "jpeg"};
const std::string IMAGE_PNG_EXT[] = {"png"};
const std::string APPLICATION_JSON_EXT[] = {"json"};
const std::string APPLICATION_XML_EXT[] = {"xml"};
const std::string APPLICATION_PDF_EXT[] = {"pdf"};
const std::string APPLICATION_ZIP_EXT[] = {"zip"};

const MimeType MimeType::TEXT_PLAIN("TEXT_PLAIN", "text/plain", TEXT_PLAIN_EXT, sizeof(TEXT_PLAIN_EXT) / sizeof(TEXT_PLAIN_EXT[0]));
const MimeType MimeType::TEXT_HTML("TEXT_HTML", "text/html", TEXT_HTML_EXT, sizeof(TEXT_HTML_EXT) / sizeof(TEXT_HTML_EXT[0]));
const MimeType MimeType::IMAGE_JPEG("IMAGE_JPEG", "image/jpeg", IMAGE_JPEG_EXT, sizeof(IMAGE_JPEG_EXT) / sizeof(IMAGE_JPEG_EXT[0]));
const MimeType MimeType::IMAGE_PNG("IMAGE_PNG", "image/png", IMAGE_PNG_EXT, sizeof(IMAGE_PNG_EXT) / sizeof(IMAGE_PNG_EXT[0]));
const MimeType MimeType::APPLICATION_JSON("APPLICATION_JSON", "application/json", APPLICATION_JSON_EXT, sizeof(APPLICATION_JSON_EXT) / sizeof(APPLICATION_JSON_EXT[0]));
const MimeType MimeType::APPLICATION_XML("APPLICATION_XML", "application/xml", APPLICATION_XML_EXT, sizeof(APPLICATION_XML_EXT) / sizeof(APPLICATION_XML_EXT[0]));
const MimeType MimeType::APPLICATION_PDF("APPLICATION_PDF", "application/pdf", APPLICATION_PDF_EXT, sizeof(APPLICATION_PDF_EXT) / sizeof(APPLICATION_PDF_EXT[0]));
const MimeType MimeType::APPLICATION_ZIP("APPLICATION_ZIP", "application/zip", APPLICATION_ZIP_EXT, sizeof(APPLICATION_ZIP_EXT) / sizeof(APPLICATION_ZIP_EXT[0]));

bool MimeType::addTypes(void)
{
    _types.push_back((MimeType *)&TEXT_PLAIN);
    _types.push_back((MimeType *)&TEXT_HTML);
    _types.push_back((MimeType *)&IMAGE_JPEG);
    _types.push_back((MimeType *)&IMAGE_PNG);
    _types.push_back((MimeType *)&APPLICATION_JSON);
    _types.push_back((MimeType *)&APPLICATION_XML);
    _types.push_back((MimeType *)&APPLICATION_PDF);
    _types.push_back((MimeType *)&APPLICATION_ZIP);
    return (true);
}

const MimeType &MimeType::get(const std::string &key)
{
    std::vector<MimeType *>::iterator it = _types.begin();

    for (; it != _types.end(); it++)
    {
        MimeType *type = *it;

        if (type->_key == key)
        {
            return (*type);
        }
    }

    throw EnumException("Cannot get from key: '" + highlight(key) + "'");
}

const MimeType &MimeType::getByValue(const std::string &value)
{
    std::vector<MimeType *>::iterator it = _types.begin();

    for (; it != _types.end(); it++)
    {
        MimeType *type = *it;

        if (type->_value == value)
        {
            return (*type);
        }
    }

    throw EnumException("Cannot get from value: '" + highlight(value) + "'");
}

const MimeType &MimeType::getByExtension(const std::string &extension)
{
    std::vector<MimeType *>::iterator it = _types.begin();

    for (; it != _types.end(); it++)
    {
        MimeType *type = *it;

        std::vector<std::string>::iterator it2 = type->_extensions.begin();

        for (; it2 != type->_extensions.end(); it2++)
        {
            if ((*it2) == extension)
            {
                return (*type);
            }
        }
    }

    throw EnumException("Cannot get from extension: '" + highlight(extension) + "'");
}

static bool added = MimeType::addTypes();

MimeType::MimeType(void) : Enum() {}

MimeType::MimeType(const std::string &key, const std::string &value, const std::string *extensions, size_t ext_count) : Enum(key, value)
{
    for (unsigned int i = 0; i < ext_count; i++)
    {
        _extensions.push_back(extensions[i]);
    }
}

MimeType::~MimeType() {}

MimeType::MimeType(const MimeType &other) : Enum(other) {}

MimeType MimeType::operator=(const MimeType &other)
{
    if (this != &other)
    {
        _key = other._key;
        _value = other._value;
    }

    return (*this);
}
