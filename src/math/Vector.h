#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <math.h>

// deprecated, use glm instead

typedef struct Vector2{
	float x;
	float y;

	Vector2() : x{0}, y{0}{}
	Vector2(float _x, float _y) : x{_x}, y{_y}{}

	float length(){
		return sqrtf(x * x + y * y);
	}

	float angle(){
		return angle(Vector2(1, 0));
	}
	float angle(Vector2 to){
		// handle division by zero exception
		if(!length()) return 0;
		if(!to.length()) return 0;
		//return acosf(normalized().dot(to) / (length() * to.length()));
		return -atan2f( cross(to), dot(to) );
	}
	float angle_deg(){
		return angle_deg(Vector2(1, 0));
	}
	float angle_deg(Vector2 to){
		return angle(to) * 180.0f / 3.14;	// TODO: some error with M_PI, solve later, but for now use
	}

	Vector2 normalized(){
		// handle division by zero exception
		if(!length()) return Vector2();
		return Vector2(x,y) / length();
	}

	float dot(Vector2 vec){
		return x * vec.x + y * vec.y;
	}
	float cross(Vector2 vec){
		return x * vec.y - y * vec.x;
	}

	Vector2 operator+(float scalar){
		return Vector2(x + scalar, y + scalar);
	}
	Vector2 operator-(float scalar){
		return Vector2(x - scalar, y - scalar);
	}
	Vector2 operator*(float scalar){
		return Vector2(x * scalar, y * scalar);
	}
	Vector2 operator/(float scalar){
		return Vector2(x / scalar, y / scalar);
	}

	Vector2 operator+(Vector2 vector){
		return Vector2(x + vector.x, y + vector.y);
	}
	Vector2 operator-(Vector2 vector){
		return Vector2(x - vector.x, y - vector.y);
	}
	Vector2 operator*(Vector2 vector){
		return Vector2(x * vector.x, y * vector.y);
	}
	Vector2 operator/(Vector2 vector){
		return Vector2(x / vector.x, y / vector.y);
	}
} Vector2;

typedef struct Matrix3x3{

	float m11;
	float m12;
	float m13;
	float m21;
	float m22;
	float m23;
	float m31;
	float m32;
	float m33;

	

} Matrix3x3;

typedef struct Transform2D{

	Matrix3x3 matrix;

	void translate(float x, float y){

	}

} Transform2D;

#endif // !__VECTOR_H__

