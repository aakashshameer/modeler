/****************************************************************************
 * Copyright ©2017 Brian Curless.  All rights reserved.  Permission is hereby
 * granted to students registered for University of Washington CSE 457 or CSE
 * 557 for use solely during Autumn Quarter 2017 for purposes of the course.
 * No other use, copying, distribution, or modification is permitted without
 * prior written consent. Copyrights for third-party components of this work
 * must be honored.  Instructors interested in reusing these course materials
 * should contact the author.
 ****************************************************************************/
#include "meshprocessing.h"
#include <algorithm>

void MeshProcessing::ComputeNormals(Mesh& mesh) {
    // REQUIREMENT: Recompute the normals for a mesh by taking average of the normals of the adjacent faces
    //              1) Note here you need to average adjacent face normals, which need to be computed, not vertex normals
    //              2) As for average method, you can use either unweighted average or face-area-weighted average.
    //                 In solution we use weighted average, so if you want to match the solution, you can implement face-area-weighted average
    //                 But you won't lose points if you do unweighted average, which is easier to implement.
    std::vector<float> positions = mesh.GetPositions();
    std::vector<unsigned int> triangles = mesh.GetTriangles();
    std::vector<float> new_normals(positions.size());

    unsigned int pos_index = 0;
    for (unsigned int i = 0; i < new_normals.size(); i+=3) {
        std::vector<float> adj_normals;
        for (unsigned int j = 0; j < triangles.size(); j++) {
            if (3*triangles[j] == pos_index) {
                float u_x = positions[3*triangles[j - j%3 + 1]] - positions[3*triangles[j - j%3]];
                float u_y = positions[3*triangles[j - j%3 + 1] + 1] - positions[3*triangles[j - j%3] + 1];
                float u_z = positions[3*triangles[j - j%3 + 1] + 2] - positions[3*triangles[j - j%3] + 2];

                float v_x = positions[3*triangles[j - j%3 + 2]] - positions[3*triangles[j - j%3]];
                float v_y = positions[3*triangles[j - j%3 + 2] + 1] - positions[3*triangles[j - j%3] + 1];
                float v_z = positions[3*triangles[j - j%3 + 2] + 2] - positions[3*triangles[j - j%3] + 2];

                float norm_x = u_y*v_z - u_z*v_y;
                float norm_y = u_z*v_x - u_x*v_z;
                float norm_z = u_x*v_y - u_y*v_x;
                float mag = sqrt(norm_x*norm_x + norm_y*norm_y + norm_z*norm_z);

                adj_normals.push_back(norm_x/mag);
                adj_normals.push_back(norm_y/mag);
                adj_normals.push_back(norm_z/mag);
            }
        }
        float x_sum = 0;
        float y_sum = 0;
        float z_sum = 0;
        for (unsigned int k = 0; k < adj_normals.size(); k+=3) {
            x_sum += adj_normals[k];
            y_sum += adj_normals[k+1];
            z_sum += adj_normals[k+2];
        }
        new_normals[i] = x_sum/(adj_normals.size()/3);
        new_normals[i+1] = y_sum/(adj_normals.size()/3);
        new_normals[i+2] = z_sum/(adj_normals.size()/3);
        pos_index += 3;
    }

    mesh.SetNormals(new_normals);
}

void MeshProcessing::FilterMesh(const Mesh& input_mesh, Mesh& filtered_mesh, double a) {
    const std::vector<float>& input_positions = input_mesh.GetPositions();
    const std::vector<float>& input_normals = input_mesh.GetNormals();
    const std::vector<float>& input_UVs = input_mesh.GetUVs();
    const std::vector<unsigned int>& input_faces = input_mesh.GetTriangles();

    // REQUIREMENT: Take a weighted sum of the vertex and its neighbors to produce a new mesh
    // with the same connectivity as the original mesh, but with updated vertex positions.
    // Vertices are neighbors if they share an edge. Filter weights will be 1 for the vertex,
    // and a / N for each neighboring vertex. The weights will then be normalized.
    // "a" controls smoothing or sharpening, and N is the number of neighboring vertices.
    std::vector<float> output_positions(input_positions.size());

    for (unsigned int i = 0; i < input_positions.size(); i+=3) {
        std::vector<float> adj_vertices;
        for (unsigned int j = 0; j < input_faces.size(); j++) {
            if (3*input_faces[j] == i) {
                switch(j%3) {
                    case 0 :
                        adj_vertices.push_back(input_positions[3*input_faces[j+1]]);
                        adj_vertices.push_back(input_positions[3*input_faces[j+1] + 1]);
                        adj_vertices.push_back(input_positions[3*input_faces[j+1] + 2]);

                        adj_vertices.push_back(input_positions[3*input_faces[j+2]]);
                        adj_vertices.push_back(input_positions[3*input_faces[j+2] + 1]);
                        adj_vertices.push_back(input_positions[3*input_faces[j+2] + 2]);
                        break;
                    case 1 :
                        adj_vertices.push_back(input_positions[3*input_faces[j-1]]);
                        adj_vertices.push_back(input_positions[3*input_faces[j-1] + 1]);
                        adj_vertices.push_back(input_positions[3*input_faces[j-1] + 2]);

                        adj_vertices.push_back(input_positions[3*input_faces[j+1]]);
                        adj_vertices.push_back(input_positions[3*input_faces[j+1] + 1]);
                        adj_vertices.push_back(input_positions[3*input_faces[j+1] + 2]);
                        break;
                    case 2 :
                        adj_vertices.push_back(input_positions[3*input_faces[j-2]]);
                        adj_vertices.push_back(input_positions[3*input_faces[j-2] + 1]);
                        adj_vertices.push_back(input_positions[3*input_faces[j-2] + 2]);

                        adj_vertices.push_back(input_positions[3*input_faces[j-1]]);
                        adj_vertices.push_back(input_positions[3*input_faces[j-1] + 1]);
                        adj_vertices.push_back(input_positions[3*input_faces[j-1] + 2]);
                        break;
                }
            }
        }
        float x_sum = input_positions[i]/(a+1);
        float y_sum = input_positions[i+1]/(a+1);
        float z_sum = input_positions[i+2]/(a+1);
        for (unsigned int k = 0; k < adj_vertices.size(); k+=3) {
            x_sum += adj_vertices[k] * a/(adj_vertices.size()/3.0f)/(a+1);
            y_sum += adj_vertices[k+1] * a/(adj_vertices.size()/3.0f)/(a+1);
            z_sum += adj_vertices[k+2] * a/(adj_vertices.size()/3.0f)/(a+1);
        }
        output_positions[i] = x_sum;
        output_positions[i+1] = y_sum;
        output_positions[i+2] = z_sum;
    }

    filtered_mesh.SetPositions(output_positions);
    filtered_mesh.SetNormals(input_normals);
    filtered_mesh.SetUVs(input_UVs);
    filtered_mesh.SetTriangles(input_faces);
    ComputeNormals(filtered_mesh);
}

void MeshProcessing::SubdivideMesh(const Mesh& input_mesh, Mesh& filtered_mesh, bool limit) {
    const std::vector<float>& input_positions = input_mesh.GetPositions();
    const std::vector<float>& input_normals = input_mesh.GetNormals();
    const std::vector<float>& input_UVs = input_mesh.GetUVs();
    const std::vector<unsigned int>& input_faces = input_mesh.GetTriangles();

    // EXTRA CREDIT (457): Perform one iteration of Loop subdivision on the mesh, assuming
    // the mesh is watertight (has no boundary vertices or edges).
    // For an extra bell, also detect and subdivide boundary vertices and edges

    filtered_mesh.SetPositions(input_positions);
    filtered_mesh.SetNormals(input_normals);
    filtered_mesh.SetUVs(input_UVs);
    filtered_mesh.SetTriangles(input_faces);

    ComputeNormals(filtered_mesh);
}

void MeshProcessing::FlipNormals(const Mesh& input_mesh, Mesh& filtered_mesh) {
    const std::vector<float>& input_normals = input_mesh.GetNormals();
    const std::vector<unsigned int>& input_faces = input_mesh.GetTriangles();
    std::vector<float> output_normals;
    std::vector<unsigned int> output_faces;

    for (unsigned int i = 0; i < input_normals.size(); i++) {
        output_normals.push_back(-input_normals[i]);
    }

    // Also need to change face winding order so that backfaces are now frontfaces
    for (unsigned int i = 0; i < input_faces.size(); i += 3) {
        output_faces.push_back(input_faces[i]);
        output_faces.push_back(input_faces[i+2]);
        output_faces.push_back(input_faces[i+1]);
    }

    filtered_mesh.SetPositions(input_mesh.GetPositions());
    filtered_mesh.SetNormals(output_normals);
    filtered_mesh.SetUVs(input_mesh.GetUVs());
    filtered_mesh.SetTriangles(output_faces);
}
