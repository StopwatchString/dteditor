#include "dted/dtedParsedStructs.h"

#include <string>
#include <iostream>
#include <concepts>

template <size_t S>
static std::string toString(const std::array<std::byte, S>& bytes, size_t offset, size_t size) {
    return std::string(reinterpret_cast<const char*>(bytes.data() + offset), size);
}

template<std::integral T, size_t S>
static T toIntegral(const std::array<std::byte, S>& bytes, size_t offset, size_t size) {
    T out = static_cast<T>(0);
    for (uint32_t i = 0; i < size; i++) {
        out *= static_cast<T>(10);
        out += static_cast<T>(bytes[i + offset]) - static_cast<T>('0');
    }
    return out;
}

template<std::floating_point T, size_t S>
static T toFloating(const std::array<std::byte, S>& bytes, size_t offset, size_t size, size_t decimalAfterIndex) {
    if (decimalAfterIndex > size) {
        std::cout << "WARNING toFloating() Decimal is set to occur outside of designated size." << std::endl;
        decimalAfterIndex = size;
    }

    T out = static_cast<T>(0);
    uint32_t index = 0;
    // Handle whole number values
    while (index < decimalAfterIndex) {
        out *= static_cast<T>(10.0);
        out += static_cast<T>(bytes[index + offset]) - static_cast<T>('0');
        index++;
    }

    // Handle after the decimal place
    int decimalIndex = offset + size - 1;
    T decimal = static_cast<T>(0.0);
    while (index < decimalIndex) {
        decimal += static_cast<T>(bytes[decimalIndex + offset]) - static_cast<T>('0');
        decimal *= static_cast<T>(0.1);
        decimalIndex--;
    }

    return out + decimal;
}

template<std::floating_point T, size_t S>
static T toFloatingDecimalPresent(const std::array<std::byte, S>& bytes, size_t offset, size_t size) {
    T out = static_cast<T>(0);
    uint32_t index = 0;
    // Handle whole number values
    while (index < size && bytes[index + offset] != static_cast<std::byte>('.')) {
        out *= static_cast<T>(10.0);
        out += static_cast<T>(bytes[index + offset]) - static_cast<T>('0');
        index++;
    }

    // Handle after the decimal place
    int decimalIndex = offset + size - 1;
    T decimal = static_cast<T>(0.0);
    while (index < decimalIndex) {
        decimal += static_cast<T>(bytes[decimalIndex + offset]) - static_cast<T>('0');
        decimal *= static_cast<T>(0.1);
        decimalIndex--;
    }

    return out + decimal;
}

namespace dted {


UserHeaderLabel::UserHeaderLabel(const UserHeaderLabelBlob& uhlBlob)
{
    // recognitionSentinel
    recognitionSentinel = toString(uhlBlob.recognitionSentinel, 0, 3);

    // fixedbyStandard
    fixedByStandard = static_cast<uint8_t>(uhlBlob.fixedStandard[0]);

    // longitudeOfOrigin
    longitudeOfOrigin = toFloating<double>(uhlBlob.longitudeOfOrigin, 0, 7, 3);
    if (uhlBlob.longitudeOfOrigin[7] == (std::byte)'W') {
        longitudeOfOrigin *= -1;
    }
    if (longitudeOfOrigin >= 180.0 || longitudeOfOrigin <= -180.0) {
        std::cerr << "WARNING UserHeaderLabel() Longitude of origin has invalid range. [-180.0, 180.0]: " << longitudeOfOrigin << std::endl;
    }
    // latitudeOfOrigin
    latitudeOfOrigin = toFloating<double>(uhlBlob.latitudeOfOrigin, 0, 7, 3);
    if (uhlBlob.latitudeOfOrigin[7] == (std::byte)'S') {
        latitudeOfOrigin *= -1;
    }
    if (latitudeOfOrigin >= 90.0 || latitudeOfOrigin <= -90.0) {
        std::cerr << "WARNING UserHeaderLabel() Latitude of origin has invalid range. [-90.0, 90.0]: " << latitudeOfOrigin << std::endl;
    }

    // longitudeIntervalArcSeconds
    longitudeIntervalArcSeconds = toIntegral<uint16_t>(uhlBlob.longitudeIntervalArcSeconds, 0, 4);

    // latitudeIntervalArcSeconds
    latitudeIntervalArcSeconds = toIntegral<uint16_t>(uhlBlob.latitudeIntervalArcSeconds, 0, 4);

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
    numberOfLongitudeLines = toIntegral<uint16_t>(uhlBlob.numberOfLongitudeLines, 0, 4);

    // numberOfLatitudePoints
    numberOfLatitudePoints = toIntegral<uint16_t>(uhlBlob.numberOfLatitudePoints, 0, 4);

    // multipleAccuracy
    multipleAccuracy = static_cast<int>(static_cast<char>(uhlBlob.multipleAccuracy[0]) - '0') > 0;
}

DataSetIdentification::DataSetIdentification(const DataSetIdentificationBlob& dsiBlob)
{
    //recognitionSentinel
    recognitionSentinel = toString(dsiBlob.recognitionSentinel, 0, 3);

    //securityCode
    securityCode = static_cast<char>(dsiBlob.securityCode[0]);

    //securityControlMarkings
    securityControlMarkings = toString(dsiBlob.securityControlMarkings, 0, 2);

    //securityHandlingDescription
    securityHandlingDescription = toString(dsiBlob.securityHandlingDescription, 0, 27);

    //dtedLevel
    dtedLevel = std::stoi(toString(dsiBlob.dtedLevel, 0, 2));

    //uniqueReference
    uniqueReference = toString(dsiBlob.uniqueReference, 0, 15);

    //dataEditionNumber
    dataEditionNumber = std::stoi(toString(dsiBlob.dataEditionNumber, 0, 2));

    //matchMergeVersion
    matchMergeVersion = static_cast<char>(dsiBlob.matchMergeVersion[0]);

    //maintenanceDate
    std::chrono::year maintenanceDateYear(std::stoi(toString(dsiBlob.maintenanceDate, 0, 2)));
    std::chrono::month maintenanceDateMonth(std::stoi(toString(dsiBlob.maintenanceDate, 2, 2)));
    maintenanceDate = std::chrono::year_month(maintenanceDateYear, maintenanceDateMonth);

    //matchMergeDate
    std::chrono::year matchMergeDateYear(std::stoi(toString(dsiBlob.matchMergeDate, 0, 2)));
    std::chrono::month matchMergeDateMonth(std::stoi(toString(dsiBlob.matchMergeDate, 2, 2)));
    maintenanceDate = std::chrono::year_month(matchMergeDateYear, matchMergeDateMonth);

    //maintenanceDescCode
    maintenanceDescCode = std::stoi(toString(dsiBlob.maintenanceDescCode, 0, 4));

    //producerCode
    producerCode = toString(dsiBlob.producerCode, 0, 8);

    //productSpec
    productSpec = toString(dsiBlob.productSpec, 0, 9);

    //productSpecAmendmentNumber
    productSpecAmendmentNumber = std::stoi(toString(dsiBlob.productSpecAmend, 0, 2));

    //productSpecDate
    std::chrono::year productSpecDateYear(std::stoi(toString(dsiBlob.productSpecDate, 0, 2)));
    std::chrono::month productSpecDateMonth(std::stoi(toString(dsiBlob.productSpecDate, 2, 2)));
    productSpecDate = std::chrono::year_month(productSpecDateYear, productSpecDateMonth);

    //verticalDatum
    verticalDatum = toString(dsiBlob.verticalDatum, 0, 3);

    //horizontalDatum
    horizontalDatum = toString(dsiBlob.horizontalDatum, 0, 5);

    //digitizingSystem
    digitizingSystem = toString(dsiBlob.digitizingSystem, 0, 10);

    //compilationDate
    std::chrono::year compilationDateYear(std::stoi(toString(dsiBlob.compilationDate, 0, 2)));
    std::chrono::month compilationDateMonth(std::stoi(toString(dsiBlob.compilationDate, 2, 2)));
    compilationDate = std::chrono::year_month(compilationDateYear, compilationDateMonth);

    //latitudeOfOriginDeg
    latitudeOfOriginDeg = std::stod(
        toString(dsiBlob.latOrigin, 0, 2)
        + "."
        + toString(dsiBlob.latOrigin, 2, 4)
        + toString(dsiBlob.latOrigin, 7, 1)
        );
    if (dsiBlob.latOrigin[8] == static_cast<std::byte>('S')) {
        latitudeOfOriginDeg *= -1.0;
    }

    //longitudeOfOriginDeg
    longitudeOfOriginDeg = std::stod(
        toString(dsiBlob.lonOrigin, 0, 3)
        + "."
        + toString(dsiBlob.lonOrigin, 3, 4)
        + toString(dsiBlob.lonOrigin, 8, 1)
    );
    if (dsiBlob.lonOrigin[9] == static_cast<std::byte>('W')) {
        longitudeOfOriginDeg *= -1.0;
    }

    //latSWCornerDeg
    latSWCornerDeg = std::stod(
        toString(dsiBlob.latSWCorner, 0, 2)
        + "."
        + toString(dsiBlob.latSWCorner, 2, 4)
    );
    if (dsiBlob.latSWCorner[7] == static_cast<std::byte>('S')) {
        latSWCornerDeg *= -1.0;
    }

    //lonSWCornerDeg


    //latNWCornerDeg


    //lonNWCornerDeg


    //latNECornerDeg


    //lonNECornerDeg


    //latSECornerDeg


    //lonSECornerDeg


    //orientationAngle


    //latitudeIntervalArcSeconds


    //longitudeIntervalArcSeconds


    //numberLatitudeLines


    //numberLongitudeLines


    //partialCellIndicator


    //coveragePercent


    //geoidUndulation



}

AccuracyDescriptionRecord::AccuracyDescriptionRecord(const AccuracyDescriptionRecordBlob& accBlob)
{

}

} // End dted namespace