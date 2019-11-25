#pragma once

#include <array>
#include <cassert>
#include <string>
#include <sstream>

namespace math {

	// <size, datatype>
	template <size_t N, typename T>
	class Vector {
	public:
		Vector() : data_{} {}	// Initializes to default values, usually 0.
		~Vector() = default;	// Simply deletes our array of data.

		Vector(const T& s) {
			for (size_t i = 0; i < N; ++i) {
				data_[i] = s;
			}
		}

		template <typename ...Args>
		Vector(Args... args) : data_{ static_cast<T>(args)... } {}

		template <typename T2>
		Vector(const Vector<N, T2>& v) {	// Allows conversion, like making a float vector with ints.
			for (size_t i = 0; i < N; ++i) {
				data_[i] = static_cast<T>(v[i]);
			}
		};

		//Vector(Vector<N, T>&& v) {
		//	data_ = std::move(v.data_);
		//}

		static Vector<N, T> Ones() {
			Vector<N, T> onesVec;
			for (size_t i = 0; i < N; ++i) {
				onesVec.data_[i] = static_cast<T>(1);
			}
			return onesVec;
		}

		static Vector<N, T> Zero() {
			static Vector<N, T> zeroVec;
			for (size_t i = 0; i < N; ++i) {
				zeroVec.data_[i] = static_cast<T>(0);
			}
			return zeroVec;
		}

		T dot(const Vector<N, T>& rhs) const {
			T out = (*this)[0] * rhs[0];
			for (size_t i = 1; i < N; ++i) {
				out += data_[i] * rhs[i];
			}
			return out;
		}

		T norm() const {
			return static_cast<T>(sqrt(static_cast<double>(dot(*this))));
		}

		T norm2() const {
			return dot(*this);
		}

		Vector<N, T> normed() const {
			return (*this) / norm();
		}

		template <size_t M = N>
		typename std::enable_if<M == 2, Vector<2, T>>::type normal() const {
			return Vector<2, T>(-data_[1], data_[0]);
		}

		template <size_t M = N>
		typename std::enable_if<M == 3, Vector<3, T>>::type cross(const Vector<3, T>& b) const {
			const Vector<3, T>& a = (*this);
			return Vector<3, T>(a[1] * b[2] - a[2] * b[1],
				a[2] * b[0] - a[0] * b[2],
				a[0] * b[1] - a[1] * b[0]);
		}

		const size_t size() const {
			return N;
		}

		T* data() {
			return data_.data();
		}

		Vector<N, T>& operator+=(const Vector<N, T>& rhs) {
			for (size_t i = 0; i < N; ++i) {
				data_[i] += rhs[i];
			}
			return *this;
		}

		Vector<N, T>& operator-=(const Vector<N, T>& rhs) {
			for (size_t i = 0; i < N; ++i) {
				data_[i] -= rhs[i];
			}
			return *this;
		}

		Vector<N, T> operator+(const Vector<N, T>& rhs) const {
			Vector<N, T> lhs(*this);
			lhs += rhs;
			return lhs;
		}

		Vector<N, T> operator-(const Vector<N, T>& rhs) const {
			Vector<N, T> lhs(*this);
			lhs -= rhs;
			return lhs;
		}

		Vector<N, T>& operator*=(const T& rhs) {
			for (size_t i = 0; i < N; ++i) {
				data_[i] *= rhs;
			}
			return *this;
		}

		Vector<N, T>& operator/=(const T& rhs) {
			for (size_t i = 0; i < N; ++i) {
				data_[i] /= rhs;
			}
			return *this;
		}

		Vector<N, T> operator*(const T& rhs) const {
			Vector<N, T> lhs(*(this));
			lhs *= rhs;
			return lhs;
		}

		friend Vector<N, T> operator*(const T& lhs, const Vector<N, T>& rhs) {
			return rhs * lhs;
		}

		Vector<N, T> operator/(const T& rhs) const {
			Vector<N, T> lhs(*(this));
			lhs /= rhs;
			return lhs;
		}

		Vector<N, T> operator-() const {
			return -1 * *(this);
		}

		bool operator==(const Vector<N, T>& rhs) const {
			for (size_t i = 0; i < N; ++i) {
				if (data_[i] != rhs[i]) {
					return false;
				}
			}
			return true;
		}

		bool operator!=(const Vector<N, T>& rhs) const {
			return !((*this) == rhs);
		}

		T& operator[](size_t n) {
			assert(n >= 0 && n < N);
			return data_[n];
		}
		const T& operator[](size_t n) const {
			assert(n >= 0 && n < N);
			return data_[n];
		}

		friend std::ostream& operator<<(std::ostream& os, const Vector<N, T>& v) {
			os << "(";
			for (size_t i = 0; i < N - 1; ++i) {
				os << v.data_[i] << ", ";
			}
			os << v.data_.back() << ")";
			return os;
		}

		std::string toString() const {
			std::stringstream ss;
			ss << *this;
			return ss.str();
		}

	private:
		std::array<T, N> data_;
	};

	using Vec2d = Vector<2, double>;
	using Vec2f = Vector<2, float>;
	using Vec2i = Vector<2, int>;
	using Vec2u = Vector<2, unsigned int>;

	using Vec3d = Vector<3, double>;
	using Vec3f = Vector<3, float>;
	using Vec3i = Vector<3, int>;
	using Vec3u = Vector<3, unsigned int>;

	using Vec4d = Vector<4, double>;
	using Vec4f = Vector<4, float>;
	using Vec4i = Vector<4, int>;
	using Vec4u = Vector<4, unsigned int>;

}
