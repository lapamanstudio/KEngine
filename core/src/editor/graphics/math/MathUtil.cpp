#include "editor/graphics/math/MathUtil.h"

bool MathUtil::IsPointInRect(const glm::vec2& point, const glm::vec2& rectPos, const glm::vec2& rectSize, float rotation) {
    float radians = glm::radians(-rotation);
    glm::vec2 rotatedPoint;
    rotatedPoint.x = point.x * cos(radians) - point.y * sin(radians);
    rotatedPoint.y = point.x * sin(radians) + point.y * cos(radians);

    return rotatedPoint.x >= rectPos.x && rotatedPoint.x <= rectPos.x + rectSize.x &&
        rotatedPoint.y >= rectPos.y && rotatedPoint.y <= rectPos.y + rectSize.y;
}

bool MathUtil::DoRectsIntersect(const glm::vec2& pos1, const glm::vec2& size1, const glm::vec2& pos2, const glm::vec2& size2, float rotation2) {
    std::vector<glm::vec2> rect1Corners = GetCorners(pos1, size1, 0.0f);
    std::vector<glm::vec2> rect2Corners = GetCorners(pos2, size2, rotation2);

    if (IsSeparatingAxis(rect1Corners, rect2Corners) || IsSeparatingAxis(rect2Corners, rect1Corners)) {
        return false;
    }

    return true;
}

std::vector<glm::vec2> MathUtil::GetCorners(const glm::vec2& pos, const glm::vec2& size, float rotation) {
    std::vector<glm::vec2> corners = {
        pos,
        pos + glm::vec2(size.x, 0),
        pos + size,
        pos + glm::vec2(0, size.y)
    };

    if (rotation != 0.0f) {
        for (auto& corner : corners) {
            corner = RotatePointAroundCenter(corner, pos, rotation);
        }
    }

    return corners;
}

glm::vec2 MathUtil::RotatePointAroundCenter(const glm::vec2& point, const glm::vec2& center, float rotation) {
    float radians = glm::radians(rotation);
    glm::vec2 translatedPoint = point - center;
    glm::vec2 rotatedPoint;
    rotatedPoint.x = translatedPoint.x * cos(radians) - translatedPoint.y * sin(radians);
    rotatedPoint.y = translatedPoint.x * sin(radians) + translatedPoint.y * cos(radians);
    return rotatedPoint + center;
}

bool MathUtil::IsSeparatingAxis(const std::vector<glm::vec2>& rect1Corners, const std::vector<glm::vec2>& rect2Corners) {
    for (int i = 0; i < 4; i++) {
        glm::vec2 edge = rect1Corners[(i + 1) % 4] - rect1Corners[i];
        glm::vec2 axis = glm::normalize(glm::vec2(-edge.y, edge.x));

        float minRect1, maxRect1, minRect2, maxRect2;
        ProjectOntoAxis(rect1Corners, axis, minRect1, maxRect1);
        ProjectOntoAxis(rect2Corners, axis, minRect2, maxRect2);

        if (maxRect1 < minRect2 || maxRect2 < minRect1) {
            return true;
        }
    }

    return false;
}

void MathUtil::ProjectOntoAxis(const std::vector<glm::vec2>& corners, const glm::vec2& axis, float& min, float& max) {
    min = max = glm::dot(corners[0], axis);

    for (int i = 1; i < 4; i++) {
        float projection = glm::dot(corners[i], axis);
        if (projection < min) {
            min = projection;
        }
        if (projection > max) {
            max = projection;
        }
    }
}

bool MathUtil::IsPointInRect(const glm::vec2& point, const glm::vec2& rectPos, const glm::vec2& rectSize) {
    if (point.x < rectPos.x || point.x > rectPos.x + rectSize.x) return false;
    if (point.y < rectPos.y || point.y > rectPos.y + rectSize.y) return false;

    return true;
}
