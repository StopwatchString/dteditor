#include "dted/dtedParsedStructs.h"

#include <string>
#include <iostream>
#include <concepts>
#include <charconv>

//-----------------------------------------------
// stringFromChars()
//-----------------------------------------------
template <size_t S>
static void stringFromChars(std::string& target, const std::array<std::byte, S>& bytes)
{
    target.resize(bytes.size());
    memcpy(target.data(), bytes.data(), bytes.size());
}

//-----------------------------------------------
// integralFromChars()
//-----------------------------------------------
template <std::integral T, size_t S>
static std::from_chars_result integralFromChars(T& target, const std::array<std::byte, S>& bytes, size_t offset, size_t size, int base = 10)
{
    return std::from_chars(
        reinterpret_cast<const char*>(&bytes[offset]),
        reinterpret_cast<const char*>(&bytes[offset + size - 1]),
        target,
        base
    );
}

//-----------------------------------------------
// floatingPointFromChars()
//-----------------------------------------------
template <std::floating_point T, size_t S>
static std::from_chars_result floatingPointFromChars(T& target, const std::array<std::byte, S>& bytes, size_t offset, size_t size, std::chars_format fmt = std::chars_format::general)
{
    return std::from_chars(
        reinterpret_cast<const char*>(&bytes[offset]),
        reinterpret_cast<const char*>(&bytes[offset + size - 1]),
        target,
        fmt
    );
}

namespace dted {
//-----------------------------------------------
// UserHeaderLabel Constructor
//-----------------------------------------------
UserHeaderLabel::UserHeaderLabel(const UserHeaderLabelBlob& uhlBlob)
{
    // recognitionSentinel
    stringFromChars(recognitionSentinel, uhlBlob.recognitionSentinel);
    
    // fixedbyStandard
    fixedByStandard = static_cast<uint8_t>(uhlBlob.fixedStandard[0]);

    // longitudeOfOrigin
    floatingPointFromChars(longitudeOfOriginTenthsArcSeconds, uhlBlob.longitudeOfOriginTenthsArcSeconds, 0, uhlBlob.longitudeOfOriginTenthsArcSeconds.size() - 1);
    if (uhlBlob.longitudeOfOriginTenthsArcSeconds[uhlBlob.longitudeOfOriginTenthsArcSeconds.size() - 1] == (std::byte)'W') {
        longitudeOfOriginTenthsArcSeconds = -longitudeOfOriginTenthsArcSeconds;
    }

    // latitudeOfOrigin
    floatingPointFromChars(latitudeOfOriginTenthsArcSeconds, uhlBlob.latitudeOfOriginTenthsArcSeconds, 0, uhlBlob.latitudeOfOriginTenthsArcSeconds.size() - 1);
    if (uhlBlob.latitudeOfOriginTenthsArcSeconds[uhlBlob.latitudeOfOriginTenthsArcSeconds.size() - 1] == (std::byte)'S') {
        latitudeOfOriginTenthsArcSeconds = -latitudeOfOriginTenthsArcSeconds;
    }

    // longitudeIntervalArcSeconds
    integralFromChars(longitudeIntervalArcSeconds, uhlBlob.longitudeIntervalArcSeconds, 0, uhlBlob.longitudeIntervalArcSeconds.size());

    // latitudeIntervalArcSeconds
    integralFromChars(latitudeIntervalArcSeconds, uhlBlob.latitudeIntervalArcSeconds, 0, uhlBlob.latitudeIntervalArcSeconds.size());

    // absoluteVerticalAccuracy
    std::string absoluteVerticalAccuracyString;
    stringFromChars(absoluteVerticalAccuracyString, uhlBlob.absoluteVerticalAccuracy);
    if (absoluteVerticalAccuracyString != "NA$$") {
        absoluteVerticalAccuracy = std::stoi(absoluteVerticalAccuracyString);
    }

    // unclassifiedSecurityCode
    unclassifiedSecurtiyCode = static_cast<char>(uhlBlob.securityCode[0]);

    // uniqueReferenceNumber
    stringFromChars(uniqueReferenceNumber, uhlBlob.uniqueReference);
    
    // numberOfLongitudeLines
    integralFromChars(numberOfLongitudeLines, uhlBlob.numberOfLongitudeLines, 0, 4);

    // numberOfLatitudePoints
    integralFromChars(numberOfLatitudePoints, uhlBlob.numberOfLatitudePoints, 0, 4);

    // multipleAccuracy
    multipleAccuracy = static_cast<int>(static_cast<char>(uhlBlob.multipleAccuracy[0]) - '0') > 0;
}

//-----------------------------------------------
// DataSetIdentification Constructor
//-----------------------------------------------
DataSetIdentification::DataSetIdentification(const DataSetIdentificationBlob& dsiBlob)
{
    //recognitionSentinel
    stringFromChars(recognitionSentinel, dsiBlob.recognitionSentinel);

    //securityCode
    securityCode = static_cast<char>(dsiBlob.securityCode[0]);

    //securityControlMarkings
    stringFromChars(securityControlMarkings, dsiBlob.securityControlMarkings);

    //securityHandlingDescription
    stringFromChars(securityHandlingDescription, dsiBlob.securityHandlingDescription);

    //dtedLevel
    integralFromChars(dtedLevel, dsiBlob.dtedLevel, 4, 1);

    //uniqueReference
    stringFromChars(uniqueReference, dsiBlob.uniqueReference);

    //dataEditionNumber
    integralFromChars(dataEditionNumber, dsiBlob.dataEditionNumber, 0, 2);

    //matchMergeVersion
    matchMergeVersion = static_cast<char>(dsiBlob.matchMergeVersion[0]);

    //maintenanceDate
    uint8_t maintenanceDateYear;
    integralFromChars(maintenanceDateYear, dsiBlob.maintenanceDate, 0, 2);
    uint8_t maintenanceDateMonth;
    integralFromChars(maintenanceDateMonth, dsiBlob.maintenanceDate, 2, 2);
    maintenanceDate = std::chrono::year_month(std::chrono::year(maintenanceDateYear), std::chrono::month(maintenanceDateMonth));

    //matchMergeDate
    uint8_t matchMergeDateYear;
    integralFromChars(matchMergeDateYear, dsiBlob.matchMergeDate, 0, 2);
    uint8_t matchMergeDateMonth;
    integralFromChars(matchMergeDateMonth, dsiBlob.matchMergeDate, 2, 2);
    matchMergeDate = std::chrono::year_month(std::chrono::year(matchMergeDateYear), std::chrono::month(matchMergeDateMonth));

    //maintenanceDescCode
    integralFromChars(maintenanceDescCode, dsiBlob.maintenanceDescCode, 0, 4);

    //producerCode
    stringFromChars(producerCode, dsiBlob.producerCode);

    //productSpec
    stringFromChars(productSpec, dsiBlob.productSpec);

    //productSpecAmendmentNumber
    integralFromChars(productSpecAmendmentNumber, dsiBlob.productSpecAmend, 0, 2);

    //productSpecDate
    uint8_t productSpecDateYear;
    integralFromChars(productSpecDateYear, dsiBlob.productSpecDate, 0, 2);
    uint8_t productSpecDateMonth;
    integralFromChars(productSpecDateMonth, dsiBlob.productSpecDate, 2, 2);
    productSpecDate = std::chrono::year_month(std::chrono::year(productSpecDateYear), std::chrono::month(productSpecDateMonth));

    //verticalDatum
    stringFromChars(verticalDatum, dsiBlob.verticalDatum);

    //horizontalDatum
    stringFromChars(horizontalDatum, dsiBlob.horizontalDatum);

    //digitizingSystem
    stringFromChars(digitizingSystem, dsiBlob.digitizingSystem);

    //compilationDate
    uint8_t compilationDateYear;
    integralFromChars(compilationDateYear, dsiBlob.compilationDate, 0, 2);
    uint8_t compilationDateMonth;
    integralFromChars(compilationDateMonth, dsiBlob.compilationDate, 2, 2);
    compilationDate = std::chrono::year_month(std::chrono::year(compilationDateYear), std::chrono::month(compilationDateMonth));

    //latitudeOfOriginTenthsArcSeconds
    floatingPointFromChars(latitudeOfOriginTenthsArcSeconds, dsiBlob.latitudeOfOriginTenthsArcSeconds, 0, dsiBlob.latitudeOfOriginTenthsArcSeconds.size() - 1);
    if (dsiBlob.latitudeOfOriginTenthsArcSeconds[dsiBlob.latitudeOfOriginTenthsArcSeconds.size() - 1] == static_cast<std::byte>('S')) {
        latitudeOfOriginTenthsArcSeconds = -latitudeOfOriginTenthsArcSeconds;
    }

    //longitudeOfOriginTenthsArcSeconds
    floatingPointFromChars(longitudeOfOriginTenthsArcSeconds, dsiBlob.longitudeOfOriginTenthsArcSeconds, 0, dsiBlob.longitudeOfOriginTenthsArcSeconds.size() - 1);
    if (dsiBlob.longitudeOfOriginTenthsArcSeconds[dsiBlob.longitudeOfOriginTenthsArcSeconds.size() - 1] == static_cast<std::byte>('W')) {
        longitudeOfOriginTenthsArcSeconds = -longitudeOfOriginTenthsArcSeconds;
    }

    //latSWCornerTenthsArcSeconds
    floatingPointFromChars(latSWCornerTenthsArcSeconds, dsiBlob.latSWCorner, 0, dsiBlob.latSWCorner.size() - 1);
    if (dsiBlob.latSWCorner[dsiBlob.latSWCorner.size() - 1] == static_cast<std::byte>('S')) {
        latSWCornerTenthsArcSeconds = -latSWCornerTenthsArcSeconds;
    }

    //lonSWCornerTenthsArcSeconds
    floatingPointFromChars(lonSWCornerTenthsArcSeconds, dsiBlob.lonSWCorner, 0, dsiBlob.lonSWCorner.size() - 1);
    if (dsiBlob.lonSWCorner[dsiBlob.lonSWCorner.size() - 1] == static_cast<std::byte>('W')) {
        lonSWCornerTenthsArcSeconds = -lonSWCornerTenthsArcSeconds;
    }

    //latNWCornerTenthsArcSeconds
    floatingPointFromChars(latNWCornerTenthsArcSeconds, dsiBlob.latNWCorner, 0, dsiBlob.latNWCorner.size() - 1);
    if (dsiBlob.latNWCorner[dsiBlob.latNWCorner.size() - 1] == static_cast<std::byte>('S')) {
        latNWCornerTenthsArcSeconds = -latNWCornerTenthsArcSeconds;
    }

    //lonNWCornerTenthsArcSeconds
    floatingPointFromChars(lonNWCornerTenthsArcSeconds, dsiBlob.lonNWCorner, 0, dsiBlob.lonNWCorner.size() - 1);
    if (dsiBlob.lonNWCorner[dsiBlob.lonNWCorner.size() - 1] == static_cast<std::byte>('W')) {
        lonNWCornerTenthsArcSeconds = -lonNWCornerTenthsArcSeconds;
    }

    //latNECornerTenthsArcSeconds
    floatingPointFromChars(latNECornerTenthsArcSeconds, dsiBlob.latNECorner, 0, dsiBlob.latNECorner.size() - 1);
    if (dsiBlob.latNECorner[dsiBlob.latNECorner.size() - 1] == static_cast<std::byte>('S')) {
        latNECornerTenthsArcSeconds = -latNECornerTenthsArcSeconds;
    }

    //lonNECornerTenthsArcSeconds
    floatingPointFromChars(lonNECornerTenthsArcSeconds, dsiBlob.lonNECorner, 0, dsiBlob.lonNECorner.size() - 1);
    if (dsiBlob.lonNECorner[dsiBlob.lonNECorner.size() - 1] == static_cast<std::byte>('W')) {
        lonNECornerTenthsArcSeconds = -lonNECornerTenthsArcSeconds;
    }

    //latSECornerTenthsArcSeconds
    floatingPointFromChars(latSECornerTenthsArcSeconds, dsiBlob.latSECorner, 0, dsiBlob.latSECorner.size() - 1);
    if (dsiBlob.latSECorner[dsiBlob.latSECorner.size() - 1] == static_cast<std::byte>('S')) {
        latSECornerTenthsArcSeconds = -latSECornerTenthsArcSeconds;
    }

    //lonSECornerTenthsArcSeconds
    floatingPointFromChars(lonSECornerTenthsArcSeconds, dsiBlob.lonSECorner, 0, dsiBlob.lonSECorner.size() - 1);
    if (dsiBlob.lonSECorner[dsiBlob.lonSECorner.size() - 1] == static_cast<std::byte>('W')) {
        lonSECornerTenthsArcSeconds = -lonSECornerTenthsArcSeconds;
    }

    //orientationAngle
    floatingPointFromChars(orientationAngle, dsiBlob.orientationAngle, 0, dsiBlob.orientationAngle.size());

    //latitudeIntervalArcSeconds
    integralFromChars(latitudeIntervalArcSeconds, dsiBlob.latitudeIntervalArcSeconds, 0, dsiBlob.latitudeIntervalArcSeconds.size());

    //longitudeIntervalArcSeconds
    integralFromChars(longitudeIntervalArcSeconds, dsiBlob.longitudeIntervalArcSeconds, 0, dsiBlob.longitudeIntervalArcSeconds.size());

    //numberLatitudeLines
    integralFromChars(numberLatitudeLines, dsiBlob.numberLatitudeLines, 0, dsiBlob.numberLatitudeLines.size());

    //numberLongitudeLines
    integralFromChars(numberLongitudeLines, dsiBlob.numberLongitudeLines, 0, dsiBlob.numberLongitudeLines.size());

    //partialCellIndicator
    integralFromChars(partialCellIndicator, dsiBlob.partialCellIndicator, 0, dsiBlob.partialCellIndicator.size());

    //coveragePercent
    integralFromChars(coveragePercent, dsiBlob.coveragePercent, 0, dsiBlob.coveragePercent.size());

    //geoidUndulation
    stringFromChars(geoidUndulation, dsiBlob.geoidUndulation);
}

//-----------------------------------------------
// AccuracyDescriptionRecord Constructor
//-----------------------------------------------
AccuracyDescriptionRecord::AccuracyDescriptionRecord(const AccuracyDescriptionRecordBlob& accBlob)
{

}

} // End dted namespace