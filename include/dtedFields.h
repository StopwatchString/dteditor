#ifndef DTED_FIELDS_H
#define DTED_FIELDS_H
#pragma pack(push, 1)

#include <array>
#include <cstddef>
#include <ios>

// User Header Label
struct UserHeaderLabel {
    std::array<std::byte, 3>  recognitionSentinel;      // 1-3   | Recognition sentinel
    std::array<std::byte, 1>  fixedStandard;            // 4     | Fixed by standard
    std::array<std::byte, 8>  longitudeOfOrigin;        // 5-12  | Longitude of origin
    std::array<std::byte, 8>  latitudeOfOrigin;         // 13-20 | Latitude of origin
    std::array<std::byte, 4>  longitudeInterval;        // 21-24 | Longitude data interval in seconds
    std::array<std::byte, 4>  latitudeInterval;         // 25-28 | Latitude data interval in seconds
    std::array<std::byte, 4>  absoluteVerticalAccuracy; // 29-32 | Absolute vertical accuracy
    std::array<std::byte, 3>  securityCode;             // 33-35 | Unclassified security code
    std::array<std::byte, 12> uniqueReference;          // 36-47 | Unique reference number
    std::array<std::byte, 4>  numberOfLongitudeLines;   // 48-51 | Number of longitude lines
    std::array<std::byte, 4>  numberOfLatitudePoints;   // 52-55 | Number of latitude points
    std::array<std::byte, 1>  multipleAccuracy;         // 56    | Multiple accuracy indicator
    std::array<std::byte, 24> reserved;                 // 57-80 | Reserved for future use
};
const std::streampos USER_HEADER_LABEL_OFFSET = 0;
const std::streamsize USER_HEADER_LABEL_SIZE = sizeof(UserHeaderLabel);

// Data Set Identification
struct DataSetIdentification {
    std::array<std::byte, 3>   recognitionSentinel;         // 1-3     | Recognition sentinel
    std::array<std::byte, 1>   securityCode;                // 4       | Unclassified security code
    std::array<std::byte, 2>   securityControlMarkings;     // 5-6     | Security control markings
    std::array<std::byte, 27>  securityHandlingDescription; // 7-33    | Security handling description
    std::array<std::byte, 26>  reserved1;                   // 34-59   | Reserved for future use
    std::array<std::byte, 5>   dtedLevel;                   // 60-64   | DTED level designator
    std::array<std::byte, 15>  uniqueReference;             // 65-79   | Unique reference number
    std::array<std::byte, 8>   reserved2;                   // 80-87   | Reserved for future use
    std::array<std::byte, 2>   dataEditionNumber;           // 88-89   | Data edition number
    std::array<std::byte, 1>   matchMergeVersion;           // 90      | Match/merge version
    std::array<std::byte, 4>   maintenanceDate;             // 91-94   | Maintenance date
    std::array<std::byte, 4>   matchMergeDate;              // 95-98   | Match/merge date
    std::array<std::byte, 4>   maintenanceDescCode;         // 99-102  | Maintenance description code
    std::array<std::byte, 8>   producerCode;                // 103-110 | Producer code
    std::array<std::byte, 16>  reserved3;                   // 111-126 | Reserved for future use
    std::array<std::byte, 9>   productSpec;                 // 127-135 | Product specification
    std::array<std::byte, 2>   productSpecAmend;            // 136-137 | Product spec amendment number
    std::array<std::byte, 4>   productSpecDate;             // 138-141 | Date of product specification
    std::array<std::byte, 3>   verticalDatum;               // 142-144 | Vertical datum
    std::array<std::byte, 5>   horizontalDatum;             // 145-149 | Horizontal datum
    std::array<std::byte, 10>  digitizingSystem;            // 150-159 | Digitizing/collection system
    std::array<std::byte, 4>   compilationDate;             // 160-163 | Compilation date
    std::array<std::byte, 22>  reserved4;                   // 164-185 | Reserved for future use
    std::array<std::byte, 9>   latOrigin;                   // 186-194 | Latitude of origin
    std::array<std::byte, 10>  lonOrigin;                   // 195-204 | Longitude of origin
    std::array<std::byte, 7>   latSWCorner;                 // 205-211 | Latitude of SW corner
    std::array<std::byte, 8>   lonSWCorner;                 // 212-219 | Longitude of SW corner
    std::array<std::byte, 7>   latNWCorner;                 // 220-226 | Latitude of NW corner
    std::array<std::byte, 8>   lonNWCorner;                 // 227-234 | Longitude of NW corner
    std::array<std::byte, 7>   latNECorner;                 // 235-241 | Latitude of NE corner
    std::array<std::byte, 8>   lonNECorner;                 // 242-249 | Longitude of NE corner
    std::array<std::byte, 7>   latSECorner;                 // 250-256 | Latitude of SE corner
    std::array<std::byte, 8>   lonSECorner;                 // 257-264 | Longitude of SE corner
    std::array<std::byte, 9>   orientationAngle;            // 265-273 | Clockwise orientation angle
    std::array<std::byte, 4>   latitudeInterval;            // 274-277 | Latitude interval
    std::array<std::byte, 4>   longitudeInterval;           // 278-281 | Longitude interval
    std::array<std::byte, 4>   numberLatitudeLines;         // 282-285 | Number of latitude lines
    std::array<std::byte, 4>   numberLongitudeLines;        // 286-289 | Number of longitude lines
    std::array<std::byte, 2>   partialCellIndicator;        // 290-291 | Partial cell indicator
    std::array<std::byte, 101> coveragePercent;             // 292-392 | Coverage percentage
    std::array<std::byte, 100> geoidUndulation;             // 393-492 | Geoid undulation
    std::array<std::byte, 156> reserved5;                   // 493-648 | Reserved for future use
};
const std::streampos DATA_SET_IDENTIFICATION_OFFSET = USER_HEADER_LABEL_OFFSET + USER_HEADER_LABEL_SIZE;
const std::streamsize DATA_SET_IDENTIFICATION_SIZE = sizeof(DataSetIdentification);

// Accuracy Description Record
struct AccuracyDescriptionRecord {
    std::array<std::byte, 3>    recognitionSentinel;   // 1-3       | Recognition sentinel
    std::array<std::byte, 4>    absHorizontalAccuracy; // 4-7       | Absolute horizontal accuracy
    std::array<std::byte, 4>    absVerticalAccuracy;   // 8-11      | Absolute vertical accuracy
    std::array<std::byte, 4>    relHorizontalAccuracy; // 12-15     | Relative horizontal accuracy
    std::array<std::byte, 4>    relVerticalAccuracy;   // 16-19     | Relative vertical accuracy
    std::array<std::byte, 4>    reserved1;             // 20-23     | Reserved
    std::array<std::byte, 1>    reservedDMA;           // 24        | Reserved for DMA
    std::array<std::byte, 31>   reserved2;             // 25-55     | Reserved
    std::array<std::byte, 2>    accuracyOutlineFlag;   // 56-57     | Accuracy outline flag
    std::array<std::byte, 2556> accuracySubregions;    // 58-2613   | Accuracy of subregions
    std::array<std::byte, 18>   reservedDMA2;          // 2614-2631 | Reserved for DMA
    std::array<std::byte, 69>   reserved3;             // 2632-2700 | Reserved
};
const std::streampos ACCURACY_DESCRIPTION_RECORD_OFFSET = DATA_SET_IDENTIFICATION_OFFSET + DATA_SET_IDENTIFICATION_SIZE;
const std::streamsize ACCURACTY_DESCRIPTION_RECORD_SIZE = sizeof(UserHeaderLabel);

// DtedHeader
struct DtedHeader {
    UserHeaderLabel uhl{};
    DataSetIdentification dsi{};
    AccuracyDescriptionRecord acc{};
};
const std::streampos DTED_HEADER_OFFSET = 0;
const std::streamsize DTED_HEADER_SIZE = sizeof(DtedHeader);

#pragma pack(pop)
#endif