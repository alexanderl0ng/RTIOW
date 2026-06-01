#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <cstdint>
#endif

class camera {
	public:
		float aspect_ratio 		= 1.0;
		int	  image_width  		= 100;
		int   samples_per_pixel = 10;
        int   max_depth         = 10;

        float vfov = 90;
        point3 lookfrom = point3(0, 0, 0);
        point3 lookat = point3(0, 0, -1);
        vec3 vup = vec3(0, 1, 0);

        float defocus_angle = 0;
        float focus_dist = 10;

		#ifdef __EMSCRIPTEN__
        void render(const hittable& world, uint8_t* buffer) {
			initialize();
		#else
        void render(const hittable& world) {
			initialize();
			std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
		#endif

			for (int j = 0; j < image_height; j++) {
                #ifndef __EMSCRIPTEN__
				std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
                #endif
				for (int i = 0; i < image_width; i++) {
					color pixel_color(0, 0, 0);
					for (int sample = 0; sample < samples_per_pixel; sample++) {
						ray r = get_ray(i, j);
						pixel_color += ray_color(r, max_depth, world);
					}
                    #ifdef __EMSCRIPTEN__
                    auto c = pixel_samples_scale * pixel_color;
                    int idx = (j * image_width + i) * 4;
                    buffer[idx + 0] = static_cast<uint8_t>(256 * std::clamp(c.x(), 0.0f, 0.999f));
                    buffer[idx + 1] = static_cast<uint8_t>(256 * std::clamp(c.y(), 0.0f, 0.999f));
                    buffer[idx + 2] = static_cast<uint8_t>(256 * std::clamp(c.z(), 0.0f, 0.999f));
                    buffer[idx + 3] = 255;
                    #else
					write_color(std::cout, pixel_samples_scale * pixel_color);
                    #endif
				}
			}
            #ifndef __EMSCRIPTEN__
			std::clog << "\rDone.                      \n";
            #endif
		}

	private:
		int    image_height;
		float  pixel_samples_scale;
		point3 center;
		point3 pixel00_loc;
		vec3   pixel_delta_u;
		vec3   pixel_delta_v;
        vec3   u, v, w;
        vec3   defocus_disk_u;
        vec3   defocus_disk_v;

		void initialize() {
						
			image_height = int(image_width / aspect_ratio);
			image_height = (image_height < 1) ? 1 : image_height;

			pixel_samples_scale = 1.0 / samples_per_pixel;
			
			center = lookfrom;
			
            auto theta = degrees_to_radians(vfov);
            auto h = std::tan(theta / 2);
			auto viewport_height = 2 * h * focus_dist;
			auto viewport_width = viewport_height * (float(image_width)/image_height);

            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            vec3 viewport_u = viewport_width * u;
            vec3 viewport_v = viewport_height * -v;

			pixel_delta_u = viewport_u / image_width;
			pixel_delta_v = viewport_v / image_height;

			auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
			pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;
		}

		ray get_ray(int i, int j) const {
			auto offset = sample_square();
			auto pixel_sample = pixel00_loc
							  + ((i + offset.x()) * pixel_delta_u)
							  + ((j + offset.y()) * pixel_delta_v);

			auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
			auto ray_direction = pixel_sample - ray_origin;

			return ray(ray_origin, ray_direction);
		}

		vec3 sample_square() const {
			return vec3(random_float() - 0.5, random_float() - 0.5, 0);
		}

        point3 defocus_disk_sample() const {
            auto p = random_in_unit_disk();
            return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

		color ray_color(const ray& r, int depth, const hittable& world) const {
            if (depth <= 0)
                return color(0, 0, 0);

			hit_record rec;
			if (world.hit(r, interval(0.001, infinity), rec)) {
                ray scattered;
                color attenuation;
                if (rec.mat->scatter(r, rec, attenuation, scattered))
                    return attenuation * ray_color(scattered, depth - 1, world);
                return color(0, 0, 0);
			}

			vec3 unit_direction = unit_vector(r.direction());
			auto a = 0.5 * (unit_direction.y() + 1.0);
			return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
		}
};

#endif

