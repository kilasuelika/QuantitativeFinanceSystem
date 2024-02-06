#pragma once

#include <string>
#include <array>
#include <vector>

#define TIME_COL "time"
#define CODE_COL "code"
#define OPEN_COL "open"
#define HIGH_COL "high"
#define LOW_COL "low"
#define CLOSE_COL "close"
#define VOLUME_COL "volume"
#define AMOUNT_COL "amount"
#define TURNOVER_COL "turnover"

static constexpr std::array<std::string,7> PRICE_FIRLDS{ OPEN_COL, CLOSE_COL, HIGH_COL, LOW_COL, VOLUME_COL, AMOUNT_COL, TURNOVER_COL };