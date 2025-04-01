#include "internals.h"
#include "platform.h"
#include <math.h>

posit16_t p16_log2(posit16_t pA) {
    // TODO: Implement proper base-2 logarithm function for posit16_t
    // Currently using double conversion as placeholder
    union ui16_p16 uA;
    double doubleA;
    
    uA.p = pA;
    
    // Check for special cases
    if (uA.ui == 0x8000) {
        // If NaR, return NaR
        return pA;
    }
    
    // Handle non-positive values
    if (uA.ui <= 0x4000) {
        if (uA.ui == 0x4000) {
            // log2(1) = 0
            uA.ui = 0;
            return uA.p;
        } else {
            // Negative or zero input returns NaR for logarithm
            uA.ui = 0x8000;
            return uA.p;
        }
    }
    
    // Convert posit to double
    doubleA = convertP16ToDouble(pA);
    
    // Calculate base-2 logarithm using standard math library
    double result = log2(doubleA);
    
    // Convert back to posit
    return convertDoubleToP16(result);
} 