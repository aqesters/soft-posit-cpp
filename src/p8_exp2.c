#include "internals.h"
#include "platform.h"
#include <math.h>

posit8_t p8_exp2(posit8_t pA) {
    // TODO: Implement proper exp2 function for posit8_t
    // Currently using double conversion as placeholder
    union ui8_p8 uA;
    double doubleA;
    
    uA.p = pA;
    
    // Check for special cases
    if (uA.ui == 0x80) {
        // If NaR, return NaR
        return pA;
    }
    
    // Convert posit to double
    doubleA = convertP8ToDouble(pA);
    
    // Calculate 2^x using standard math library
    double result = pow(2.0, doubleA);
    
    // Convert back to posit
    return convertDoubleToP8(result);
} 