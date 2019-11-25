#pragma once

#include "Vector.h"

namespace math {
	class Matrix4f {
	public:

		float* Data() {
			return data_;
		}

		const float* Data() const {
			return data_;
		}

		static Matrix4f Zero() {
			static Matrix4f zeroMatrix;
			for (size_t i = 0; i < 16; ++i) {
				zeroMatrix.data_[i] = 0.f;
			}
			return zeroMatrix;
		}

		static Matrix4f Identity() {
			static Matrix4f eyeMatrix;
			for (size_t i = 0; i < 16; ++i) {
				if (i == 0 || i == 5 || i == 10 || i == 15) {
					eyeMatrix.data_[i] = 1.f;
				}
				else {
					eyeMatrix.data_[i] = 0.f;
				}
			}
			return eyeMatrix;
		}

		void TranslateSelf(Vec3f t) {
			Matrix4f& self = *this;
			for (size_t i = 0; i < 3; ++i) {
				self(i, 3) += t[0] * self(i, 0) + t[1] * self(i, 1) + t[2] * self(i, 2);
			}
		}

		void ScaleSelf(Vec3f t) {
			Matrix4f& self = *this;
			for (size_t i = 0; i < 3; ++i) {
				self(0, i) *= t[i];
				self(1, i) *= t[i];
				self(2, i) *= t[i];
			}
		}

		// 2D only. Not generalized.
		void RotateSelf(float theta) {
			Matrix4f& self = *this;
			float a = self(0, 0);
			float b = self(0, 1);
			float c = self(1, 0);
			float d = self(1, 1);
			float v_cos = cos(theta);
			float v_sin = sin(theta);
			self(0, 0) = a * v_cos - c * v_sin;
			self(0, 1) = b * v_cos - d * v_sin;
			self(1, 0) = a * v_sin + c * v_cos;
			self(1, 1) = b * v_sin + d * v_cos;
		}

		float& operator()(size_t m, size_t n) {
			return data_[4*m + n];
		}
		const float& operator()(size_t m, size_t n) const {
			return data_[4*m + n];
		}

	private:
		float data_[16] = {
				0,0,0,0,
				0,0,0,0,
				0,0,0,0
		};
	};

	using Mat4f = Matrix4f;

	// Right handed, with -1,1 clip space. Adapted from glm's orthographic projection function.
	static Matrix4f MyOrtho(float left, float right, float bottom, float top, float zNear, float zFar) {
		Matrix4f result = Matrix4f::Identity();
		result(0, 0) = 2.0f / (right - left);
		result(1, 1) = 2.0f / (top - bottom);
		result(2, 2) = -2.0f / (zFar - zNear);
		result(0, 3) = -(right + left) / (right - left);
		result(1, 3) = -(top + bottom) / (top - bottom);
		result(2, 3) = -(zFar + zNear) / (zFar - zNear);
		return result;
	}
}