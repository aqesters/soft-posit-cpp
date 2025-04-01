#include "internals.h"
#include "platform.h"
#include <math.h>

posit16_t p16_tan_pi(posit16_t pA) {
    // TODO: Implement proper tan(pi*x) function for posit16_t
    // Currently using double conversion as placeholder
    union ui16_p16 uA;
    double doubleA;
    
    uA.p = pA;
    
    // Check for special cases
    if (uA.ui == 0x8000) {
        // If NaR, return NaR
        return pA;
    }
    
    // Convert posit to double
    doubleA = convertP16ToDouble(pA);
    
    // Handle special cases for tan
    double modValue = fmod(fabs(doubleA), 1.0);
    if (modValue == 0.5) {
        // tan(pi/2 + n*pi) is undefined, return NaR
        uA.ui = 0x8000;
        return uA.p;
    }
    
    // Calculate tan(pi*x) using standard math library
    double result = tan(M_PI * doubleA);
    
    // Convert back to posit
    return convertDoubleToP16(result);
} 