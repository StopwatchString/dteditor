#ifndef DTED_PARSED_STRUCTS_H
#define DTED_PARSED_STRUCTS_H

#include "dted/dtedFileDefinitions.h"

#include <string>
#include <iostream>
#include <chrono>

namespace dted {

struct UserHeaderLabel
{
    std::string recognitionSentinel;            // 1-3   | Recognition sentinel
    uint8_t fixedByStandard{};                  // 4     | Fixed by standard
    double longitudeOfOriginTenthsArcSeconds{}; // 5-12  | Longitude of origin
    double latitudeOfOriginTenthsArcSeconds{};  // 13-20 | Latitude of origin
    uint16_t longitudeIntervalArcSeconds{};     // 21-24 | Longitude data interval in arc seconds       
    uint16_t latitudeIntervalArcSeconds{};      // 25-28 | Latitude data interval in arc seconds      
    uint16_t absoluteVerticalAccuracy{};        // 29-32 | Absolute vertical accuracy    
    char unclassifiedSecurtiyCode{};            // 33-35 | Unclassified security code
    std::string uniqueReferenceNumber;          // 36-47 | Unique reference number  
    uint16_t numberOfLongitudeLines{};          // 48-51 | Number of longitude lines  
    uint16_t numberOfLatitudePoints{};          // 52-55 | Number of latitude points  
    bool multipleAccuracy{};                    // 56    | Multiple accuracy indicator
    UserHeaderLabel() = delete;
    UserHeaderLabel(const UserHeaderLabelBlob& uhlBlob);
};

struct DataSetIdentification
{
    std::string recognitionSentinel;          // 1-3     | Recognition sentinel
    char securityCode;                        // 4       | Unclassified security code
    std::string securityControlMarkings;      // 5-6     | Security control markings
    std::string securityHandlingDescription;  // 7-33    | Security handling description
    uint8_t dtedLevel;                        // 60-64   | DTED level designator
    std::string uniqueReference;              // 65-79   | Unique reference number
    uint8_t dataEditionNumber;                // 88-89   | Data edition number
    char matchMergeVersion;                   // 90      | Match/merge version
    std::chrono::year_month maintenanceDate;  // 91-94   | Maintenance date
    std::chrono::year_month matchMergeDate;   // 95-98   | Match/merge date
    uint8_t maintenanceDescCode;              // 99-102  | Maintenance description code
    std::string producerCode;                 // 103-110 | Producer code
    std::string productSpec;                  // 127-135 | Product specification
    uint8_t productSpecAmendmentNumber;       // 136-137 | Product spec amendment number
    std::chrono::year_month productSpecDate;  // 138-141 | Date of product specification
    std::string verticalDatum;                // 142-144 | Vertical datum
    std::string horizontalDatum;              // 145-149 | Horizontal datum
    std::string digitizingSystem;             // 150-159 | Digitizing/collection system
    std::chrono::year_month compilationDate;  // 160-163 | Compilation date
    double latitudeOfOriginTenthsArcSeconds;  // 186-194 | Latitude of origin
    double longitudeOfOriginTenthsArcSeconds; // 195-204 | Longitude of origin
    double latSWCornerTenthsArcSeconds;       // 205-211 | Latitude of SW corner
    double lonSWCornerTenthsArcSeconds;       // 212-219 | Longitude of SW corner
    double latNWCornerTenthsArcSeconds;       // 220-226 | Latitude of NW corner
    double lonNWCornerTenthsArcSeconds;       // 227-234 | Longitude of NW corner
    double latNECornerTenthsArcSeconds;       // 235-241 | Latitude of NE corner
    double lonNECornerTenthsArcSeconds;       // 242-249 | Longitude of NE corner
    double latSECornerTenthsArcSeconds;       // 250-256 | Latitude of SE corner
    double lonSECornerTenthsArcSeconds;       // 257-264 | Longitude of SE corner
    double orientationAngle;                  // 265-273 | Clockwise orientation angle
    uint16_t latitudeIntervalArcSeconds;      // 274-277 | Latitude interval
    uint16_t longitudeIntervalArcSeconds;     // 278-281 | Longitude interval
    uint16_t numberLatitudeLines;             // 282-285 | Number of latitude lines
    uint16_t numberLongitudeLines;            // 286-289 | Number of longitude lines
    uint8_t partialCellIndicator;             // 290-291 | Partial cell indicator
    uint32_t coveragePercent;                 // 292-392 | Coverage percentage
    std::string geoidUndulation;              // 393-492 | Geoid undulation

    DataSetIdentification() = delete;
    DataSetIdentification(const DataSetIdentificationBlob& dsiBlob);
};

struct AccuracyDescriptionRecord
{
    std::string recognitionSentinel;                // 1-3       | Recognition sentinel
    uint16_t absHorizontalAccuracy;                 // 4-7       | Absolute horizontal accuracy
    uint16_t absVerticalAccuracy;                   // 8-11      | Absolute vertical accuracy
    uint16_t relHorizontalAccuracy;                 // 12-15     | Relative horizontal accuracy
    uint16_t relVerticalAccuracy;                   // 16-19     | Relative vertical accuracy
    uint8_t accuracyOutlineFlag;                    // 56-57     | Accuracy outline flag
    std::array<std::byte, 2556> accuracySubregions; // 58-2613   | Accuracy of subregions

    AccuracyDescriptionRecord() = delete;
    AccuracyDescriptionRecord(const AccuracyDescriptionRecordBlob& accBlob);
};

} // End dted namespace

#endif