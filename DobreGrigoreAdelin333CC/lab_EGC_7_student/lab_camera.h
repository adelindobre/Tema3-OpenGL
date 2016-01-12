//-------------------------------------------------------------------------------------------------
// Descriere: header camera
//
// Autor: Lucian Petrescu
// Data: 14 oct 2013
//-------------------------------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <math.h>

#pragma once
#include "dependente\glm\glm.hpp"
#include "dependente\glm\gtc\type_ptr.hpp"
#include "dependente\glm\gtc\matrix_transform.hpp"


// Rotates a Point/Vector around the world OY (0, 1, 0) with a specific angle(radians)
// This just implements the rotation matrix around the world UP vector that we learned in the previous lab
// For rotating a vector/point around an axis use glm::rotate()
inline glm::vec3 RotateOY(const glm::vec3 Point, float radians)
{
	glm::vec3 R;
	R.x = Point.x * cos(radians) - Point.z * sin(radians);
	R.y = Point.y;
	R.z = Point.x * sin(radians) + Point.z * cos(radians);
	return R;
}

inline glm::vec3 RotateOX(const glm::vec3 Point, float radians)
{
	glm::vec3 R;
	R.x = Point.x;
	R.y = Point.y * cos(radians) - Point.z * sin(radians);
	R.z = Point.y * sin(radians) + Point.z * cos(radians);
	return R;
}

//**************************************************************
//**************************************************************
//		Comments are there for a reason - READ THEM !!!!!
//		Comments are there for a reason - READ THEM !!!!!
//		Comments are there for a reason - READ THEM !!!!!
//		Comments are there for a reason - READ THEM !!!!!
//		Comments are there for a reason - READ THEM !!!!!
//						
//										  before asking stupid questions
//
//		P.S  Questions are welcomed!
//			 Just make sure the explanation isn't already given 
//			 in comments or the Lab docummentation
//
//**************************************************************
//**************************************************************

namespace lab
{
	class Camera
	{
	public:
		Camera()
		{
			position = glm::vec3(0, 0, 50);
			forward = glm::vec3(0, 0, -1);
			up = glm::vec3(0, 1, 0);
			right = glm::vec3(1, 0, 0);
			distanceToTarget = 140;
		}

		Camera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
		{
			set(position, center, up);
		}

		~Camera()
		{ }

		// Update camera
		void set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
		{
			this->position = position;
			forward = glm::normalize(center - position);
			right = glm::cross(forward, up);
			this->up = glm::cross(right, forward);
		}

		void moveForwardKeepOY(float distance)
		{
			glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
			// [OPTIONAL] - movement will keep the camera at the same height always
			// Example: If you rotate up/down your head and walk forward you will stil keep the same relative distance (height) to the ground!
			// Translate the camera using the DIR vector computed from forward
		}

		void translateForward(float distance)
		{
			position = position + forward * distance;
			// Translate the camera using the forward vector
		}

		void translateUpword(float distance)
		{
			position = position + up *distance;
			// Translate the camera using the up vector
		}

		void translateRight(float distance)
		{
			glm::vec3 dir = glm::normalize(glm::vec3(right.x, 0, right.z));
			// Translate the camera using the dir or right vector
			// Usually translation using camera right is not very useful because if the camera is rotated around the forward vector 
			// translation over the right direction will have an undisered efect; the camera will get closer or farther from the ground
			// Using the projected right vector (onto the ground plane) makes more sense because we will keep the same distance from the ground plane
			// TODO
			position = position + dir * distance;
		}

		// Rotate the camera in FPS mode over the local OX axis
		void rotateFPS_OX(float angle)
		{
			// Compute the new forward and Up vectors
			// Atention! Don't forget to normalize the vectors
			// Use glm::rotate()
			forward = RotateOX(forward, angle);
			up = glm::cross(right, forward);
		}

		void rotateFPS_OY(float angle)
		{
			// Compute the new Forward and Up and Right vectors
			// Atention! Don't forget to normalize the vectors
			// Use glm::rotate() or the provided RotateOY function
			forward = RotateOY(forward, angle);
			right = RotateOY(right, angle);
			up = glm::cross(right, forward);
		}

		void rotateFPS_OZ(float angle)
		{
			// TODO
			// Compute the new Right and Up, Forward stays the same
			// Atention! Don't forget to normalize the vectors
			// use glm::rotate()
		}

		void rotateTPS_OX(float angle)
		{
			// TODO
			// Rotate the camera in Third Person mode - OX axis
			// Use distanceToTarget as translation distance
		}

		void rotateTPS_OY(float angle)
		{
			// Rotate the camera in Third Person mode - OY axis
			translateForward(200);
			rotateFPS_OY(angle);
			translateForward(-200);
		}

		void rotateTPS_OZ(float angle)
		{
			// TODO
			// Rotate the camera in Third Person mode - OY axis
		}

		glm::mat4 getViewMatrix()
		{
			// Return the View Matrix
			return glm::lookAt(position, position + forward, up);
		}

		glm::vec3 getTargetPosition()
		{
			return position + forward * distanceToTarget;
		}

		glm::vec3 getPosition()
		{
			return position;
		}

		glm::vec3 getForward()
		{
			return forward;
		}

	private:
		float distanceToTarget;
		glm::vec3 position;
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;
	};
}