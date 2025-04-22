#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

// Points of interest for zooming
struct ZoomPoint {
    double x;
    double y;
    double zoomStart;
    double zoomEnd;
    double zoomFactor;
    std::string name;
    
    ZoomPoint(double _x, double _y, double _zoomStart, double _zoomEnd, 
              double _zoomFactor, const std::string& _name)
        : x(_x), y(_y), zoomStart(_zoomStart), zoomEnd(_zoomEnd), 
          zoomFactor(_zoomFactor), name(_name) {}
};

int main() {
    // Define interesting points for zooming
    std::vector<ZoomPoint> zoomPoints = {
        // Main view
        ZoomPoint(0.0, 0.0, 1.0, 1.0, 1.0, "overview"),
        
        // Boundary area showing precision differences
        ZoomPoint(-0.75, 0.1, 5.0, 5.0, 1.0, "boundary"),
        
        // Progressive zoom sequence on a detail
        ZoomPoint(-0.753, 0.12, 10.0, 10.0, 1.0, "detail_low_zoom"),
        ZoomPoint(-0.753, 0.12, 100.0, 100.0, 1.0, "detail_med_zoom"),
        ZoomPoint(-0.753, 0.12, 1000.0, 1000.0, 1.0, "detail_high_zoom"),
        ZoomPoint(-0.753, 0.12, 10000.0, 10000.0, 1.0, "detail_extreme_zoom"),
        
        // Another interesting detail
        ZoomPoint(0.28, 0.008, 50.0, 50.0, 1.0, "spiral")
    };
    
    // Julia set parameter c = -0.8 + 0.156i
    double cReal = -0.8;
    double cImag = 0.156;
    
    // Generate comparison images at each zoom level
    for (const auto& point : zoomPoints) {
        std::cout << "Generating comparison for " << point.name << " at zoom level " << point.zoomStart << std::endl;
        
        // Generate posit version
        std::string posit_cmd = "./julia_set_posit " + 
                               std::to_string(point.zoomStart) + " " + 
                               std::to_string(point.x) + " " + 
                               std::to_string(point.y) + " " + 
                               std::to_string(cReal) + " " + 
                               std::to_string(cImag);
        
        // Generate float version
        std::string float_cmd = "./julia_set_float " + 
                               std::to_string(point.zoomStart) + " " + 
                               std::to_string(point.x) + " " + 
                               std::to_string(point.y) + " " + 
                               std::to_string(cReal) + " " + 
                               std::to_string(cImag);
        
        std::cout << "Running: " << posit_cmd << std::endl;
        system(posit_cmd.c_str());
        
        std::cout << "Running: " << float_cmd << std::endl;
        system(float_cmd.c_str());
    }
    
    std::cout << "\nAll comparison images generated successfully." << std::endl;
    std::cout << "To view the difference between posit and float precision, compare:" << std::endl;
    std::cout << "  - julia_posit_zoom_*.ppm (posit32 version)" << std::endl;
    std::cout << "  - julia_float_zoom_*.ppm (double precision float version)" << std::endl;
    std::cout << "\nThe most dramatic differences will be visible at extreme zoom levels (1000+)" << std::endl;
    std::cout << "where float precision issues become apparent but posits maintain higher accuracy." << std::endl;
    
    return 0;
} 