#include "dted/dtedParsedStructs.h"

#include <string>
#include <iostream>

template <size_t S>
static std::string toString(const std::array<std::byte, S>& bytes, size_t offset, size_t size) {
    return std::string(reinterpret_cast<const char*>(bytes.data() + offset), size);
}

namespace dted {

UserHeaderLabel::UserHeaderLabel(const UserHeaderLabelBlob& uhlBlob)
{
    // recognitionSentinel
    recognitionSentinel = std::string(reinterpret_cast<const char*>(uhlBlob.recognitionSentinel.data()), 3);

    // fixedbyStandard
    fixedByStandard = static_cast<uint8_t>(uhlBlob.fixedStandard[0]);

    // longitudeOfOrigin
    longitudeOfOrigin = std::stod(toString(uhlBlob.longitudeOfOrigin, 0, 3) + "." + toString(uhlBlob.longitudeOfOrigin, 3, 4));
    if (uhlBlob.longitudeOfOrigin[7] == (std::byte)'W') {
        longitudeOfOrigin *= -1;
    }
    if (longitudeOfOrigin >= 180.0 || longitudeOfOrigin <= -180.0) {
        std::cerr << "WARNING UserHeaderLabel() Longitude of origin has invalid range. [-180.0, 180.0]: " << longitudeOfOrigin << std::endl;
    }

    // latitudeOfOrigin
    std::string latitudeOfOriginString = std::string(reinterpret_cast<const char*>(uhlBlob.latitudeOfOrigin.data()), 3)
        + "." + std::string(reinterpret_cast<const char*>(uhlBlob.latitudeOfOrigin.data() + 3), 4);
    latitudeOfOrigin = std::stod(latitudeOfOriginString);
    if (uhlBlob.latitudeOfOrigin[7] == (std::byte)'S') {
        latitudeOfOrigin *= -1;
    }
    if (latitudeOfOrigin >= 90.0 || latitudeOfOrigin <= -90.0) {
        std::cerr << "WARNING UserHeaderLabel() Latitude of origin has invalid range. [-90.0, 90.0]: " << latitudeOfOrigin << std::endl;
    }

    // longitudeIntervalArcSeconds
    longitudeIntervalArcSeconds = std::stoi(toString(uhlBlob.longitudeIntervalArcSeconds, 0, 4));

    // latitudeIntervalArcSeconds
    latitudeIntervalArcSeconds = std::stoi(toString(uhlBlob.latitudeIntervalArcSeconds, 0, 4));

    // absoluteVerticalAccuracy
    std::string absoluteVerticalAccuracyString = toString(uhlBlob.absoluteVerticalAccuracy, 0, 4);
    if (absoluteVerticalAccuracyString != "NA$$") {
        absoluteVerticalAccuracy = std::stoi(absoluteVerticalAccuracyString);
    }

    // unclassifiedSecurityCode
    unclassifiedSecurtiyCode = (reinterpret_cast<const char*>(uhlBlob.securityCode.data()))[0];

    // uniqueReferenceNumber
    uniqueReferenceNumber = toString(uhlBlob.uniqueReference, 0, 12);

    // numberOfLongitudeLines
    numberOfLongitudeLines = std::stoi(toString(uhlBlob.numberOfLongitudeLines, 0, 4));

    // numberOfLatitudePoints
    numberOfLatitudePoints = std::stoi(toString(uhlBlob.numberOfLatitudePoints, 0, 4));

    // multipleAccuracy
    int multipleAccuracyInt = std::stod(toString(uhlBlob.multipleAccuracy, 0, 1));
    multipleAccuracy = multipleAccuracyInt > 0;
}

} // End dted namespace