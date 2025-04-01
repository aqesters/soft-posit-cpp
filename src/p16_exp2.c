#include "internals.h"
#include "platform.h"
#include <math.h>

posit16_t p16_exp2(posit16_t pA) {
    // TODO: Implement proper exp2 function for posit16_t
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
    
    // Calculate 2^x using standard math library
    double result = pow(2.0, doubleA);
    
    // Convert back to posit
    return convertDoubleToP16(result);
} 