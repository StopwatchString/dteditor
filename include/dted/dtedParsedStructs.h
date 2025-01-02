#ifndef DTED_PARSED_STRUCTS_H
#define DTED_PARSED_STRUCTS_H

#include "dted/dtedFileDefinitions.h"

#include <string>
#include <iostream>

namespace dted {

struct UserHeaderLabel
{
    std::string recognitionSentinel;
    uint8_t fixedByStandard{};
    double longitudeOfOrigin{};
    double latitudeOfOrigin{};
    uint16_t longitudeIntervalArcSeconds{};
    uint16_t latitudeIntervalArcSeconds{};
    uint16_t absoluteVerticalAccuracy{};
    char unclassifiedSecurtiyCode{};
    std::string uniqueReferenceNumber;
    uint16_t numberOfLongitudeLines{};
    uint16_t numberOfLatitudePoints{};
    bool multipleAccuracy{};

    UserHeaderLabel() = delete;
    UserHeaderLabel(const UserHeaderLabelBlob& uhlBlob);
};

struct DataSetIdentification
{
    std::string recognitionSentinel;         // 1-3     | Recognition sentinel
    char securityCode;                // 4       | Unclassified security code
    std::string securityControlMarkings;     // 5-6     | Security control markings
    std::string securityHandlingDescription; // 7-33    | Security handling description
    uint8_t dtedLevel;                   // 60-64   | DTED level designator
    std::string uniqueReference;             // 65-79   | Unique reference number
    uint8_t dataEditionNumber;           // 88-89   | Data edition number
    char matchMergeVersion;           // 90      | Match/merge version
    uint8_t maintenanceDate;             // 91-94   | Maintenance date
    uint8_t matchMergeDate;              // 95-98   | Match/merge date
    uint8_t maintenanceDescCode;         // 99-102  | Maintenance description code
    std::string producerCode;                // 103-110 | Producer code
    std::string productSpec;                 // 127-135 | Product specification
    uint8_t productSpecAmendmentNumber;            // 136-137 | Product spec amendment number
    std::pair<uint8_t, uint8_t> productSpecDateYYMM;             // 138-141 | Date of product specification
    //verticalDatum;               // 142-144 | Vertical datum
    //horizontalDatum;             // 145-149 | Horizontal datum
    //digitizingSystem;            // 150-159 | Digitizing/collection system
    //compilationDate;             // 160-163 | Compilation date
    //reserved4;                   // 164-185 | Reserved for future use
    //latOrigin;                   // 186-194 | Latitude of origin
    //lonOrigin;                   // 195-204 | Longitude of origin
    //latSWCorner;                 // 205-211 | Latitude of SW corner
    //lonSWCorner;                 // 212-219 | Longitude of SW corner
    //latNWCorner;                 // 220-226 | Latitude of NW corner
    //lonNWCorner;                 // 227-234 | Longitude of NW corner
    //latNECorner;                 // 235-241 | Latitude of NE corner
    //lonNECorner;                 // 242-249 | Longitude of NE corner
    //latSECorner;                 // 250-256 | Latitude of SE corner
    //lonSECorner;                 // 257-264 | Longitude of SE corner
    //orientationAngle;            // 265-273 | Clockwise orientation angle
    //latitudeIntervalArcSeconds;  // 274-277 | Latitude interval
    //longitudeIntervalArcSeconds; // 278-281 | Longitude interval
    //numberLatitudeLines;         // 282-285 | Number of latitude lines
    //numberLongitudeLines;        // 286-289 | Number of longitude lines
    //partialCellIndicator;        // 290-291 | Partial cell indicator
    //coveragePercent;             // 292-392 | Coverage percentage
    //geoidUndulation;             // 393-492 | Geoid undulation
    //reserved5;                   // 493-648 | Reserved for future use

    DataSetIdentification() = delete;
    DataSetIdentification(const DataSetIdentificationBlob& dsiBlob);
};

struct AccuracyDescriptionRecord
{
    AccuracyDescriptionRecord() = delete;
    AccuracyDescriptionRecord(const DataSetIdentificationBlob& accBlob);
};

} // End dted namespace

#endif