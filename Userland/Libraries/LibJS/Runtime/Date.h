/*
 * Copyright (c) 2020-2022, Linus Groh <linusg@serenityos.org>
 * Copyright (c) 2022-2023, Tim Flynn <trflynn89@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibCrypto/BigInt/SignedBigInteger.h>
#include <LibJS/Runtime/Object.h>

namespace JS {

class Date final : public Object {
    JS_OBJECT(Date, Object);
    JS_DECLARE_ALLOCATOR(Date);

public:
    static NonnullGCPtr<Date> create(Realm&, double date_value);

    virtual ~Date() override = default;

    double date_value() const { return m_date_value; }
    void set_date_value(double value) { m_date_value = value; }

    ErrorOr<String> iso_date_string() const;

private:
    Date(double date_value, Object& prototype);

    double m_date_value { 0 }; // [[DateValue]]
};

// 21.4.1.22 Time Zone Identifier Record, https://tc39.es/ecma262/#sec-time-zone-identifier-record
struct TimeZoneIdentifier {
    StringView identifier;         // [[Identifier]]
    StringView primary_identifier; // [[PrimaryIdentifier]]
};

// https://tc39.es/ecma262/#eqn-HoursPerDay
constexpr inline double hours_per_day = 24;
// https://tc39.es/ecma262/#eqn-MinutesPerHour
constexpr inline double minutes_per_hour = 60;
// https://tc39.es/ecma262/#eqn-SecondsPerMinute
constexpr inline double seconds_per_minute = 60;
// https://tc39.es/ecma262/#eqn-msPerSecond
constexpr inline double ms_per_second = 1'000;
// https://tc39.es/ecma262/#eqn-msPerMinute
constexpr inline double ms_per_minute = 60'000;
// https://tc39.es/ecma262/#eqn-msPerHour
constexpr inline double ms_per_hour = 3'600'000;
// https://tc39.es/ecma262/#eqn-msPerDay
constexpr inline double ms_per_day = 86'400'000;
// https://tc39.es/proposal-temporal/#eqn-nsPerDay
constexpr inline double ns_per_day = 86'400'000'000'000;
extern Crypto::SignedBigInteger const ns_per_day_bigint;

double day(double);
double time_within_day(double);
u16 days_in_year(i32);
double day_from_year(i32);
double time_from_year(i32);
i32 year_from_time(double);
u16 day_within_year(double);
bool in_leap_year(double);
u8 month_from_time(double);
u8 date_from_time(double);
u8 week_day(double);
u8 hour_from_time(double);
u8 min_from_time(double);
u8 sec_from_time(double);
u16 ms_from_time(double);
Crypto::SignedBigInteger get_utc_epoch_nanoseconds(i32 year, u8 month, u8 day, u8 hour, u8 minute, u8 second, u16 millisecond, u16 microsecond, u16 nanosecond);
Vector<Crypto::SignedBigInteger> get_named_time_zone_epoch_nanoseconds(StringView time_zone_identifier, i32 year, u8 month, u8 day, u8 hour, u8 minute, u8 second, u16 millisecond, u16 microsecond, u16 nanosecond);
i64 get_named_time_zone_offset_nanoseconds(StringView time_zone_identifier, Crypto::SignedBigInteger const& epoch_nanoseconds);
Vector<TimeZoneIdentifier> available_named_time_zone_identifiers();
StringView system_time_zone_identifier();
double local_time(double time);
double utc_time(double time);
double make_time(double hour, double min, double sec, double ms);
double make_day(double year, double month, double date);
double make_date(double day, double time);
double time_clip(double time);
bool is_time_zone_offset_string(StringView offset_string);
double parse_time_zone_offset_string(StringView offset_string);

}
