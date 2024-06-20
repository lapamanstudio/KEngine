#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class MathUtil {
public:
    static bool IsPointInRect(const glm::vec2& point, const glm::vec2& rectPos, const glm::vec2& rectSize) {
        return point.x >= rectPos.x && point.x <= rectPos.x + rectSize.x &&
               point.y >= rectPos.y && point.y <= rectPos.y + rectSize.y;
    }

    static bool DoRectsIntersect(const glm::vec2& pos1, const glm::vec2& size1, const glm::vec2& pos2, const glm::vec2& size2) {
        return pos1.x < pos2.x + size2.x && pos1.x + size1.x > pos2.x &&
               pos1.y < pos2.y + size2.y && pos1.y + size1.y > pos2.y;
    }
};

#endif // MATH_UTIL_H