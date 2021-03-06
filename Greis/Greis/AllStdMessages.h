#pragma once

#include "Greis/StdMessage/AngularVelocityStdMessage.h"
#include "Greis/StdMessage/AntNameStdMessage.h"
#include "Greis/StdMessage/BaseInfoStdMessage.h"
#include "Greis/StdMessage/BaselineStdMessage.h"
#include "Greis/StdMessage/BaselinesStdMessage.h"
#include "Greis/StdMessage/BeiDouAlmStdMessage.h"
#include "Greis/StdMessage/BeiDouEphemerisStdMessage.h"
#include "Greis/StdMessage/BeiDouIonoParamsStdMessage.h"
#include "Greis/StdMessage/BeiDouUtcParamStdMessage.h"
#include "Greis/StdMessage/CalBandsDelayStdMessage.h"
#include "Greis/StdMessage/ClockOffsetsStdMessage.h"
#include "Greis/StdMessage/CNRStdMessage.h"
#include "Greis/StdMessage/CNR4StdMessage.h"
#include "Greis/StdMessage/CompRawNavDataStdMessage.h"
#include "Greis/StdMessage/CPStdMessage.h"
#include "Greis/StdMessage/DopsStdMessage.h"
#include "Greis/StdMessage/DPStdMessage.h"
#include "Greis/StdMessage/EpochEndStdMessage.h"
#include "Greis/StdMessage/EpochTimeStdMessage.h"
#include "Greis/StdMessage/ERStdMessage.h"
#include "Greis/StdMessage/EventStdMessage.h"
#include "Greis/StdMessage/ExtEventStdMessage.h"
#include "Greis/StdMessage/FileIdStdMessage.h"
#include "Greis/StdMessage/FlagsStdMessage.h"
#include "Greis/StdMessage/FullRotationMatrixStdMessage.h"
#include "Greis/StdMessage/GALAlmStdMessage.h"
#include "Greis/StdMessage/GALEphemerisStdMessage.h"
#include "Greis/StdMessage/GalRawNavDataStdMessage.h"
#include "Greis/StdMessage/GalUtcGpsParamStdMessage.h"
#include "Greis/StdMessage/GeoPosStdMessage.h"
#include "Greis/StdMessage/GeoVelStdMessage.h"
#include "Greis/StdMessage/GLOAlmanacStdMessage.h"
#include "Greis/StdMessage/GloDelaysStdMessage.h"
#include "Greis/StdMessage/GLOEphemerisStdMessage.h"
#include "Greis/StdMessage/GloNavDataStdMessage.h"
#include "Greis/StdMessage/GloRawNavDataStdMessage.h"
#include "Greis/StdMessage/GLOTimeStdMessage.h"
#include "Greis/StdMessage/GloUtcGpsParamStdMessage.h"
#include "Greis/StdMessage/GPSAlm0StdMessage.h"
#include "Greis/StdMessage/GPSEphemeris0StdMessage.h"
#include "Greis/StdMessage/GpsNavData0StdMessage.h"
#include "Greis/StdMessage/GpsRawNavData0StdMessage.h"
#include "Greis/StdMessage/GPSTimeStdMessage.h"
#include "Greis/StdMessage/GpsUtcParamStdMessage.h"
#include "Greis/StdMessage/HeadAndPitchStdMessage.h"
#include "Greis/StdMessage/IAmpStdMessage.h"
#include "Greis/StdMessage/InertialMeasurementsStdMessage.h"
#include "Greis/StdMessage/IonoDelayStdMessage.h"
#include "Greis/StdMessage/IonoParams0StdMessage.h"
#include "Greis/StdMessage/LatencyStdMessage.h"
#include "Greis/StdMessage/LoggingHistoryStdMessage.h"
#include "Greis/StdMessage/MsgFmtStdMessage.h"
#include "Greis/StdMessage/NavStatusStdMessage.h"
#include "Greis/StdMessage/ParamsStdMessage.h"
#include "Greis/StdMessage/PosStdMessage.h"
#include "Greis/StdMessage/PosCompTimeStdMessage.h"
#include "Greis/StdMessage/PosCovStdMessage.h"
#include "Greis/StdMessage/PosStatStdMessage.h"
#include "Greis/StdMessage/PosVelStdMessage.h"
#include "Greis/StdMessage/PosVelVectorStdMessage.h"
#include "Greis/StdMessage/PPSOffsetStdMessage.h"
#include "Greis/StdMessage/PRStdMessage.h"
#include "Greis/StdMessage/QAmpStdMessage.h"
#include "Greis/StdMessage/QZSSAlmStdMessage.h"
#include "Greis/StdMessage/QZSSEphemerisStdMessage.h"
#include "Greis/StdMessage/QzssIonoParamsStdMessage.h"
#include "Greis/StdMessage/QzssNavDataStdMessage.h"
#include "Greis/StdMessage/QzssRawNavDataStdMessage.h"
#include "Greis/StdMessage/QzssUtcParamStdMessage.h"
#include "Greis/StdMessage/RangeResidualStdMessage.h"
#include "Greis/StdMessage/RawMeasStdMessage.h"
#include "Greis/StdMessage/RCPRc1StdMessage.h"
#include "Greis/StdMessage/RCPRC0StdMessage.h"
#include "Greis/StdMessage/RcvBeiDouTimeOffsetStdMessage.h"
#include "Greis/StdMessage/RcvDateStdMessage.h"
#include "Greis/StdMessage/RcvGALTimeOffsetStdMessage.h"
#include "Greis/StdMessage/RcvGLOTimeOffsetStdMessage.h"
#include "Greis/StdMessage/RcvGPSTimeOffsetStdMessage.h"
#include "Greis/StdMessage/RcvOscOffsStdMessage.h"
#include "Greis/StdMessage/RcvQZSSTimeOffsetStdMessage.h"
#include "Greis/StdMessage/RcvSBASTimeOffsetStdMessage.h"
#include "Greis/StdMessage/RcvTimeStdMessage.h"
#include "Greis/StdMessage/RcvTimeAccuracyStdMessage.h"
#include "Greis/StdMessage/RcvTimeOffsAtPPSStdMessage.h"
#include "Greis/StdMessage/RcvTimeOffsetStdMessage.h"
#include "Greis/StdMessage/RcvTimeOffsetDotStdMessage.h"
#include "Greis/StdMessage/REStdMessage.h"
#include "Greis/StdMessage/RefEpochStdMessage.h"
#include "Greis/StdMessage/RmsStdMessage.h"
#include "Greis/StdMessage/RotationAnglesStdMessage.h"
#include "Greis/StdMessage/RotationMatrixStdMessage.h"
#include "Greis/StdMessage/RotationMatrixAndVectorsStdMessage.h"
#include "Greis/StdMessage/RPRStdMessage.h"
#include "Greis/StdMessage/SatAzimuthStdMessage.h"
#include "Greis/StdMessage/SatElevationStdMessage.h"
#include "Greis/StdMessage/SatIndexStdMessage.h"
#include "Greis/StdMessage/SatNumbersStdMessage.h"
#include "Greis/StdMessage/SBASAlmanacStdMessage.h"
#include "Greis/StdMessage/SBASEhemerisStdMessage.h"
#include "Greis/StdMessage/SbasRawNavDataStdMessage.h"
#include "Greis/StdMessage/SbasUtcParamStdMessage.h"
#include "Greis/StdMessage/SCStdMessage.h"
#include "Greis/StdMessage/SCPStdMessage.h"
#include "Greis/StdMessage/Security0StdMessage.h"
#include "Greis/StdMessage/Security1StdMessage.h"
#include "Greis/StdMessage/SolutionTimeStdMessage.h"
#include "Greis/StdMessage/Spectrum0StdMessage.h"
#include "Greis/StdMessage/Spectrum1StdMessage.h"
#include "Greis/StdMessage/SPRStdMessage.h"
#include "Greis/StdMessage/SRDPStdMessage.h"
#include "Greis/StdMessage/SRPRStdMessage.h"
#include "Greis/StdMessage/SSStdMessage.h"
#include "Greis/StdMessage/TrackingTimeStdMessage.h"
#include "Greis/StdMessage/TrackingTimeCAStdMessage.h"
#include "Greis/StdMessage/VelStdMessage.h"
#include "Greis/StdMessage/VelCovStdMessage.h"
#include "Greis/StdMessage/VelocityResidualStdMessage.h"
#include "Greis/StdMessage/WrapperStdMessage.h"

