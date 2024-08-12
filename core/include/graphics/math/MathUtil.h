#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class MathUtil {
public:
    static bool IsPointInRect(const glm::vec2& point, const glm::vec2& rectPos, const glm::vec2& rectSize, float rotation);


    /**
     * @brief Checks if two rectangles, one potentially rotated, are intersecting.
     *
     * This method is based on an optimized version of the Separating Axis Theorem (SAT),
     * specifically tailored for rectangle collision detection. The SAT states that two
     * convex polygons (like rectangles) do not intersect if there exists an axis along which
     * their projections do not overlap.
     * 
     * For rectangles:
     * - If both rectangles are axis-aligned, the relevant axes are their sides parallel to
     *   the X and Y axes.
     * - If one rectangle is rotated, the relevant axes are its rotated edges.
     * 
     * The method projects the corners of both rectangles onto these axes and checks for overlap.
     * If there's an axis where the projections do not overlap, the rectangles are not colliding.
     * If all projections overlap, the rectangles are intersecting.
     * 
     * This approach is optimal for rectangles as it efficiently handles both axis-aligned and
     * rotated cases with minimal computational overhead.
     * 
     * @param pos1 Position of the top-left corner of the first rectangle.
     * @param size1 Size (width and height) of the first rectangle.
     * @param pos2 Position of the top-left corner of the second rectangle.
     * @param size2 Size (width and height) of the second rectangle.
     * @param rotation2 Rotation angle of the second rectangle, in degrees.
     * @return true if the rectangles intersect; false otherwise.
     */
    static bool DoRectsIntersect(const glm::vec2& pos1, const glm::vec2& size1, const glm::vec2& pos2, const glm::vec2& size2, float rotation);

    static bool IsPointInRect(const glm::vec2& point, const glm::vec2& rectPos, const glm::vec2& rectSize);

private:
    static std::vector<glm::vec2> GetCorners(const glm::vec2& pos, const glm::vec2& size, float rotation);
    static glm::vec2 RotatePointAroundCenter(const glm::vec2& point, const glm::vec2& center, float rotation);
    static bool IsSeparatingAxis(const std::vector<glm::vec2>& rect1Corners, const std::vector<glm::vec2>& rect2Corners);
    static void ProjectOntoAxis(const std::vector<glm::vec2>& corners, const glm::vec2& axis, float& min, float& max);
};

#endif // MATH_UTIL_H
