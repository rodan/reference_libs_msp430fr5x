
#include <math.h>
#include <string.h>

#include "config.h"
#include "helper.h"

// |error| < 0.005
float _atan2f(const float y, const float x)
{

    float rv;
    float z = y / x;

    if (x == 0.0f) {
        if (y > 0.0f) {
            return PIBY2_FLOAT;
        }
        if (y == 0.0f) {
            return 0.0f;
        }
        return -PIBY2_FLOAT;
    }
    if (fabsf(z) < 1.0f) {
        rv = z / (1.0f + 0.28f * z * z);
        if (x < 0.0f) {
            if (y < 0.0f) {
                return rv - PI_FLOAT;
            }
            return rv + PI_FLOAT;
        }
    } else {
        rv = PIBY2_FLOAT - z / (z * z + 0.28f);
        if (y < 0.0f) {
            return rv - PI_FLOAT;
        }
    }
    return rv;
}

float _sin(const float x)
{
    unsigned char i;
    float denum = 1;
    float res = 0;
    float x_2 = x * x;
    float num = x;
    int s = 1;
    for (i = 0; i < PREC; i++) {
        res += s * (num / denum);
        denum = denum * (denum + 1) * (denum + 2);
        num = num * x_2;
        s = s * -1;
    }
    return res;
    /*
       float x_3 = x * x_2;
       float x_5 = x_3 * x_2;
       float x_7 = x_5 * x_2;
       float res = (x - x_3/6.0 + x_5/120.0 - x_7/5040.0 );
       return res;//+ x_9/362880.0); */
}

float _cos(const float x)
{
    unsigned char i;
    float denum = 2;
    float res = 1;
    float x_2 = x * x;
    float num = x_2;
    int s = -1;
    for (i = 0; i < PREC; i++) {
        res += s * (num / denum);
        denum = denum * (denum + 1) * (denum + 2);
        num = num * x_2;
        s = s * -1;
    }
    return res;
    /*

       float x_2 = x * x;
       float x_4 = x_2 * x_2;
       float x_6 = x_4 * x_2;
       float res = 1 - x_2 / 2.0 + x_4 / 24.0 - x_6 / 720.0 ;
       //float x_8 = x_6 * x_2;
       return res ;//+ x_8/40320.0;
     */
}

float _sqrt(const float number)
{
    unsigned char i = 0;
    float x0, sqx0, error;
    if (number < 1) {
        x0 = number * 2;
    } else {
        x0 = number / 2;
    }
    do {
        x0 = (x0 + (number / x0)) / 2;
        sqx0 = x0 * x0;
        error = (number - sqx0) / number;
        i++;
    } while (i < 20
             && ((error > 0 && error > 0.01) || (error < 0 && error < -0.01)));

    return x0;
}

float radians(const float x)
{
    return PI * x / 180.0f;
}

float sq(const float x)
{
    return x * x;
}

// ###############################################
// #
// #  string functions
// #

uint8_t str_to_uint16(char *str, uint16_t * out, const uint8_t seek,
                      const uint8_t len, const uint16_t min, const uint16_t max)
{
    uint16_t val = 0;
    uint32_t pow = 1;
    uint8_t i, c;

    for (i = len; i > seek; i--) {
        c = str[i-1] - 48;
        if (c < 10) {
            val += c * pow;
            pow *= 10;
        } else {
            if (val) {
                // if we already have a number and this char is unexpected
                break;
            }
        }
    }

    if ((val >= min) && (val <= max)) {
        *out = val;
    } else {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

uint8_t str_to_uint32(char *str, uint32_t * out, const uint8_t seek,
                      const uint8_t len, const uint32_t min, const uint32_t max)
{
    uint32_t val = 0, pow = 1;
    uint8_t i;

    // pow() is missing in msp gcc, so we improvise
    for (i = 0; i < len - 1; i++) {
        pow *= 10;
    }
    for (i = 0; i < len; i++) {
        if ((str[seek + i] > 47) && (str[seek + i] < 58)) {
            val += (str[seek + i] - 48) * pow;
        }
        pow /= 10;
    }
    if ((val >= min) && (val <= max)) {
        *out = val;
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

static uint16_t const bin_ascii[2] = { 0x30, 0x31 };

char *_utob(char *buf, const uint32_t val)
{
    char *p = &buf[33];
    uint32_t m = val;

    *p = '\0';

    if (val == 0) {
        p -= 1;
        memcpy(p, &bin_ascii[0], sizeof(uint8_t));
    }

    while (m > 0)
    {
        p -= 1;
        memcpy(p, &bin_ascii[m & 0x1], sizeof(uint8_t));
        m >>= 1;
    }

    return p;
}

static uint16_t const hex_ascii[16] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66 };

char *_utoh(char *buf, const uint32_t val)
{
    char *p = &buf[11];
    uint32_t m = val;

    *p = '\0';

    if (val == 0) {
        p -= 1;
        memcpy(p, &hex_ascii[0], sizeof(uint8_t));
    }

    while (m > 0)
    {
        p -= 1;
        memcpy(p, &hex_ascii[m & 0xf], sizeof(uint8_t));
        m >>= 4;
    }

    p -= 2;
    memcpy(p, "0x" , sizeof(uint16_t));

    return p;
}

#ifdef BIG_ITOA_TABLE
static uint16_t const dec_ascii[100] = {
    0x3030, 0x3130, 0x3230, 0x3330, 0x3430, 0x3530, 0x3630, 0x3730, 0x3830, 0x3930,
    0x3031, 0x3131, 0x3231, 0x3331, 0x3431, 0x3531, 0x3631, 0x3731, 0x3831, 0x3931,
    0x3032, 0x3132, 0x3232, 0x3332, 0x3432, 0x3532, 0x3632, 0x3732, 0x3832, 0x3932,
    0x3033, 0x3133, 0x3233, 0x3333, 0x3433, 0x3533, 0x3633, 0x3733, 0x3833, 0x3933,
    0x3034, 0x3134, 0x3234, 0x3334, 0x3434, 0x3534, 0x3634, 0x3734, 0x3834, 0x3934,
    0x3035, 0x3135, 0x3235, 0x3335, 0x3435, 0x3535, 0x3635, 0x3735, 0x3835, 0x3935,
    0x3036, 0x3136, 0x3236, 0x3336, 0x3436, 0x3536, 0x3636, 0x3736, 0x3836, 0x3936,
    0x3037, 0x3137, 0x3237, 0x3337, 0x3437, 0x3537, 0x3637, 0x3737, 0x3837, 0x3937,
    0x3038, 0x3138, 0x3238, 0x3338, 0x3438, 0x3538, 0x3638, 0x3738, 0x3838, 0x3938,
    0x3039, 0x3139, 0x3239, 0x3339, 0x3439, 0x3539, 0x3639, 0x3739, 0x3839, 0x3939, };

char *_uint32toa(char *buf, const uint32_t val)
{
    char *p = &buf[11];
    uint32_t m = val;

    *p = '\0';

    while(m >= 100)
    {
        uint32_t const old = m;

        p -= 2;
        m /= 100;
        memcpy(p, &dec_ascii[old - (m * 100)], sizeof(uint16_t));
    }

    p -= 2;
    memcpy(p, &dec_ascii[m], sizeof(uint16_t));

    return &p[m < 10];
}
#else
static uint16_t const dec_ascii[10] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39 };

char *_uint32toa(char *buf, const uint32_t val)
{
    char *p = &buf[11];
    uint32_t m = val;

    *p = '\0';

    while(m >= 10)
    {
        uint32_t const old = m;

        p -= 1;
        m /= 10;
        memcpy(p, &dec_ascii[old - (m * 10)], sizeof(uint8_t));
    }

    p -= 1;
    memcpy(p, &dec_ascii[m], sizeof(uint8_t));

    return p;
}
#endif

char *_utoa(char *buf, const uint32_t val)
{
    return _uint32toa(buf, val);
}

char *_itoa(char *buf, const int32_t val)
{
    char *p;
    if (val >= 0) {
        return _uint32toa(buf, val);
    } else {
        p = _uint32toa(buf, val * -1);
        *(p - 1) = '-';
        return p-1;
    }
}

