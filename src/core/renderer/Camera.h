#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Framebuffer.h"
//#include "BufferEntity.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace tse{

	typedef enum PROJECTION {ORTHOGONAL, PERSPECTIVE};

	class Camera{

	public:
		Camera();
		Camera(PROJECTION type, int width, int height);
		virtual ~Camera();

		PROJECTION get_projection_type() const;
		virtual void set_projection_type(PROJECTION projection);

		// Projection * View
		const glm::mat4& get_camera_view() const;

		const glm::mat4& get_projection_matrix() const;
		virtual void set_projection_matrix(glm::mat4 matrix);
		// Update projection matrix with the currently set values
		virtual void update_projection_matrix();
		const glm::mat4& get_view_matrix() const;
		virtual void set_view_matrix(glm::mat4 matrix);
		// Update view matrix based on current position and rotation
		virtual void update_view_matrix();

		int get_viewport_x();
		virtual void set_viewport_x(int x);
		int get_viewport_y();
		virtual void set_viewport_y(int y);
		int get_viewport_width();
		virtual void set_viewport_width(int width, bool update_ratio = true);
		int get_viewport_height();
		virtual void set_viewport_height(int height, bool update_ratio = true);
		virtual void set_viewport_size(int width, int height, bool update_ratio = true);

		float get_aspect_ratio() const;
		virtual void set_aspect_ratio(float ratio);
		// Compute aspect ratio from given width and height
		virtual void set_aspect_ratio(int width, int height);
		// Update aspect ratio based on viewport width and height
		virtual void update_aspect_ratio();

		float get_fov() const;
		virtual void set_fov(float fov);

		float get_near_plane() const;
		virtual void set_near_plane(float near);

		float get_far_plane() const;
		virtual void set_far_plane(float far);

		const glm::vec3& get_position() const;
		virtual void set_position(glm::vec3 pos);

		float get_pitch() const;
		virtual void set_pitch(float pitch);
		float get_yaw() const;
		virtual void set_yaw(float yaw);
		float get_roll() const;
		virtual void set_roll(float roll);

	private:
		PROJECTION m_type = PERSPECTIVE;

		glm::mat4 m_projection;
		glm::mat4 m_view;

		int m_viewport_x = 0;
		int m_viewport_y = 0;
		int m_viewport_width;
		int m_viewport_height;
		float m_aspect_ratio;
		float m_fov = 70.0f;
		float m_near_plane = 1.0f;
		float m_far_plane = 1000.0f;

		glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
		float m_pitch = 0.0f;
		float m_yaw = 0.0f;
		float m_roll = 0.0f;
	};

}


#endif // !__CAMERA_H__
