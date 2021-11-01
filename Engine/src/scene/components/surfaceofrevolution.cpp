/****************************************************************************
 * Copyright ©2017 Brian Curless.  All rights reserved.  Permission is hereby
 * granted to students registered for University of Washington CSE 457 or CSE
 * 557 for use solely during Autumn Quarter 2017 for purposes of the course.
 * No other use, copying, distribution, or modification is permitted without
 * prior written consent. Copyrights for third-party components of this work
 * must be honored.  Instructors interested in reusing these course materials
 * should contact the author.
 ****************************************************************************/
#include "surfaceofrevolution.h"
#include <fileio.h>
#include <math.h>

REGISTER_COMPONENT(SurfaceOfRevolution, Geometry)

SurfaceOfRevolution::SurfaceOfRevolution() :
    Geometry(),
    Curve(FileType::Curve),
    Quality({"High", "Medium", "Low", "Poor"})
{
    AddProperty("Curve", &Curve);
    AddProperty("Quality", &Quality);

    Curve.ValueSet.Connect(this, &SurfaceOfRevolution::OnCurveSet);
    Quality.ValueSet.Connect(this, &SurfaceOfRevolution::OnQualitySet);

    // Add some default points and generate a default mesh
    static const std::vector<glm::vec2> default_points = {
        glm::vec2(0.0f, 0.5f),
        glm::vec2(0.25f, 0.25f),
        glm::vec2(0.5f, 0.0f),
        glm::vec2(0.25f, -0.25f),
        glm::vec2(0.0f, -0.5f)
    };
    points_list_.push_back(default_points);
    OnQualitySet(Quality.Get());
}

void SurfaceOfRevolution::OnCurveSet(std::string curve_file) {
    // Get a list of points indexed by the quality
    points_list_ = FileIO::ReadCurveFile(curve_file);
    OnQualitySet(0);
}

void SurfaceOfRevolution::OnQualitySet(int quality) {
    if (points_list_.size() == 0) return;
    if ((unsigned int) Quality.Get() >= points_list_.size()) quality = 0;
    // Create a new surface of revolution mesh
    mesh_ = CreateMesh(points_list_[quality], points_list_[quality].size());
}

// Transfers ownership of a new Surface of Revolution Mesh to the caller
std::unique_ptr<Mesh> SurfaceOfRevolution::CreateMesh(const std::vector<glm::vec2>& curve_points, unsigned int subdivisions) {
    std::unique_ptr<Mesh> surface = std::make_unique<Mesh>("Surface of Revolution");

    // REQUIREMENT: Compute and set vertex positions, normals, UVs, and triangle faces

    // you need to change the size of these arrays
    std::vector<float> vertices(3*subdivisions*curve_points.size());
    std::vector<float> normals(3*subdivisions*curve_points.size());
    std::vector<float> UVs(2*subdivisions*curve_points.size());
    std::vector<unsigned int> triangles(6*subdivisions*(curve_points.size()-1));

    unsigned int v_index = 0;
    unsigned int n_index = 0;
    unsigned int uv_index = 0;
    unsigned int tri_index = 0;

    for (unsigned int i = 0; i < subdivisions; i++) {
        for (unsigned int j = 0; j < curve_points.size(); j++) {
            vertices[v_index++] = curve_points[j].x * cosf(360.0f/subdivisions*i*M_PI/180.0f); // + 0*y + sin(theta)*z // z = 0
            vertices[v_index++] = curve_points[j].y;
            vertices[v_index++] = curve_points[j].x * (-sinf(360.0f/subdivisions*i*M_PI/180.0f)); // + 0*y + cos(theta)*z // z = 0

            float norm_x = curve_points[j+1 < curve_points.size() ? j+1 : j].y - curve_points[j+1 < curve_points.size() ? j : j-1].y;
            float norm_y = curve_points[j+1 < curve_points.size() ? j : j-1].x - curve_points[j+1 < curve_points.size() ? j+1 : j].x;
            float mag = sqrt(2*norm_x*norm_x + norm_y*norm_y);
            normals[n_index++] = norm_x/mag * cosf(360.0f/subdivisions*i*M_PI/180.0f); // + 0*y + sin(theta)*z // z = 0
            normals[n_index++] = norm_y/mag;
            normals[n_index++] = norm_x/mag * (-sinf(360.0f/subdivisions*i*M_PI/180.0f)); // + 0*y + cos(theta)*z // z = 0

            UVs[uv_index++] = i*1.0f/subdivisions;
            UVs[uv_index++] = curve_points[j].y;

            if (j > 0) {
                triangles[tri_index++] = (i == 0 ? subdivisions-1 : i-1)*curve_points.size() + j;
                triangles[tri_index++] = i*curve_points.size() + j;
                triangles[tri_index++] = i*curve_points.size() + j - 1;

                triangles[tri_index++] = i*curve_points.size() + j - 1;
                triangles[tri_index++] = (i == 0 ? subdivisions-1 : i-1)*curve_points.size() + j - 1;
                triangles[tri_index++] = (i == 0 ? subdivisions-1 : i-1)*curve_points.size() + j;
            }
        }
    }



    surface->SetPositions(vertices);
    surface->SetNormals(normals);
    surface->SetUVs(UVs);
    surface->SetTriangles(triangles);


    return std::move(surface);
}
