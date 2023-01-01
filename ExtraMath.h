#pragma once
// Assuming all polygons have four sides (it's all we need for now)
#define POLYGON_NUM_POINTS 4

#include <stdlib.h>
#include <time.h>

#include "Engine/Asset.h"
#include "utils/JavidCollision.h"

#define PI 3.14159265
#define ROOT2 1.41421356237
#define ToRadians(a) a *PI / 180


float pyth_s(float x, float y, float x2, float y2);

float pyth(float a, float b);

// min inclusive, max exclusive.
int random(int min, int max);

float min(float a, float b);

float max(float a, float b);

int abs(int n);

bool AABB(float x, float y, float w, float h, float x2, float y2, float w2, float h2);

bool AABB(Asset_Ase *a, Asset_Ase *b, float x, float y, float x2, float y2);

bool AABB_Movement(float x, float y, float w, float h, Asset_Ase *a, float x2, float y2);

bool AABB_Movement(Rect rect, Asset_Ase *a, float x2, float y2);

bool AABB_Collision(float x, float y, float w, float h, Asset_Ase *a, float x2, float y2);

// This explains it better than I could:
// http://jeffreythompson.org/collision-detection/line-line.php
bool LineLine(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);

bool LineRect(float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh);

bool PointRect(float x, float y, Rect *rect);

// Not safe
void RectToV2(FloatRect *r, v2 *vertices);

// Assuming polygon has 4 sides
template<typename A, typename B>
bool PolygonPoint(v2 *vertices, A px, B py);

void RotatePoint(v2 *point, v2 *source, float angle);
void RotatePoint(v2 *point, float pivot_x, float pivot_y, float angle);
v2 RotatePoint(float point_x, float point_y, v2* pivot_point, float angle);

// Assuming polygon has 4 sides
bool PolygonRectangle(v2 *vertices, FloatRect *rect);

// in radians
float AngleBetweenTwoPoints(int target_x, int target_y, int origin_x, int origin_y);





#ifdef ENGINE_IMPLEMENTATION





float pyth_s(float x, float y, float x2, float y2) {
    return (x - x2) * (x - x2) + (y - y2) * (y - y2);
}

float pyth(float a, float b) {
    return sqrt(a * a + b * b);
}

// min inclusive, max exclusive.
int random(int min, int max) {
    return rand() % (max - min) + min;
}

float min(float a, float b) {
    if (b < a)
        return b;
    return a;
}

float max(float a, float b) {
    if (b > a)
        return b;
    return a;
}

int abs(int n) {
    return (n < 0) ? -n : n;
}

bool AABB(float x, float y, float w, float h, float x2, float y2, float w2, float h2) {
    return x + w > x2 && x < x2 + w2 && y + h > y2 && y < y2 + h2;
}

bool AABB(Asset_Ase *a, Asset_Ase *b, float x, float y, float x2, float y2) {
    return x + a->collision_box->x + a->collision_box->w > x2 + b->collision_box->x &&
           x + a->collision_box->x < x2 + b->collision_box->x + b->collision_box->w &&
           y + a->collision_box->y + a->collision_box->h > y2 + b->collision_box->y &&
           y + a->collision_box->y < y2 + b->collision_box->y + b->collision_box->h;
}

bool AABB_Movement(float x, float y, float w, float h, Asset_Ase *a, float x2, float y2) {
    return x + w > x2 + a->movement_box->x && x < x2 + a->movement_box->x + a->movement_box->w &&
           y + h > y2 + a->movement_box->y && y < y2 + a->movement_box->y + a->movement_box->h;
}

bool AABB_Movement(Rect rect, Asset_Ase *a, float x2, float y2) {
    return AABB_Movement(rect.x, rect.y, rect.w, rect.h, a, x2, y2);
}

bool AABB_Collision(float x, float y, float w, float h, Asset_Ase *a, float x2, float y2) {
    return x + w > x2 + a->collision_box->x && x < x2 + a->collision_box->x + a->collision_box->w &&
           y + h > y2 + a->collision_box->y && y < y2 + a->collision_box->y + a->collision_box->h;
}

// This explains it better than I could:
// http://jeffreythompson.org/collision-detection/line-line.php
bool LineLine(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    float a = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) /
              ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
    float b = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) /
              ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));

    return a >= 0.0 && a <= 1.0 && b >= 0.0 && b <= 1.0;
}

bool LineRect(float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh) {
    // check if the line has hit any of the rectangle's sides
    bool left = LineLine(x1, y1, x2, y2, rx, ry, rx, ry + rh);
    bool right = LineLine(x1, y1, x2, y2, rx + rw, ry, rx + rw, ry + rh);
    bool top = LineLine(x1, y1, x2, y2, rx, ry, rx + rw, ry);
    bool bottom = LineLine(x1, y1, x2, y2, rx, ry + rh, rx + rw, ry + rh);

    return left || right || top || bottom;
}

bool PointRect(float x, float y, Rect *rect) {
    return x > rect->x && x < rect->x + rect->w && y > rect->y && y < rect->y + rect->h;
}

// Not safe

// v2* SHOULD BE [4]
void RectToV2(FloatRect *r, v2 *vertices) {
    vertices[0] = { r->x, r->y };
    vertices[1] = { r->x + r->w, r->y };
    vertices[2] = { r->x + r->w, r->y + r->h };
    vertices[3] = { r->x, r->y + r->h };
}

// Assuming polygon has 4 sides for now
template<typename A, typename B>
bool PolygonPoint(v2 *vertices, A px, B py) {
    bool collision = false;

    for (int i = 0; i < POLYGON_NUM_POINTS; i++) {
        v2 p1 = vertices[i];
        v2 p2 = vertices[(i + 1) % POLYGON_NUM_POINTS];

        if ((p1.y > py != p2.y > py) && px < (p2.x - p1.x) * (py - p1.y) / (p2.y - p1.y) + p1.x) {
            collision = !collision;
        }
    }

    return collision;
}

void RotatePoint(v2 *point, v2 *source, float angle) {
    float tx = point->x - source->x;
    float ty = point->y - source->y;

    point->x = source->x + tx * cos(angle) - ty * sin(angle);
    point->y = source->y + ty * cos(angle) + tx * sin(angle);
}

void RotatePoint(v2 *point, float pivot_x, float pivot_y, float angle) {
    float tx = point->x - pivot_x;
    float ty = point->y - pivot_y;

    point->x = pivot_x + tx * cos(angle) - ty * sin(angle);
    point->y = pivot_y + ty * cos(angle) + tx * sin(angle);
}

v2 RotatePoint(float point_x, float point_y, v2* pivot_point, float angle) {
    float tx = point_x - pivot_point->x;
    float ty = point_y - pivot_point->y;
    return {
        pivot_point->x + tx * cos(angle) - ty * sin(angle),
        pivot_point->y + ty * cos(angle) + tx * sin(angle)
    };
}

// inline void print_v2_size_four(v2* a) {
    // print("(%f %f), (%f %f), (%f %f), (%f %f)", a[0].x, a[0].y, a[1].x, a[1].y, a[2].x, a[2].y, a[3].x, a[3].y);
// }

bool PolygonRectangle(v2* vertices, FloatRect* rect) {

    // 1. Checks if every line of the polygon collides with the rectangle.
    // 2. Checks if any point of the rectangle is inside the polygon - For
    // rectangles engulfed by polygons
    // 3. Checks if any point of the polygon is inside the rectangle - For
    // polygons engulfed by rectangles

    // for (int i = 0; i < POLYGON_NUM_POINTS; i++) {
    //     v2 p1 = vertices[i];
    //     v2 p2 = vertices[(i + 1) % POLYGON_NUM_POINTS];

    //     if (LineRect(p1.x, p1.y, p2.x, p2.y, rect->x, rect->y, rect->w, rect->h))
    //         return true;
    //     if (PointRect(p1.x, p1.y, rect))
    //         return true;
    // }

    // return PolygonPoint(vertices, rect->x, rect->y) ||
    //        PolygonPoint(vertices, rect->x + rect->w, rect->y) ||
    //        PolygonPoint(vertices, rect->x + rect->w, rect->y + rect->h) ||
    //        PolygonPoint(vertices, rect->x, rect->y + rect->h);

    v2 converted_to_v2 [4];
    RectToV2(rect, (v2*) & converted_to_v2);

    // print("%f %f %f %f", rect->x, rect->y, rect->w, rect->h);
    // print_v2_size_four(converted_to_v2);


    return ShapeOverlap_SAT(vertices, (v2*) & converted_to_v2);
}


float AngleBetweenTwoPoints(int target_x, int target_y, int origin_x, int origin_y) {
    return atan2(target_y - origin_y, target_x - origin_x);
}

#endif