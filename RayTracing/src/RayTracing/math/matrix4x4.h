#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include "../cuda_includes.h"

//template <class T> class vec2;
//template <class T> class vec3;
//template <class T> class vec4;

////////////////////////////////////////////////////////////////////////////////
//
//  Matrix
//
////////////////////////////////////////////////////////////////////////////////
template<class T>
class matrix4 {

public:

	__device__ __host__ matrix4() {
		make_identity();
	}

	__device__ __host__ matrix4(T t) {
		set_value(t);
	}

	__device__ __host__ matrix4(const T* m) {
		set_value(m);
	}

	__device__ __host__ matrix4(
		T a00, T a01, T a02, T a03,
		T a10, T a11, T a12, T a13,
		T a20, T a21, T a22, T a23,
		T a30, T a31, T a32, T a33
	) :
		_11(a00), _12(a01), _13(a02), _14(a03),
		_21(a10), _22(a11), _23(a12), _24(a13),
		_31(a20), _32(a21), _33(a22), _34(a23),
		_41(a30), _42(a31), _43(a32), _44(a33) {
	}

	__device__ __host__ void get_value(T* mp) const {
		int c = 0;
		for(int j = 0; j < 4; j++) {
			for(int i = 0; i < 4; i++) {
				mp[c++] = element(i, j);
			}
		}
	}

	__device__ __host__ const T* get_value() const {
		return _array;
	}

	__device__ __host__ void set_value(T* mp) {
		int c = 0;
		for(int j = 0; j < 4; j++) {
			for(int i = 0; i < 4; i++) {
				element(i, j) = mp[c++];
			}
		}
	}

	__device__ __host__ void set_value(T r) {
		for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 4; j++) {
				element(i, j) = r;
			}
		}
	}

	__device__ __host__ void make_identity() {
		element(0, 0) = 1.0f;
		element(0, 1) = 0.0f;
		element(0, 2) = 0.0f;
		element(0, 3) = 0.0f;

		element(1, 0) = 0.0f;
		element(1, 1) = 1.0f;
		element(1, 2) = 0.0f;
		element(1, 3) = 0.0f;

		element(2, 0) = 0.0f;
		element(2, 1) = 0.0f;
		element(2, 2) = 1.0f;
		element(2, 3) = 0.0f;

		element(3, 0) = 0.0f;
		element(3, 1) = 0.0f;
		element(3, 2) = 0.0f;
		element(3, 3) = 1.0f;
	}

	// set a uniform scale
	__device__ __host__ void set_scale(T s) {
		element(0, 0) = s;
		element(1, 1) = s;
		element(2, 2) = s;
	}

	__device__ __host__ vec4 get_row(int r) const {
		vec4 v;

		v = { element(r, 0), element(r, 1), element(r, 2), element(r, 3) };

		return v;
	}

	__device__ __host__ vec4 get_column(int c) const {
		vec4 v;

		v = { element(0, c), element(1, c), element(2, c), element(3, c) };

		return v;
	}

	__device__ __host__ friend matrix4 inverse(const matrix4& m) {
		matrix4 minv;

		T r1[8], r2[8], r3[8], r4[8];
		T* s[4], * tmprow;

		s[0] = &r1[0];
		s[1] = &r2[0];
		s[2] = &r3[0];
		s[3] = &r4[0];

		int i, j, p, jj;

		for(i = 0; i < 4; i++) {
			for(j = 0; j < 4; j++) {
				s[i][j] = m.element(i, j);

				if(i == j) {
					s[i][j + 4] = 1.0;
				}
				else {
					s[i][j + 4] = 0.0;
				}
			}
		}

		T scp[4];

		for(i = 0; i < 4; i++) {
			scp[i] = T(fabs(s[i][0]));

			for(j = 1; j < 4; j++)
				if(T(fabs(s[i][j])) > scp[i]) {
					scp[i] = T(fabs(s[i][j]));
				}

			if(scp[i] == 0.0) {
				return minv;    // singular matrix!
			}
		}

		int pivot_to;
		T scp_max;

		for(i = 0; i < 4; i++) {
			// select pivot row
			pivot_to = i;
			scp_max = T(fabs(s[i][i] / scp[i]));

			// find out which row should be on top
			for(p = i + 1; p < 4; p++)
				if(T(fabs(s[p][i] / scp[p])) > scp_max) {
					scp_max = T(fabs(s[p][i] / scp[p]));
					pivot_to = p;
				}

			// Pivot if necessary
			if(pivot_to != i) {
				tmprow = s[i];
				s[i] = s[pivot_to];
				s[pivot_to] = tmprow;
				T tmpscp;
				tmpscp = scp[i];
				scp[i] = scp[pivot_to];
				scp[pivot_to] = tmpscp;
			}

			T mji;

			// perform gaussian elimination
			for(j = i + 1; j < 4; j++) {
				mji = s[j][i] / s[i][i];
				s[j][i] = 0.0;

				for(jj = i + 1; jj < 8; jj++) {
					s[j][jj] -= mji * s[i][jj];
				}
			}
		}

		if(s[3][3] == 0.0) {
			return minv;    // singular matrix!
		}

		//
		// Now we have an upper triangular matrix.
		//
		//  x x x x | y y y y
		//  0 x x x | y y y y
		//  0 0 x x | y y y y
		//  0 0 0 x | y y y y
		//
		//  we'll back substitute to get the inverse
		//
		//  1 0 0 0 | z z z z
		//  0 1 0 0 | z z z z
		//  0 0 1 0 | z z z z
		//  0 0 0 1 | z z z z
		//

		T mij;

		for(i = 3; i > 0; i--) {
			for(j = i - 1; j > -1; j--) {
				mij = s[j][i] / s[i][i];

				for(jj = j + 1; jj < 8; jj++) {
					s[j][jj] -= mij * s[i][jj];
				}
			}
		}

		for(i = 0; i < 4; i++) {
			for(j = 0; j < 4; j++) {
				minv(i, j) = s[i][j + 4] / s[i][i];
			}
		}

		return minv;
	}


	__device__ __host__ friend matrix4 transpose(const matrix4& m) {
		matrix4 mtrans;

		for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 4; j++) {
				mtrans(i, j) = m.element(j, i);
			}
		}

		return mtrans;
	}

	__device__ __host__ matrix4& operator *= (const matrix4& rhs) {
		matrix4 mt(*this);
		set_value(T(0));

		for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 4; j++)
				for(int c = 0; c < 4; c++) {
					element(i, j) += mt(i, c) * rhs(c, j);
				}
		}

		return *this;
	}

	__device__ __host__ friend matrix4 operator * (const matrix4& lhs, const matrix4& rhs) {
		matrix4 r(T(0));

		for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 4; j++)
				for(int c = 0; c < 4; c++) {
					r.element(i, j) += lhs(i, c) * rhs(c, j);
				}
		}

		return r;
	}

	// dst = M * src (4 x 4 * 4 x 1)
	__device__ __host__ vec4 operator *(const vec4& src) const {
		vec4 r;

		r.x = (src.x * element(0, 0) + src.y * element(0, 1) +
			src.z * element(0, 2) + src.w * element(0, 3));

		r.y = (src.x * element(1, 0) + src.y * element(1, 1) +
			src.z * element(1, 2) + src.w * element(1, 3));

		r.z = (src.x * element(2, 0) + src.y * element(2, 1) +
			src.z * element(2, 2) + src.w * element(2, 3));

		r.w = (src.x * element(3, 0) + src.y * element(3, 1) +
			src.z * element(3, 2) + src.w * element(3, 3));

		return r;
	}

	// dst = src * M (4 x 1 * 4 x 4)
	__device__ __host__ friend vec4 operator *(const vec4& lhs, const matrix4& rhs) {
		vec4 r;

		r.x = (lhs.x * rhs.element(0, 0) + lhs.y * rhs.element(1, 0) +
			lhs.z * rhs.element(2, 0) + lhs.w * rhs.element(3, 0));

		r.y = (lhs.x * rhs.element(0, 1) + lhs.y * rhs.element(1, 1) +
			lhs.z * rhs.element(2, 1) + lhs.w * rhs.element(3, 1));

		r.z = (lhs.x * rhs.element(0, 2) + lhs.y * rhs.element(1, 2) +
			lhs.z * rhs.element(2, 2) + lhs.w * rhs.element(3, 2));

		r.w = (lhs.x * rhs.element(0, 3) + lhs.y * rhs.element(1, 3) +
			lhs.z * rhs.element(2, 3) + lhs.w * rhs.element(3, 3));

		return r;
	}

	__device__ __host__ T& operator()(int row, int col) {
		return element(row, col);
	}

	__device__ __host__ const T& operator()(int row, int col) const {
		return element(row, col);
	}

	__device__ __host__ T& element(int row, int col) {
		return _array[row | (col << 2)];
	}

	__device__ __host__ const T& element(int row, int col) const {
		return _array[row | (col << 2)];
	}

	__device__ __host__ matrix4& operator *= (const T& r) {
		for(int i = 0; i < 4; ++i) {
			element(0, i) *= r;
			element(1, i) *= r;
			element(2, i) *= r;
			element(3, i) *= r;
		}

		return *this;
	}

	__device__ __host__ matrix4& operator += (const matrix4& mat) {
		for(int i = 0; i < 4; ++i) {
			element(0, i) += mat.element(0, i);
			element(1, i) += mat.element(1, i);
			element(2, i) += mat.element(2, i);
			element(3, i) += mat.element(3, i);
		}

		return *this;
	}


	__device__ __host__ friend bool operator == (const matrix4& lhs, const matrix4& rhs) {
		bool r = true;

		for(int i = 0; i < 16; i++) {
			r &= lhs._array[i] == rhs._array[i];
		}

		return r;
	}

	__device__ __host__ friend bool operator != (const matrix4& lhs, const matrix4& rhs) {
		bool r = true;

		for(int i = 0; i < 16; i++) {
			r &= lhs._array[i] != rhs._array[i];
		}

		return r;
	}

	union {
		struct {
			T _11, _12, _13, _14;   // standard names for components
			T _21, _22, _23, _24;   // standard names for components
			T _31, _32, _33, _34;   // standard names for components
			T _41, _42, _43, _44;   // standard names for components
		};
		T _array[16];     // array access
	};
};

using matrix4f = matrix4<float>;

#endif // !MATRIX4X4_H
