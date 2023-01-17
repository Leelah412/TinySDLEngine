#include "Camera.h"

#include <iostream>

namespace tse{

Camera::Camera(){
	set_viewport_size(640, 480);

	update_projection_matrix();
}

Camera::Camera(PROJECTION type, int width, int height){
	m_type = type;
	set_viewport_size(width, height);
	update_projection_matrix();
}

Camera::~Camera(){

}

const glm::mat4& Camera::get_camera_view() const{
	return m_projection * m_view;
}


const glm::mat4& Camera::get_projection_matrix() const{
	return m_projection;
}

void Camera::set_projection_matrix(glm::mat4 matrix){
	m_projection = matrix;
}

void Camera::update_projection_matrix(){

	if(m_type == ORTHOGONAL){
		m_projection = glm::ortho(-m_aspect_ratio, m_aspect_ratio, -1.0f, 1.0f, m_near_plane, m_far_plane);
	}
	else{
		m_projection = glm::perspective(glm::radians(m_fov / 2), m_aspect_ratio, m_near_plane, m_far_plane);
	}
	m_view = glm::translate(glm::mat4(1.0f), m_position);

}

const glm::mat4& Camera::get_view_matrix() const{
	return m_view;
}

void Camera::set_view_matrix(glm::mat4 matrix){
	m_view = matrix;
}

int Camera::get_viewport_x(){
	return m_viewport_x;
}

void Camera::set_viewport_x(int x){
	m_viewport_x = x;
}

int Camera::get_viewport_y(){
	return m_viewport_y;
}

void Camera::set_viewport_y(int y){
	m_viewport_y = y;
}

int Camera::get_viewport_width(){
	return m_viewport_width;
}

void Camera::set_viewport_width(int width, bool update_ratio){
	m_viewport_width = width;
	if(update_ratio) update_aspect_ratio();
}

int Camera::get_viewport_height(){
	return m_viewport_height;
}

void Camera::set_viewport_height(int height, bool update_ratio){
	m_viewport_height = height;
	if(update_ratio) update_aspect_ratio();
}

void Camera::set_viewport_size(int width, int height, bool update_ratio){
	m_viewport_width = width;
	m_viewport_height = height;
	if(update_ratio) update_aspect_ratio();
}

float Camera::get_aspect_ratio() const{
	return m_aspect_ratio;
}

void Camera::set_aspect_ratio(float ratio){
	m_aspect_ratio = ratio;
}

void Camera::set_aspect_ratio(int width, int height){
	// dont divide by zero
	if(!height){
		m_aspect_ratio = std::numeric_limits<float>().infinity();
		return;
	}
	m_aspect_ratio = (float)width / (float)height;
}

void Camera::update_aspect_ratio(){
	// dont divide by zero
	if(!m_viewport_height){
		m_aspect_ratio = std::numeric_limits<float>().infinity();
		return;
	}
	m_aspect_ratio = (float)m_viewport_width/(float)m_viewport_height;
}

float Camera::get_fov() const{
	return m_fov;
}

void Camera::set_fov(float fov){
	m_fov = fov;
}

float Camera::get_near_plane() const{
	return m_near_plane;
}

void Camera::set_near_plane(float near){
	m_near_plane = near;
}

float Camera::get_far_plane() const{
	return m_far_plane;
}

void Camera::set_far_plane(float far){
	m_far_plane = far;
}

const glm::vec3& Camera::get_position() const{
	return m_position;
}

void Camera::set_position(glm::vec3 pos){
	m_position = pos;
}

float Camera::get_pitch() const{
	return m_pitch;
}

void Camera::set_pitch(float pitch){
	m_pitch = pitch;
}

float Camera::get_yaw() const{
	return m_yaw;
}

void Camera::set_yaw(float yaw){
	m_yaw = yaw;
}

float Camera::get_roll() const{
	return m_roll;
}

void Camera::set_roll(float roll){
	m_roll = roll;
}

}

