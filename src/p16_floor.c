#include "internals.h"
#include "platform.h"
#include <math.h>

posit16_t p16_floor(posit16_t pA) {
    // TODO: Implement proper floor function for posit16_t
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
    
    // Calculate floor using standard math library
    double result = floor(doubleA);
    
    // Convert back to posit
    return convertDoubleToP16(result);
} 