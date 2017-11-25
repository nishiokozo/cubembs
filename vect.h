// version 20170527 Metaball
// version 20170707 ray3
typedef	float	MAT3[3][3];
typedef	float	MAT4[4][4];

void	vect44_rotateX( float* m, float th );
void	vect44_rotateY( float* m, float th );
void	vect44_rotateZ( float* m, float th );

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#define	pi	M_PI
class	ivect3
{
public:
	union
	{
		int	x;
		int	r;
		int	p1;
	};
	union
	{
		int	y;
		int	g;
		int	p2;
	};
	union
	{
		int	z;
		int	b;
		int	p3;
	};

};


typedef	struct
{
	float	x;	
	float	y;	
	float	z;	
	float	w;	
} vector;

class	vect2
{
public:

	union
	{
		float	x;	
		float	u;	
		float	s;	
		float	r;	
	};
	union
	{
		float	y;	
		float	v;	
		float	t;	
		float	g;	
	};

	vect2( const float ax, const float ay )
	{
		x = ax;
		y = ay;
	}
	vect2(){};

	vect2( float ax, float ay, float az )
	{
		x = ax;
		y = ay;
	}

	vect2& operator*=( const vect2& v ) 
	{
		x *= v.x;
		y *= v.y;
		
		return *this;
	}
	vect2& operator/=( const vect2& v ) 
	{
		x /= v.x;
		y /= v.y;
		
		return *this;
	}
	vect2& operator+=( const vect2& v ) 
	{
		x += v.x;
		y += v.y;
		
		return *this;
	}

	vect2& operator-=( const vect2& v ) 
	{
		x -= v.x;
		y -= v.y;
		
		return *this;
	}

	vect2 operator-() const
	{
		vect2	ret;
		ret.x = -x;
		ret.y = -y;

		return ret;
	}
	vect2 operator+() const
	{
		vect2	ret;
		ret.x = x;
		ret.y = y;

		return ret;
	}
	vect2 operator*( const float f ) const
	{
		vect2	ret;
		ret.x = x * f;
		ret.y = y * f;

		return ret;
	}

	vect2 operator/( const float f ) const
	{
		float a = 1.0f / f;
		vect2	ret;
		ret.x = x * a;
		ret.y = y * a;

		return ret;
	}

	vect2 operator*( const vect2& v ) const
	{
		vect2	ret;
		ret.x = x * v.x;
		ret.y = y * v.y;

		return ret;
	}

	vect2 operator-( const vect2& v ) const
	{
		vect2	ret;
		ret.x = x - v.x;
		ret.y = y - v.y;

		return ret;
	}

	vect2 operator+( const vect2& v ) const
	{
		vect2	ret;
		ret.x = x + v.x;
		ret.y = y + v.y;

		return ret;
	}

	friend	vect2 operator*( const float f, const vect2& v ) 
	{
		vect2	ret;
		ret.x = f * v.x;
		ret.y = f * v.y;
		
		return ret;
	}

	friend	vect2 operator+( const float f, const vect2& v ) 
	{
		vect2	ret;
		ret.x = f + v.x;
		ret.y = f + v.y;
		
		return ret;
	}


	void print()
	{
		printf("(vect2)%9.6f %9.6f\n", x, y );
	}

	void print( const char* str )
	{
		printf("%s%9.6f %9.6f", str, x, y );
	}

};


class vect3 : public vect2
{
//    friend class vect4;

public:
/*
	union
	{
		float	x;	
		float	r;	
	};
	union
	{
		float	y;	
		float	g;	
	};
*/
	union	{	float	z,b; };
	
	vect3(){};
	vect3( float a )
	{
		x = a;
		y = a;
		z = a;
	};

	vect3( float _x, float _y, float _z )
	{
		x = _x;
		y = _y;
		z = _z;
	};
	vect3( const vect2& v, float _z )
	{
		x = v.x;
		y = v.y;
		z = _z;
	};

	vect3& operator*=( const vect3& v ) 
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		
		return *this;
	}

	vect3& operator/=( const vect3& v ) 
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		
		return *this;
	}
	vect3& operator+=( const vect3& v ) 
	{
		x += v.x;
		y += v.y;
		z += v.z;
		
		return *this;
	}

	vect3& operator-=( const vect3& v ) 
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		
		return *this;
	}

	vect3 operator-() const
	{
		vect3	ret;
		ret.x = -x;
		ret.y = -y;
		ret.z = -z;
		return ret;
	}
	vect3 operator+() const
	{
		vect3	ret;
		ret.x = x;
		ret.y = y;
		ret.z = z;
		return ret;
	}
	vect3 operator*( const float f ) const
	{
		vect3	ret;
		ret.x = x * f;
		ret.y = y * f;
		ret.z = z * f;
		return ret;
	}

	vect3 operator/( const float f ) const
	{
		float a = 1.0f / f;
		vect3	ret;
		ret.x = x * a;
		ret.y = y * a;
		ret.z = z * a;
		return ret;
	}

	vect3 operator*( const vect3& v ) const
	{
		vect3	ret;
		ret.x = x * v.x;
		ret.y = y * v.y;
		ret.z = z * v.z;
		return ret;
	}

	vect3 operator-( const vect3& v ) const
	{
		vect3	ret;
		ret.x = x - v.x;
		ret.y = y - v.y;
		ret.z = z - v.z;
		return ret;
	}

	vect3 operator+( const vect3& v ) const
	{
		vect3	ret;
		ret.x = x + v.x;
		ret.y = y + v.y;
		ret.z = z + v.z;
		return ret;
	}

	friend	vect3 operator*( const float f, const vect3& v ) 
	{
		vect3	ret;
		ret.x = f * v.x;
		ret.y = f * v.y;
		ret.z = f * v.z;
		
		return ret;
	}

	friend	vect3 operator+( const float f, const vect3& v ) 
	{
		vect3	ret;
		ret.x = f + v.x;
		ret.y = f + v.y;
		ret.z = f + v.z;
		
		return ret;
	}


	void print()
	{
		printf("(vect3)%9.6f %9.6f %9.6f\n", x, y, z );
	}

	void print( const char* str )
	{
		printf("%s<%9.6f %9.6f %9.6f>\n", str, x, y, z );
	}

};


#if 1
class vect4 : public vect3
{
public:
	union{		float	w,a;		};
#else
class vect4
{
public:
	union{		float	x,r;		};
	union{		float	y,g;		};
	union{		float	z,b;		};
	union{		float	w,a;		};
#endif

	vect4(){};
	vect4( float f )
	{
		x = f;
		y = f;
		z = f;
		w = f;
	};

	vect4( float ax, float ay, float az, float aw )
	{
		x = ax;
		y = ay;
		z = az;
		w = aw;
	};
	vect4( const vect3& v, float aw )
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = aw;
	};

	vect4 operator+( const vect4& v )	const//	V + V
	{
		vect4	ret;
		ret.x = x + v.x;
		ret.y = y + v.y;
		ret.z = z + v.z;
		ret.w = w + v.w;
		return ret;
	}
	vect4 operator-( const vect4& v )	const //	V - V
	{
		vect4	ret;
		ret.x = x - v.x;
		ret.y = y - v.y;
		ret.z = z - v.z;
		ret.w = w - v.w;
		return ret;
	}
	vect4 operator*( const vect4& v )	const//	V * V
	{
		vect4	ret;
		ret.x = x * v.x;
		ret.y = y * v.y;
		ret.z = z * v.z;
		ret.w = w * v.w;
		return ret;
	}
	vect4 operator/( const vect4& v )	const//	V / V
	{
		vect4	ret;
		ret.x = x / v.x;
		ret.y = y / v.y;
		ret.z = z / v.z;
		ret.w = w / v.w;
		return ret;
	}

	friend	vect4 operator*( const float f, const vect4& v )	//	f * V
	{
		vect4	ret;
		ret.x = f * v.x;
		ret.y = f * v.y;
		ret.z = f * v.z;
		ret.w = f * v.w;
		
		return ret;
	}
/*

	vect4 operator*( const vect4& v ) const
	{
		vect4	ret;
		ret.x = x * v.x;
		ret.y = y * v.y;
		ret.z = z * v.z;
		ret.w = w * v.w;
		return ret;
	}
	vect4 operator=( const vect4& v )
	{
		x =  v.x;
		y =  v.y;
		z =  v.z;
		w =  v.w;
	}
	vect4& operator+=( const vect4& v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		
		return *this;
	}
	vect4 operator*( const float f ) const
	{
		vect4	ret;
		ret.x = x * f;
		ret.y = y * f;
		ret.z = z * f;
		ret.w = w * f;
		return ret;
	}
	vect4 operator/( const float f ) const
	{
		vect4	ret;
		ret.x = x / f;
		ret.y = y / f;
		ret.z = z / f;
		ret.w = w / f;
		return ret;
	}


	friend	vect4 operator+( const float f, const vect4& v )
	{
		vect4	ret;
		ret.x = f + v.x;
		ret.y = f + v.y;
		ret.z = f + v.z;
		ret.w = f + v.w;
		
		return ret;
	}

	friend	vect4 operator-( const float f, const vect4& v )
	{
		vect4	ret;
		ret.x = f - v.x;
		ret.y = f - v.y;
		ret.z = f - v.z;
		ret.w = f - v.w;
		
		return ret;
	}

	friend	vect4 operator/( const float f, const vect4& v )
	{
		vect4	ret;
		ret.x = f / v.x;
		ret.y = f / v.y;
		ret.z = f / v.z;
		ret.w = f / v.w;
		
		return ret;
	}
*/


	void print()
	{
		printf("(vect4)%9.6f %9.6f %9.6f %9.6f\n", x, y, z, w );
	}

	void print( const char* str )
	{
		printf("%s<%9.6f %9.6f %9.6f %9.6f>\n", str, x, y, z, w );
	}

};

float	vec3_dotproduct( float a[3], float b[3] );

void	vec3_crossproduct( float v[3], float a[3], float b[3] );
vect3	       cross( const vect3& a, const vect3& b );

void	vec3_normalize( float v[3] );
void	vec3_normalize( vect3* v );
float	vec3_length( float v[3] );
float	vec3_length( vect3& v );

void	vec4_multiplyMatrix( float v[4], float m[16], float a[4] );

float	dot( const vect3& a, const vect3& b );

//float	dot( vect4& a, vect3& b );
//float	dot( vect3& a, vect4& b );
//float	dot( vect4& a, vect4& b );

vect3 refract( const vect3& a, const vect3& b, float n );
vect3 reflect( const vect3& I, const vect3& N );

//float	abs( float );
float	max( float a, float b );
vect3	max( float a, const vect3& b );
vect3	max( const vect3& b, float a );
float	min( float a, float b );
vect3	min( float a, const vect3& b );
vect3	min( const vect3& b, float a );

vect3	mix( float f, const vect3& a, const vect3& b );
float	mod( float a, float b );
float	length( const vect2& a );
float	length( const vect3& a );
float	length( const vect3& a, const vect3& b );
vect2	normalizXe( const vect2& a );
vect3	normalizXe( const vect3& a );
vect3	normalize( const vect3& a );


#define	RAD(n)	((n)*M_PI/180.0f)
#define	DEG(n)	((n)*180.0f/M_PI)
#define	M00	0
#define	M01	1
#define	M02	2
#define	M03	3
#define	M10	4
#define	M11	5
#define	M12	6
#define	M13	7
#define	M20	8
#define	M21	9
#define	M22	10
#define	M23	11
#define	M30	12
#define	M31	13
#define	M32	14
#define	M33	15

#define	rad(n)((n)*M_PI/180.0f)

//void	vect44_multiply( float* m, const float* a, const float* b );
void	vect44_multiply( float* m, const float* a, const float* y1 );

class	mat3
{
public:
	union
	{
		float	m_array[9];
		MAT3	m;
	};
	mat3()
	{
		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;
	}

	mat3(
		float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22
	)
	{
		m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;
		m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;
		m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;
	}

	friend	vect3 operator*( const vect3& a, const mat3& m )
	{
		vect3 ret;
		ret.x = a.x*m.m[0][0] + a.y*m.m[1][0] + a.z*m.m[2][0];
		ret.y = a.x*m.m[0][1] + a.y*m.m[1][1] + a.z*m.m[2][1];
		ret.z = a.x*m.m[0][2] + a.y*m.m[1][2] + a.z*m.m[2][2];

		return ret;
	}

/*	friend	vect3& operator*=( vect3& v, mat3 m )
	{
		v = v * m;
	}
*/

	vect3 operator*( const vect3& a ) const
	{
		vect3 ret;
		ret.x = m[0][0]*a.x + m[0][1]*a.y + m[0][2]*a.z;
		ret.y = m[1][0]*a.x + m[1][1]*a.y + m[1][2]*a.z;
		ret.z = m[2][0]*a.x + m[2][1]*a.y + m[2][2]*a.z;
		return ret;
	}

	mat3 operator*( const mat3& m ) const
	{
		const MAT3& a = this->m;
		const MAT3& b = m.m;
		mat3 ret(
			a[0][0] * b[0][0] +  a[0][1] * b[1][0] +  a[0][2] * b[2][0],
			a[0][0] * b[0][1] +  a[0][1] * b[1][1] +  a[0][2] * b[2][1],
			a[0][0] * b[0][2] +  a[0][1] * b[1][2] +  a[0][2] * b[2][2],

			a[1][0] * b[0][0] +  a[1][1] * b[1][0] +  a[1][2] * b[2][0],
			a[1][0] * b[0][1] +  a[1][1] * b[1][1] +  a[1][2] * b[2][1],
			a[1][0] * b[0][2] +  a[1][1] * b[1][2] +  a[1][2] * b[2][2],

			a[2][0] * b[0][0] +  a[2][1] * b[1][0] +  a[2][2] * b[2][0],
			a[2][0] * b[0][1] +  a[2][1] * b[1][1] +  a[2][2] * b[2][1],
			a[2][0] * b[0][2] +  a[2][1] * b[1][2] +  a[2][2] * b[2][2]
		);
		return ret;
	}

	mat3& operator*=( const mat3& a ) 
	{
		*this = *this * a;
		
		return *this;
	}

	float* GetArray(){ return m_array; };
	float* getArray(){ return m_array; };
	float* toArray(){ return m_array; };

	void identity()
	{
		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;
	}
	


	void rotXYZ( vect3 rot )
	{
		this->rotX( rot.x );
		this->rotY( rot.y );
		this->rotZ( rot.z );
	}
	void rotX( float f)
	{
		float	c = cos(f);
		float	s = sin(f);
		*this *= mat3(
			1,  0,  0,
			0,  c, -s,
			0,  s,  c
		);
	}
	void rotY( float f)
	{
		float	c = cos(f);
		float	s = sin(f);
		*this *= mat3(
			c,  0, -s,
			0,  1,  0,
			s,  0,  c
		);
	}
	void rotZ( float f)
	{
		float	c = cos(f);
		float	s = sin(f);
		*this *= mat3(
			c, -s,  0,
			s,	c,  0,
			0,  0,  1
		);
	}
/*
	vect3 RotateX( float f )
	{
//		MAT3& m = this->m;
		float	c = cos(f);
		float	s = sin(f);

		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;	
		m[1][0] = 0.0f;	m[1][1] =   c;	m[1][2] =  -s;	
		m[2][0] = 0.0f;	m[2][1] =   s;	m[2][2] =   c;	
	}
	void rotY( float f )
	{
//		MAT3& m = this->m;
		float	c = cos(f);
		float	s = sin(f);

		m[0][0] =    c;	m[0][1] = 0.0f;	m[0][2] =  -s;	
		m[1][0] = 0.0f;	m[1][1] = 0.0f;	m[1][2] = 0.0f;	
		m[2][0] =    s;	m[2][1] = 0.0f;	m[2][2] =   c;	
	}
	void rotZ( float f )
	{
//		MAT3& m = this->m;
		float	c = cos(f);
		float	s = sin(f);

		m[0][0] =    c;	m[0][1] =   -s;	m[0][2] = 0.0f;	
		m[1][0] =    s;	m[1][1] =    c;	m[1][2] = 0.0f;	
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;	
	}
*/

	void scale( float x, float y, float z )
	{
		m[0][0] *= x;
		m[1][1] *= y;
		m[2][2] *= z;
	}
	void scale( vect3 scale )
	{
		m[0][0] *= scale.x;
		m[1][1] *= scale.y;
		m[2][2] *= scale.z;
	}

	void print()
	{
		printf("mat3:\n");
		printf("%9.6f %9.6f %9.6f \n", m[0][0], m[0][1], m[0][2] );
		printf("%9.6f %9.6f %9.6f \n", m[1][0], m[1][1], m[1][2] );
		printf("%9.6f %9.6f %9.6f \n", m[2][0], m[2][1], m[2][2] );
	}
	void print( const char* str )
	{
		printf("mat3:%s\n", str);
		printf("%9.6f %9.6f %9.6f \n", m[0][0], m[0][1], m[0][2] );
		printf("%9.6f %9.6f %9.6f \n", m[1][0], m[1][1], m[1][2] );
		printf("%9.6f %9.6f %9.6f \n", m[2][0], m[2][1], m[2][2] );
	}

};

//---
class	vect44
{
public:
	union
	{
		float	m_array[16];
		MAT4	m;
	};
	vect44()
	{
		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;	m[0][3] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0f;	m[1][3] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;	m[2][3] = 0.0f;
		m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
	}

	vect44(
		float m00, float m01, float m02, float m03, 
		float m10, float m11, float m12, float m13, 
		float m20, float m21, float m22, float m23, 
		float m30, float m31, float m32, float m33 
	)
	{
		m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;	m[0][3] = m03;
		m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;	m[1][3] = m13;
		m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;	m[2][3] = m23;
		m[3][0] = m30;	m[3][1] = m31;	m[3][2] = m32;	m[3][3] = m33;
	}

	vect4 operator*( const vect4& a ) const
	{
		vect4 ret;
		ret.x = m[0][0]*a.x + m[0][1]*a.y + m[0][2]*a.z + m[0][3]*a.w ;
		ret.y = m[1][0]*a.x + m[1][1]*a.y + m[1][2]*a.z + m[1][3]*a.w ;
		ret.z = m[2][0]*a.x + m[2][1]*a.y + m[2][2]*a.z + m[2][3]*a.w ;
		ret.w = m[3][0]*a.x + m[3][1]*a.y + m[3][2]*a.z + m[3][3]*a.w ;
		return ret;
	}

	friend	vect4 operator*( const vect4& a, const vect44& m )
	{
		vect4 ret;
		ret.x = a.x*m.m[0][0] + a.y*m.m[1][0] + a.z*m.m[2][0] + a.w*m.m[3][0] ;
		ret.y = a.x*m.m[0][1] + a.y*m.m[1][1] + a.z*m.m[2][1] + a.w*m.m[3][1] ;
		ret.z = a.x*m.m[0][2] + a.y*m.m[1][2] + a.z*m.m[2][2] + a.w*m.m[3][2] ;
		ret.w = a.x*m.m[0][3] + a.y*m.m[1][3] + a.z*m.m[2][3] + a.w*m.m[3][3] ;

		return ret;
	}

	vect44 operator*( const vect44& m ) const
	{
		const MAT4& a = this->m;
		const MAT4& b = m.m;

		vect44 ret(
			a[0][0] * b[0][0] +  a[0][1] * b[1][0] +  a[0][2] * b[2][0] +  a[0][3] * b[3][0],
			a[0][0] * b[0][1] +  a[0][1] * b[1][1] +  a[0][2] * b[2][1] +  a[0][3] * b[3][1],
			a[0][0] * b[0][2] +  a[0][1] * b[1][2] +  a[0][2] * b[2][2] +  a[0][3] * b[3][2],
			a[0][0] * b[0][3] +  a[0][1] * b[1][3] +  a[0][2] * b[2][3] +  a[0][3] * b[3][3],

			a[1][0] * b[0][0] +  a[1][1] * b[1][0] +  a[1][2] * b[2][0] +  a[1][3] * b[3][0],
			a[1][0] * b[0][1] +  a[1][1] * b[1][1] +  a[1][2] * b[2][1] +  a[1][3] * b[3][1],
			a[1][0] * b[0][2] +  a[1][1] * b[1][2] +  a[1][2] * b[2][2] +  a[1][3] * b[3][2],
			a[1][0] * b[0][3] +  a[1][1] * b[1][3] +  a[1][2] * b[2][3] +  a[1][3] * b[3][3],

			a[2][0] * b[0][0] +  a[2][1] * b[1][0] +  a[2][2] * b[2][0] +  a[2][3] * b[3][0],
			a[2][0] * b[0][1] +  a[2][1] * b[1][1] +  a[2][2] * b[2][1] +  a[2][3] * b[3][1],
			a[2][0] * b[0][2] +  a[2][1] * b[1][2] +  a[2][2] * b[2][2] +  a[2][3] * b[3][2],
			a[2][0] * b[0][3] +  a[2][1] * b[1][3] +  a[2][2] * b[2][3] +  a[2][3] * b[3][3],

			a[3][0] * b[0][0] +  a[3][1] * b[1][0] +  a[3][2] * b[2][0] +  a[3][3] * b[3][0],
			a[3][0] * b[0][1] +  a[3][1] * b[1][1] +  a[3][2] * b[2][1] +  a[3][3] * b[3][1],
			a[3][0] * b[0][2] +  a[3][1] * b[1][2] +  a[3][2] * b[2][2] +  a[3][3] * b[3][2],
			a[3][0] * b[0][3] +  a[3][1] * b[1][3] +  a[3][2] * b[2][3] +  a[3][3] * b[3][3]
		);

		return	ret;
	}
/*
	vect44& operator=( const vect44& a ) 
	{
		*this = a;
		
		return *this;
	}
*/
	vect44& operator*=( const vect44& a ) 
	{
//		vect44_multiply( m_array, m_array, a.m_array );
		*this = *this * a;
		
		return *this;
	}

	float* GetArray(){ return m_array; };
	float* getArray(){ return m_array; };
	float* toArray(){ return m_array; };

	void identity()
	{
		m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;	m[0][3] = 0.0f;
		m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0f;	m[1][3] = 0.0f;
		m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;	m[2][3] = 0.0f;
		m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
	}
	
	void SetTranslate( const vect3& pos )
	{
		m[3][0] = pos.x;
		m[3][1] = pos.y;
		m[3][2] = pos.z;
	}
	void SetTranslate( float x, float y, float z )
	{
		m[3][0] = x;
		m[3][1] = y;
		m[3][2] = z;
	}
/*
	void Invers( const vect44& m )
	{
		vect44_invers( *this, m );
	}
*/
/*
	vect3 GetVecX()
	{
		return vect3( m[0][0], m[0][1], m[0][2] );
	}
	vect3 GetVecY()
	{
		return vect3( m[1][0], m[1][1], m[1][2] );
	}
	vect3 GetVecZ()
	{
		return vect3( m[2][0], m[2][1], m[2][2] );
	}
	vect3 GetVecT()
	{
		return vect3( m[3][0], m[3][1], m[3][2] );
	}
	void setVecX( vect3 v )
	{
		m[0][0] = v.x; 
		m[0][1] = v.y; 
		m[0][2] = v.z;
	}
	void setVecY( vect3 v )
	{
		m[1][0] = v.x; 
		m[1][1] = v.y; 
		m[1][2] = v.z;
	}
	void setVecZ( vect3 v )
	{
		m[2][0] = v.x; 
		m[2][1] = v.y; 
		m[2][2] = v.z;
	}
	void setVecT( vect3 v )
	{
		m[3][0] = v.x; 
		m[3][1] = v.y; 
		m[3][2] = v.z;
	}
*/

	//-----------------------------------------------------------------------------
	void perspectiveGL( float fovy, float aspect, float n, float f)
	//-----------------------------------------------------------------------------
	{
		float p = 1.0f / tanf(fovy *(M_PI/360.0));

		*this =  vect44(
			p / aspect	, 0			, 0						,  0,
			0			, p			, 0						,  0,
			0			, 0			,(f+n)         /(n-f)	, -1,
			0			, 0			,(2.0 * f * n) /(n-f)	,  1
		);
	}
	//-----------------------------------------------------------------------------
	void perspective( float fovy, float aspect )
	//-----------------------------------------------------------------------------
	{
		float p = 1.0f / tanf(fovy *(M_PI/360.0));

		*this =  vect44(
			p / aspect	,		0	,		0	,	 0,
			0			,		p	,		0	,	 0,
			0			,		0	,		0	, 	 2,
			0			,		0	,		0	,	 0
		);
	}

	//-----------------------------------------------------------------------------
	void lookAt( vect3 pos, vect3 at, vect3 up )
	//-----------------------------------------------------------------------------
	{
		vect3	z = normalizXe(at-pos);
		vect3	x = cross( up, z );
		vect3	y = cross( z, x );
	
		m[0][0] =x.x;	m[0][1] =x.y;	m[0][2] =x.z;	m[0][3] = 0.0f;
		m[1][0] =y.x;	m[1][1] =y.y;	m[1][2] =y.z;	m[1][3] = 0.0f;
		m[2][0] =z.x;	m[2][1] =z.y;	m[2][2] =z.z;	m[2][3] = 0.0f;
		m[3][0] =pos.x;	m[3][1] =pos.y;	m[3][2] =pos.z;	m[3][3] = 1.0f;
	}
	//-----------------------------------------------------------------------------
	void lookFor( vect3 pos, vect3 z, vect3 up )
	//-----------------------------------------------------------------------------
	{
		vect3	x = cross( up, z );
		vect3	y = cross( z, x );
	
		m[0][0] =x.x;	m[0][1] =x.y;	m[0][2] =x.z;	m[0][3] = 0.0f;
		m[1][0] =y.x;	m[1][1] =y.y;	m[1][2] =y.z;	m[1][3] = 0.0f;
		m[2][0] =z.x;	m[2][1] =z.y;	m[2][2] =z.z;	m[2][3] = 0.0f;
		m[3][0] =pos.x;	m[3][1] =pos.y;	m[3][2] =pos.z;	m[3][3] = 1.0f;
	}
	//-----------------------------------------------------------------------------
	void rotXYZ( vect3 rot )
	//-----------------------------------------------------------------------------
	{
		this->rotX( rot.x );
		this->rotY( rot.y );
		this->rotZ( rot.z );
	}
	//-----------------------------------------------------------------------------
	vect44& rotX( float f)
	//-----------------------------------------------------------------------------
	{
		float	c = cos(f);
		float	s = sin(f);
		*this *= vect44(
			1,  0,  0,  0,
			0,  c, -s,  0,
			0,  s,  c,  0,
			0,  0,  0,  1
		);
		return (*this);
	}
	//-----------------------------------------------------------------------------
	vect44& rotY( float f)
	//-----------------------------------------------------------------------------
	{
		float	c = cos(f);
		float	s = sin(f);
		*this *= vect44(
			c,  0, -s,  0,
			0,  1,  0,  0,
			s,  0,  c,  0,
			0,  0,  0,  1
		);
		return (*this);
	}
	//-----------------------------------------------------------------------------
	vect44& rotZ( float f)
	//-----------------------------------------------------------------------------
	{
		float	c = cos(f);
		float	s = sin(f);
		*this *= vect44(
			c, -s,  0,  0,
			s,	c,  0,  0,
			0,  0,  1,  0,
			0,  0,  0,  1
		);
		return (*this);
	}
	//-----------------------------------------------------------------------------
	void setPos( float x, float y, float z )
	//-----------------------------------------------------------------------------
	{
		m[0][3] = x;
		m[1][3] = y;
		m[2][3] = z;
	}
	//-----------------------------------------------------------------------------
	void translate( vect3 vec )
	//-----------------------------------------------------------------------------
	{
		m[3][0] += vec.x;
		m[3][1] += vec.y;
		m[3][2] += vec.z;
	}
	//-----------------------------------------------------------------------------
	void translate( float x, float y, float z )
	//-----------------------------------------------------------------------------
	{
		m[3][0] += x;
		m[3][1] += y;
		m[3][2] += z;
	}

	//-----------------------------------------------------------------------------
	vect44& scale( float s )
	//-----------------------------------------------------------------------------
	{
		*this *= vect44(
			  s,  0,  0,  0,
			  0,  s,  0,  0,
			  0,  0,  s,  0,
			  0,  0,  0,  1
		);
		return (*this);
	}
	//-----------------------------------------------------------------------------
	vect44& scale( float x, float y, float z )
	//-----------------------------------------------------------------------------
	{
		*this *= vect44(
			  x,  0,  0,  0,
			  0,  y,  0,  0,
			  0,  0,  z,  0,
			  0,  0,  0,  1
		);
		return (*this);
	}
	//-----------------------------------------------------------------------------
	vect44& scale( float x, float y, float z, float w )
	//-----------------------------------------------------------------------------
	{
		*this *= vect44(
			  x,  0,  0,  0,
			  0,  y,  0,  0,
			  0,  0,  z,  0,
			  0,  0,  0,  w
		);
		return (*this);
	}
	//-----------------------------------------------------------------------------
	int	size()
	//-----------------------------------------------------------------------------
	{
		return sizeof(*this);
	}

/*	//-----------------------------------------------------------------------------
	void scale( float x, float y, float z )
	//-----------------------------------------------------------------------------
	{
		m[0][0] *= x;
		m[1][1] *= y;
		m[2][2] *= z;
	}
	//-----------------------------------------------------------------------------
	void scale( vect3 scale )
	//-----------------------------------------------------------------------------
	{
		m[0][0] *= scale.x;
		m[1][1] *= scale.y;
		m[2][2] *= scale.z;
	}
*/

	//-----------------------------------------------------------------------------
	void print()
	//-----------------------------------------------------------------------------
	{
		printf("vect44:\n");
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[0][0], m[0][1], m[0][2], m[0][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[1][0], m[1][1], m[1][2], m[1][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[2][0], m[2][1], m[2][2], m[2][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[3][0], m[3][1], m[3][2], m[3][3] );
	}
	//-----------------------------------------------------------------------------
	void print( const char* str )
	//-----------------------------------------------------------------------------
	{
		printf("vect44:%s\n", str);
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[0][0], m[0][1], m[0][2], m[0][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[1][0], m[1][1], m[1][2], m[1][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[2][0], m[2][1], m[2][2], m[2][3] );
		printf("%9.6f %9.6f %9.6f %9.6f\n", m[3][0], m[3][1], m[3][2], m[3][3] );
	}

};


void	vect44_Frustum( vect44& m, float l, float r, float b, float t, float n, float f );
void	vect44_Ortho( float* m, float l, float r, float b, float t, float n, float f );

int	vect44_print( vect44& m );
int	vect44_print( float* m );
void	vect44_frustum( float* m, float l, float r, float b, float t, float n, float f );
void	vect44_translate( vect44& m, float x, float y, float z );
void	vect44_translate( float* m, float x, float y, float z );
void	vect44_translate( float* m, vect3 vec );
void	vect44_translate( vect44& m, vect3 vec );
void	vect44_rotateX( vect44& m, float th );
void	vect44_rotateY( vect44& m, float th );
void	vect44_rotateZ( vect44& m, float th );
void	vect44_scaling( vect44& m, float sx, float sy, float sz  );
void	vect44_scaling( float* m, float sx, float sy, float sz  );
void	vect44_scaling( float* m, const vect3& vecScale  );
void	vect44_scaling( vect44& m, const vect3& vecScale  );
void	vect44_transpose( float* m, float a[16] );

void	vect44_copy( float* m, float a[16] );

void 	vect44_invers( vect44& a, const vect44& b );
//void	vect44_invers( float* out, float* in );
void	vect44_identity( float* m );
void	vect44_identity( vect44& m );

void	vect44_getTranslate( float* m, float x, float y, float z );
void	vect44_getRotateX( float* m, float th );
void	vect44_getRotateY( float* m, float th );
void	vect44_getRotateZ( float* m, float th );
void	vect44_getScale( float* m, float sx, float sy, float sz  );

vect4	mul( const vect4& a, const vect44& m );

float*	vect44_identity();

void 	vect44_glFrustumf( float* m,  float left, float right, float bottom, float top, float near, float far);
void 	vect44_perspective( vect44& m, float fovy, float aspect, float zNear, float zFar);
void 	vect44_perspective(float* m, float fovy, float aspect, float zNear, float zFar);
void 	vect44_invers( float* m, const float* in );
void	vect44_add( float* m, float* a, float* b );
void	vect44_sub( float* m, float* a, float* b );
void	vect44_div( float* m, float f );

// for raytrace
void vect44_ray_perspective(float* m, float fovy, float aspect );
void	vect44_otrho( float* m, float l, float r, float b, float t, float n, float f );

vect44	vect44_GetTranslate( float x, float y, float z );
vect44	vect44_GetRotateX( float th );
vect44	vect44_GetRotateY( float th );
vect44	vect44_GetRotateZ( float th );
