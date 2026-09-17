#pragma once

#include <cmath>

// Engine-convention angle math, ported from the IW3 com_math functions of the same names (KisakCOD
// universal/com_math.cpp). Angles are [pitch, yaw, roll] in degrees with positive pitch looking down;
// an axis is the rows [forward, left, up]. Shared by the converters so every target agrees on the
// convention - a textbook quaternion-to-Euler conversion gives [roll, pitch, yaw] with the opposite
// pitch sign, which is how converted dynents once came out tipped over.
namespace ZoneTool::math
{
	inline constexpr float deg_to_rad = 0.017453292f;
	inline constexpr float rad_to_deg = 57.295776f;

	// vectoangles: pitch and yaw in [0, 360), roll 0
	inline void VectorToAngles(const float* vec, float* angles)
	{
		const float x = vec[0];
		const float y = vec[1];
		const float z = vec[2];

		float pitch;
		float yaw;

		if (x == 0.0f && y == 0.0f)
		{
			yaw = 0.0f;
			pitch = (z > 0.0f) ? 270.0f : 90.0f;
		}
		else
		{
			yaw = std::atan2(y, x) * rad_to_deg;
			if (yaw < 0.0f)
			{
				yaw += 360.0f;
			}

			pitch = std::atan2(z, std::sqrt(x * x + y * y)) * -rad_to_deg;
			if (pitch < 0.0f)
			{
				pitch += 360.0f;
			}
		}

		angles[0] = pitch;
		angles[1] = yaw;
		angles[2] = 0.0f;
	}

	// vectosignedpitch: pitch in [-90, 90]
	inline float VectorToSignedPitch(const float* vec)
	{
		if (vec[0] == 0.0f && vec[1] == 0.0f)
		{
			return (vec[2] > 0.0f) ? -90.0f : 90.0f;
		}

		return std::atan2(vec[2], std::sqrt(vec[0] * vec[0] + vec[1] * vec[1])) * -rad_to_deg;
	}

	inline void AxisToAngles(const float axis[3][3], float angles[3])
	{
		VectorToAngles(axis[0], angles);

		// undo yaw, then pitch, on the left vector; what is left of it is the roll
		const float yaw = -angles[1] * deg_to_rad;
		const float sin_yaw = std::sin(yaw);
		const float cos_yaw = std::cos(yaw);

		float left[3] = { axis[1][0], axis[1][1], axis[1][2] };
		const float temp = cos_yaw * left[0] - sin_yaw * left[1];
		left[1] = sin_yaw * left[0] + cos_yaw * left[1];

		const float pitch = -angles[0] * deg_to_rad;
		const float sin_pitch = std::sin(pitch);
		const float cos_pitch = std::cos(pitch);
		left[0] = sin_pitch * left[2] + cos_pitch * temp;
		left[2] = cos_pitch * left[2] - sin_pitch * temp;

		const float roll = VectorToSignedPitch(left);
		if (left[1] >= 0.0f)
		{
			angles[2] = -roll;
		}
		else
		{
			angles[2] = roll + ((roll >= 0.0f) ? -180.0f : 180.0f);
		}
	}

	inline void AngleVectors(const float* angles, float* forward, float* right, float* up)
	{
		const float pitch = angles[0] * deg_to_rad;
		const float yaw = angles[1] * deg_to_rad;
		const float roll = angles[2] * deg_to_rad;

		const float sp = std::sin(pitch);
		const float cp = std::cos(pitch);
		const float sy = std::sin(yaw);
		const float cy = std::cos(yaw);

		if (forward)
		{
			forward[0] = cp * cy;
			forward[1] = cp * sy;
			forward[2] = -sp;
		}

		if (right || up)
		{
			const float sr = std::sin(roll);
			const float cr = std::cos(roll);

			if (right)
			{
				right[0] = cr * sy - sr * sp * cy;
				right[1] = -cr * cy - sr * sp * sy;
				right[2] = -sr * cp;
			}

			if (up)
			{
				up[0] = cr * sp * cy + sr * sy;
				up[1] = cr * sp * sy - sr * cy;
				up[2] = cr * cp;
			}
		}
	}

	// AnglesToAxis: rows forward, left (= -right), up
	inline void AnglesToAxis(const float* angles, float axis[3][3])
	{
		float right[3];
		AngleVectors(angles, axis[0], right, axis[2]);
		axis[1][0] = -right[0];
		axis[1][1] = -right[1];
		axis[1][2] = -right[2];
	}

	// UnitQuatToAxis: quat is x, y, z, w
	inline void UnitQuatToAxis(const float* quat, float axis[3][3])
	{
		const float x = quat[0];
		const float y = quat[1];
		const float z = quat[2];
		const float w = quat[3];

		axis[0][0] = 1.0f - 2.0f * (y * y + z * z);
		axis[0][1] = 2.0f * (x * y + z * w);
		axis[0][2] = 2.0f * (x * z - y * w);
		axis[1][0] = 2.0f * (x * y - z * w);
		axis[1][1] = 1.0f - 2.0f * (x * x + z * z);
		axis[1][2] = 2.0f * (y * z + x * w);
		axis[2][0] = 2.0f * (x * z + y * w);
		axis[2][1] = 2.0f * (y * z - x * w);
		axis[2][2] = 1.0f - 2.0f * (x * x + y * y);
	}

	inline void UnitQuatToAngles(const float* quat, float angles[3])
	{
		float axis[3][3];
		UnitQuatToAxis(quat, axis);
		AxisToAngles(axis, angles);
	}
}
