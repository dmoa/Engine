/*
	Convex Polygon Collision Detection
	"Don't you dare try concave ones..." - javidx9
	License (OLC-3)
	~~~~~~~~~~~~~~~
	Copyright 2018-2019 OneLoneCoder.com
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:
	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.
	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.
	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	Instructions:
	~~~~~~~~~~~~~
	Use arrow keys to control pentagon
	Use WASD to control triangle
	F1..F4 selects algorithm
	Relevant Video: https://youtu.be/7Ik2vowGcU0
	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Patreon:	https://www.patreon.com/javidx9
	Homepage:	https://www.onelonecoder.com
	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2019
*/


#include "../ExtraMath.h"

#define vec2d v2

struct polygon
{
    std::vector<vec2d> p;	// Transformed Points
    vec2d pos;				// Position of shape
    float angle;			// Direction of shape
    std::vector<vec2d> o;	// "Model" of shape
    bool overlap = false;	// Flag to indicate if overlap has occurred
};

bool ShapeOverlap_SAT(v2* r1, v2* r2);





#ifdef ENGINE_IMPLEMENTATION



inline void print_v2_size_four(v2* a) {
    print("(%f %f), (%f %f), (%f %f), (%f %f)", a[0].x, a[0].y, a[1].x, a[1].y, a[2].x, a[2].y, a[3].x, a[3].y);
}


bool ShapeOverlap_SAT(v2* r1, v2* r2)
{

    v2* poly1 = r1;
    v2* poly2 = r2;

    for (int shape = 0; shape < 2; shape++)
    {
        if (shape == 1)
        {
            poly1 = r2;
            poly2 = r1;
        }
        for (int a = 0; a < POLYGON_NUM_POINTS; a++)
        {
            int b = (a + 1) % POLYGON_NUM_POINTS;
            vec2d axisProj = { -(poly1[b].y - poly1[a].y), poly1[b].x - poly1[a].x };
            float d = sqrtf(axisProj.x * axisProj.x + axisProj.y * axisProj.y);
            axisProj = { axisProj.x / d, axisProj.y / d };

            // Work out min and max 1D points for r1
            float min_r1 = INFINITY, max_r1 = -INFINITY;
            for (int p = 0; p < POLYGON_NUM_POINTS; p++)
            {
                float q = (poly1[p].x * axisProj.x + poly1[p].y * axisProj.y);
                min_r1 = std::min(min_r1, q);
                max_r1 = std::max(max_r1, q);
            }

            // Work out min and max 1D points for r2
            float min_r2 = INFINITY, max_r2 = -INFINITY;
            for (int p = 0; p < POLYGON_NUM_POINTS; p++)
            {
                float q = (poly2[p].x * axisProj.x + poly2[p].y * axisProj.y);
                min_r2 = std::min(min_r2, q);
                max_r2 = std::max(max_r2, q);
            }

            if (!(max_r2 >= min_r1 && max_r1 >= min_r2))
                return false;
        }
    }

    return true;
}

bool ShapeOverlap_SAT_STATIC(polygon &r1, polygon &r2)
{
    polygon *poly1 = &r1;
    polygon *poly2 = &r2;

    float overlap = INFINITY;

    for (int shape = 0; shape < 2; shape++)
    {
        if (shape == 1)
        {
            poly1 = &r2;
            poly2 = &r1;
        }

        for (int a = 0; a < POLYGON_NUM_POINTS; a++)
        {
            int b = (a + 1) % POLYGON_NUM_POINTS;
            vec2d axisProj = { -(poly1->p[b].y - poly1->p[a].y), poly1->p[b].x - poly1->p[a].x };

            // Optional normalisation of projection axis enhances stability slightly
            //float d = sqrtf(axisProj.x * axisProj.x + axisProj.y * axisProj.y);
            //axisProj = { axisProj.x / d, axisProj.y / d };

            // Work out min and max 1D points for r1
            float min_r1 = INFINITY, max_r1 = -INFINITY;
            for (int p = 0; p < POLYGON_NUM_POINTS; p++)
            {
                float q = (poly1->p[p].x * axisProj.x + poly1->p[p].y * axisProj.y);
                min_r1 = std::min(min_r1, q);
                max_r1 = std::max(max_r1, q);
            }

            // Work out min and max 1D points for r2
            float min_r2 = INFINITY, max_r2 = -INFINITY;
            for (int p = 0; p < POLYGON_NUM_POINTS; p++)
            {
                float q = (poly2->p[p].x * axisProj.x + poly2->p[p].y * axisProj.y);
                min_r2 = std::min(min_r2, q);
                max_r2 = std::max(max_r2, q);
            }

            // Calculate actual overlap along projected axis, and store the minimum
            overlap = std::min(std::min(max_r1, max_r2) - std::max(min_r1, min_r2), overlap);

            if (!(max_r2 >= min_r1 && max_r1 >= min_r2))
                return false;
        }
    }

    // If we got here, the objects have collided, we will displace r1
    // by overlap along the vector between the two object centers
    vec2d d = { r2.pos.x - r1.pos.x, r2.pos.y - r1.pos.y };
    float s = sqrtf(d.x*d.x + d.y*d.y);
    r1.pos.x -= overlap * d.x / s;
    r1.pos.y -= overlap * d.y / s;
    return false;
}

// Use edge/diagonal intersections.
bool ShapeOverlap_DIAGS(polygon &r1, polygon &r2)
{
    polygon *poly1 = &r1;
    polygon *poly2 = &r2;

    for (int shape = 0; shape < 2; shape++)
    {
        if (shape == 1)
        {
            poly1 = &r2;
            poly2 = &r1;
        }

        // Check diagonals of polygon...
        for (int p = 0; p < POLYGON_NUM_POINTS; p++)
        {
            vec2d line_r1s = poly1->pos;
            vec2d line_r1e = poly1->p[p];

            // ...against edges of the other
            for (int q = 0; q < POLYGON_NUM_POINTS; q++)
            {
                vec2d line_r2s = poly2->p[q];
                vec2d line_r2e = poly2->p[(q + 1) % POLYGON_NUM_POINTS];

                // Standard "off the shelf" line segment intersection
                float h = (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r1e.y) - (line_r1s.x - line_r1e.x) * (line_r2e.y - line_r2s.y);
                float t1 = ((line_r2s.y - line_r2e.y) * (line_r1s.x - line_r2s.x) + (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r2s.y)) / h;
                float t2 = ((line_r1s.y - line_r1e.y) * (line_r1s.x - line_r2s.x) + (line_r1e.x - line_r1s.x) * (line_r1s.y - line_r2s.y)) / h;

                if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

// Use edge/diagonal intersections.
bool ShapeOverlap_DIAGS_STATIC(polygon &r1, polygon &r2)
{
    polygon *poly1 = &r1;
    polygon *poly2 = &r2;

    for (int shape = 0; shape < 2; shape++)
    {
        if (shape == 1)
        {
            poly1 = &r2;
            poly2 = &r1;
        }

        // Check diagonals of this polygon...
        for (int p = 0; p < POLYGON_NUM_POINTS; p++)
        {
            vec2d line_r1s = poly1->pos;
            vec2d line_r1e = poly1->p[p];

            vec2d displacement = { 0,0 };

            // ...against edges of this polygon
            for (int q = 0; q < POLYGON_NUM_POINTS; q++)
            {
                vec2d line_r2s = poly2->p[q];
                vec2d line_r2e = poly2->p[(q + 1) % POLYGON_NUM_POINTS];

                // Standard "off the shelf" line segment intersection
                float h = (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r1e.y) - (line_r1s.x - line_r1e.x) * (line_r2e.y - line_r2s.y);
                float t1 = ((line_r2s.y - line_r2e.y) * (line_r1s.x - line_r2s.x) + (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r2s.y)) / h;
                float t2 = ((line_r1s.y - line_r1e.y) * (line_r1s.x - line_r2s.x) + (line_r1e.x - line_r1s.x) * (line_r1s.y - line_r2s.y)) / h;

                if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f)
                {
                    displacement.x += (1.0f - t1) * (line_r1e.x - line_r1s.x);
                    displacement.y += (1.0f - t1) * (line_r1e.y - line_r1s.y);
                }
            }

            r1.pos.x += displacement.x * (shape == 0 ? -1 : +1);
            r1.pos.y += displacement.y * (shape == 0 ? -1 : +1);
        }
    }

    // Cant overlap if static collision is resolved
    return false;
}




#endif