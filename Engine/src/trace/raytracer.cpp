/****************************************************************************
 * Copyright ©2017 Brian Curless.  All rights reserved.  Permission is hereby
 * granted to students registered for University of Washington CSE 457 or CSE
 * 557 for use solely during Autumn Quarter 2017 for purposes of the course.
 * No other use, copying, distribution, or modification is permitted without
 * prior written consent. Copyrights for third-party components of this work
 * must be honored.  Instructors interested in reusing these course materials
 * should contact the author.
 ****************************************************************************/
#include "trace/raytracer.h"
#include <scene/scene.h>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <glm/gtx/component_wise.hpp>
#include <scene/components/triangleface.h>
#include <glm/gtx/string_cast.hpp>
#include "components.h"

unsigned int pow4(unsigned int e) {
    unsigned int i = 1;
    while(e>0) {
        i *= 4;
        e--;
    }
    return i;
}

RayTracer::RayTracer(Scene& scene, SceneObject& camobj) :
    trace_scene(&scene, camobj.GetComponent<Camera>()->TraceEnableAcceleration.Get()), next_render_index(0), cancelling(false), first_pass_buffer(nullptr)
{
    Camera* cam = camobj.GetComponent<Camera>();

    settings.width = cam->RenderWidth.Get();
    settings.height = cam->RenderHeight.Get();
    settings.pixel_size_x = 1.0/double(settings.width);
    settings.pixel_size_y = 1.0/double(settings.height);
}

RayTracer::~RayTracer() {
}

int RayTracer::GetProgress() {
  return 100;
}

double RayTracer::AspectRatio() {
    return ((double)settings.width)/((double)settings.height);
}

void RayTracer::ComputePixel(int i, int j, Camera* debug_camera) {
    // Calculate the normalized coordinates [0, 1]
    double x_corner = i * settings.pixel_size_x;
    double y_corner = j * settings.pixel_size_y;

    if (debug_camera) {
        if (debug_camera_used_) {
            debug_camera_used_->ClearDebugRays();
        }
        debug_camera_used_ = debug_camera;
        SampleCamera(x_corner, y_corner, settings.pixel_size_x, settings.pixel_size_y, debug_camera);
        return;
    }

    // Trace the ray!
    glm::vec3 color(0,0,0);

    switch (settings.samplecount_mode) {
        case Camera::TRACESAMPLING_CONSTANT:
            color = SampleCamera(x_corner, y_corner, settings.pixel_size_x, settings.pixel_size_y, debug_camera);
            break;
        default:
            break;
    }

    color = glm::clamp(color, 0.0f, 1.0f);

    // Set the pixel in the render buffer
    uint8_t* pixel = buffer + (i + j * settings.width) * 3;
    pixel[0] = (uint8_t)( 255.0f * color[0]);
    pixel[1] = (uint8_t)( 255.0f * color[1]);
    pixel[2] = (uint8_t)( 255.0f * color[2]);
}


glm::vec3 RayTracer::SampleCamera(double x_corner, double y_corner, double pixel_size_x, double pixel_size_y, Camera* debug_camera) {
    glm::vec2 aa_sample = glm::dvec2(0.5,0.5);
    double x = x_corner + pixel_size_x * aa_sample.x;
    double y = y_corner + pixel_size_y * aa_sample.y;

    glm::dvec3 point_on_focus_plane = settings.projection_origin + settings.projection_forward + (2.0*x-1.0)*settings.projection_right + (2.0*y-1.0)*settings.projection_up;

    glm::vec2 sample = glm::dvec2(0,0);
    double angle = sample.x;
    double radius = sqrt(sample.y);

    glm::dvec3 origin = settings.projection_origin + radius * (sin(angle) * settings.aperture_up + cos(angle) * settings.aperture_right);

    glm::dvec3 dir = glm::normalize(point_on_focus_plane - origin);

    Ray camera_ray(origin, dir);

    return TraceRay(camera_ray, 0, RayType::camera, debug_camera);
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
// Depth is the number of times the ray has intersected an object.
glm::vec3 RayTracer::TraceRay(const Ray& r, int depth, RayType ray_type, Camera* debug_camera) {
    Intersection i;

    if (debug_camera) {
        glm::dvec3 endpoint = r.at(1000);
        if (trace_scene.Intersect(r, i)) {
            endpoint = r.at(i.t);
            debug_camera->AddDebugRay(endpoint, endpoint+0.25*(glm::dvec3)i.normal, RayType::hit_normal);
        }
        debug_camera->AddDebugRay(r.position, endpoint, ray_type);
    }

    if (trace_scene.Intersect(r, i)) {
        // An intersection occured!  We've got work to do. For now,
        // this code gets the material parameters for the surface
        // that was intersected.
        Material* mat = i.GetMaterial();
        glm::vec3 I_direct = mat->Diffuse->GetColorUV(i.uv);
        return I_direct;

        // This is a great place to insert code for recursive ray tracing.
        // Compute the blinn-phong shading, and don't stop there:
        // add in contributions from reflected and refracted rays.

        // To iterate over all light sources in the scene, use code like this:
        // for (auto j = trace_scene.lights.begin(); j != trace_scene.lights.end(); j++) {
        //   TraceLight* trace_light = *j;
        //   Light* scene_light = trace_light->light;
        // }


        // No intersection. This ray travels to infinity, so we color it according to the background color,
        // which in this (simple) case is just black.
        glm::vec3 background_color = glm::vec3(0, 0, 0);

        return background_color;
    }
}
