#ifndef EMPTYVALUES_H
#define EMPTYVALUES_H

#define EMPTY_DOUBLE 2.2250738585072014E-308
#define EMPTY_INT16	((int16_t) 0x8000)
#define EMPTY_INT	((int32_t) 0x80000000)
#define EMPTY_INT64	((int64_t) 0x8000000000000000)
#define EMPTY_CDT_DATE	(-(int64_t) 2209161601)
#define EMPTY_CDT_TIME	(-(int64_t) 2209161601)
#define EMPTY_STRING ""

#define STR_EMPTY_INT16 "-32768"
#define STR_EMPTY_INT "-2147483648"
#define STR_EMPTY_INT64 "-9223372036854775808"
#define STR_EMPTY_DOUBLE "2.2250738585072014E-308"
#define STR_EMPTY_CDT_DATE "-2209161601"
#define STR_EMPTY_CDT_TIME "-2209161601"

//#define IS_EMPTY_DOUBLE(d) (std::abs(EMPTY_DOUBLE - (d))*100 < 0.00001)
#define IS_EMPTY_DOUBLE(d) ((double)d == EMPTY_DOUBLE)
#define IS_EMPTY_INT64(d) (d == EMPTY_INT64)

#endif
