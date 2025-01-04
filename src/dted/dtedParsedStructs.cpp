#include "dted/dtedParsedStructs.h"

#include <string>
#include <iostream>
#include <concepts>
#include <charconv>

template <size_t S>
static void stringFromChars(const std::array<std::byte, S>& bytes, std::string& target)
{
    target.resize(bytes.size());
    memcpy(target.data(), bytes.data(), bytes.size());
}

template <std::integral T, size_t S>
static T integralFromChars(const std::array<std::byte, S>& bytes, size_t offset, size_t size, int base = 10)
{
    T retVal = T();
    std::from_chars_result result = std::from_chars(
        reinterpret_cast<const char*>(&bytes[offset]),
        reinterpret_cast<const char*>(&bytes[offset + size - 1]),
        retVal,
        base
    );
    return T;
}

template <std::floating_point T, size_t S>
static T floatingPointFromChars(const std::array<std::byte, S>& bytes, size_t offset, size_t size, std::chars_format fmt = std::chars_format::general)
{
    T retVal = T();
    std::from_chars_result result = std::from_chars(
        reinterpret_cast<const char*>(&bytes[offset]),
        reinterpret_cast<const char*>(&bytes[offset + size - 1]),
        retVal,
        fmt
    );
    return T;
}

namespace dted {
UserHeaderLabel::UserHeaderLabel(const UserHeaderLabelBlob& uhlBlob)
{
    //// recognitionSentinel
    //stringFromChars(uhlBlob.recognitionSentinel, recognitionSentinel);
    //
    //// fixedbyStandard
    //fixedByStandard = static_cast<uint8_t>(uhlBlob.fixedStandard[0]);

    //// longitudeOfOrigin
    //auto result = std::from_chars((const char*)(&uhlBlob.longitudeOfOrigin[0]), (const char*)(&uhlBlob.longitudeOfOrigin[uhlBlob.longitudeOfOrigin.size() - 1]), longitudeOfOrigin);
    //std::cout << longitudeOfOrigin << std::endl;
    //
    //longitudeOfOrigin = floatingPointFromChars<double>(uhlBlob.longitudeOfOrigin, 0, 7);
    //if (uhlBlob.longitudeOfOrigin[7] == (std::byte)'W') {
    //    longitudeOfOrigin *= -1;
    //}
    //if (longitudeOfOrigin >= 180.0 || longitudeOfOrigin <= -180.0) {
    //    std::cerr << "WARNING UserHeaderLabel() Longitude of origin has invalid range. [-180.0, 180.0]: " << longitudeOfOrigin << std::endl;
    //}
    //// latitudeOfOrigin
    //latitudeOfOrigin = floatingPointFromChars<double>(uhlBlob.latitudeOfOrigin, 0, 7);
    //if (uhlBlob.latitudeOfOrigin[7] == (std::byte)'S') {
    //    latitudeOfOrigin *= -1;
    //}
    //if (latitudeOfOrigin >= 90.0 || latitudeOfOrigin <= -90.0) {
    //    std::cerr << "WARNING UserHeaderLabel() Latitude of origin has invalid range. [-90.0, 90.0]: " << latitudeOfOrigin << std::endl;
    //}

    //// longitudeIntervalArcSeconds
    //longitudeIntervalArcSeconds = integralFromChars<uint16_t>(uhlBlob.longitudeIntervalArcSeconds, 0, 4);

    //// latitudeIntervalArcSeconds
    //latitudeIntervalArcSeconds = integralFromChars<uint16_t>(uhlBlob.latitudeIntervalArcSeconds, 0, 4);

    //// absoluteVerticalAccuracy
    //stringFromChars(uhlBlob.absoluteVerticalAccuracy, absoluteVerticalAccuracy);
    //std::string absoluteVerticalAccuracyString = toString(uhlBlob.absoluteVerticalAccuracy, 0, 4);
    //if (absoluteVerticalAccuracyString != "NA$$") {
    //    absoluteVerticalAccuracy = std::stoi(absoluteVerticalAccuracyString);
    //}

    //// unclassifiedSecurityCode
    //unclassifiedSecurtiyCode = static_cast<char>(uhlBlob.securityCode[0]);

    //// uniqueReferenceNumber
    //stringFromChars(uhlBlob.uniqueReference, uniqueReferenceNumber);
    //
    //// numberOfLongitudeLines
    //numberOfLongitudeLines = toIntegral<uint16_t>(uhlBlob.numberOfLongitudeLines, 0, 4);

    //// numberOfLatitudePoints
    //numberOfLatitudePoints = toIntegral<uint16_t>(uhlBlob.numberOfLatitudePoints, 0, 4);

    //// multipleAccuracy
    //multipleAccuracy = static_cast<int>(static_cast<char>(uhlBlob.multipleAccuracy[0]) - '0') > 0;
}

DataSetIdentification::DataSetIdentification(const DataSetIdentificationBlob& dsiBlob)
{
    ////recognitionSentinel
    //recognitionSentinel = toString(dsiBlob.recognitionSentinel, 0, 3);

    ////securityCode
    //securityCode = static_cast<char>(dsiBlob.securityCode[0]);

    ////securityControlMarkings
    //securityControlMarkings = toString(dsiBlob.securityControlMarkings, 0, 2);

    ////securityHandlingDescription
    //securityHandlingDescription = toString(dsiBlob.securityHandlingDescription, 0, 27);

    ////dtedLevel
    //dtedLevel = toIntegral<uint8_t>(dsiBlob.dtedLevel, 4, 1);

    ////uniqueReference
    //uniqueReference = toString(dsiBlob.uniqueReference, 0, 15);

    ////dataEditionNumber
    //dataEditionNumber = std::stoi(toString(dsiBlob.dataEditionNumber, 0, 2));

    ////matchMergeVersion
    //matchMergeVersion = static_cast<char>(dsiBlob.matchMergeVersion[0]);

    ////maintenanceDate
    //std::chrono::year maintenanceDateYear(std::stoi(toString(dsiBlob.maintenanceDate, 0, 2)));
    //std::chrono::month maintenanceDateMonth(std::stoi(toString(dsiBlob.maintenanceDate, 2, 2)));
    //maintenanceDate = std::chrono::year_month(maintenanceDateYear, maintenanceDateMonth);

    ////matchMergeDate
    //std::chrono::year matchMergeDateYear(std::stoi(toString(dsiBlob.matchMergeDate, 0, 2)));
    //std::chrono::month matchMergeDateMonth(std::stoi(toString(dsiBlob.matchMergeDate, 2, 2)));
    //maintenanceDate = std::chrono::year_month(matchMergeDateYear, matchMergeDateMonth);

    ////maintenanceDescCode
    //maintenanceDescCode = toIntegral<uint8_t>(dsiBlob.maintenanceDescCode, 0, 4);

    ////producerCode
    //producerCode = toString(dsiBlob.producerCode, 0, 8);

    ////productSpec
    //productSpec = toString(dsiBlob.productSpec, 0, 9);

    ////productSpecAmendmentNumber
    //productSpecAmendmentNumber = std::stoi(toString(dsiBlob.productSpecAmend, 0, 2));

    ////productSpecDate
    //std::chrono::year productSpecDateYear(toIntegral<uint8_t>(dsiBlob.productSpecDate, 0, 2));
    //std::chrono::month productSpecDateMonth(toIntegral<uint8_t>(dsiBlob.productSpecDate, 2, 2));
    //productSpecDate = std::chrono::year_month(productSpecDateYear, productSpecDateMonth);

    ////verticalDatum
    //verticalDatum = toString(dsiBlob.verticalDatum, 0, 3);

    ////horizontalDatum
    //horizontalDatum = toString(dsiBlob.horizontalDatum, 0, 5);

    ////digitizingSystem
    //digitizingSystem = toString(dsiBlob.digitizingSystem, 0, 10);

    ////compilationDate
    //std::chrono::year compilationDateYear(toIntegral<uint8_t>(dsiBlob.compilationDate, 0, 2));
    //std::chrono::month compilationDateMonth(toIntegral<uint8_t>(dsiBlob.compilationDate, 2, 2));
    //compilationDate = std::chrono::year_month(compilationDateYear, compilationDateMonth);

    ////latitudeOfOriginDeg
    //latitudeOfOriginDeg = toFloating<double>(dsiBlob.latOrigin, 0, 8, 2);
    //if (dsiBlob.latOrigin[8] == static_cast<std::byte>('S')) {
    //    latitudeOfOriginDeg *= -1.0;
    //}

    ////longitudeOfOriginDeg
    //longitudeOfOriginDeg = toFloatingDecimalPresent<double>(dsiBlob.lonOrigin, 0, 9);
    //if (dsiBlob.lonOrigin[9] == static_cast<std::byte>('W')) {
    //    longitudeOfOriginDeg *= -1.0;
    //}

    ////latSWCornerDeg
    //latSWCornerDeg = toFloating<double>(dsiBlob.latSWCorner, 0, 3, 3);
    //if (dsiBlob.latSWCorner[7] == static_cast<std::byte>('S')) {
    //    latSWCornerDeg *= -1.0;
    //}

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