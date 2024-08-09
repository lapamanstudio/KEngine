#include "window/gui/panels/workscene/gui/overlay/PanelRenderUtils.h"

std::vector<float> generateRoundedRectVertices(float x, float y, float width, float height, float radius) {
    std::vector<float> vertices;

    // Calculate number of segments for a quarter circle
    const int segments = 20;
    const float angleIncrement = M_PI / 2.0f / segments;

    // Center points for the rounded corners
    float cx[4] = { x + radius, x + width - radius, x + width - radius, x + radius };
    float cy[4] = { y + radius, y + radius, y + height - radius, y + height - radius };
    float startAngle[4] = { M_PI, -M_PI / 2, 0, M_PI / 2 };

    // Generate vertices for each corner
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j <= segments; ++j) {
            float angle = startAngle[i] + j * angleIncrement;
            vertices.push_back(cx[i] + radius * cos(angle));
            vertices.push_back(cy[i] + radius * sin(angle));
            vertices.push_back(0.0f);
        }
    }

    return vertices;
}
