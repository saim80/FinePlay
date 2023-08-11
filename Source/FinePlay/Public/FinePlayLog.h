// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FineBaseLoggerMacros.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFinePlay, Log, All);

#define FP_FATAL(FORMAT, ...) FINEBASE_FATAL(LogFinePlay, FORMAT, ##__VA_ARGS__)
#define FP_ERROR(FORMAT, ...) FINEBASE_ERROR(LogFinePlay, FORMAT, ##__VA_ARGS__)
#define FP_WARNING(FORMAT, ...) FINEBASE_WARNING(LogFinePlay, FORMAT, ##__VA_ARGS__)
#define FP_DISPLAY(FORMAT, ...) FINEBASE_DISPLAY(LogFinePlay, FORMAT, ##__VA_ARGS__)
#define FP_LOG(VERBOSITY, FORMAT, ...) FINEBASE_LOG(LogFinePlay, VERBOSITY, FORMAT, ##__VA_ARGS__)
#define FP_VERBOSE(FORMAT, ...) FINEBASE_VERBOSE(LogFinePlay, FORMAT, ##__VA_ARGS__)
#define FP_VERYVERBOSE(FORMAT, ...) FINEBASE_VERYVERBOSE(LogFinePlay, FORMAT, ##__VA_ARGS__)
