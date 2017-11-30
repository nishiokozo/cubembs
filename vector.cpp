#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>
#include	<string.h>
#include	"vector.h"
#include	"matrix.h"

//##########################################################
// vector
//##########################################################

vector::vector()
{
	x = 0;
	y = 0;
	z = 0;
	w = 1.0f;
}
vector::vector( float fx, float fy, float fz )
{	
	x = fx;
	y = fy;
	z = fz;
	w = 1.0f;
}
vector::vector( float fx, float fy, float fz, float fw )
{	
	x = fx;
	y = fy;
	z = fz;
	w = fw;
}
vector& vector::operator += ( vector v )
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}
vector& vector::operator -= ( vector v )
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

vector& vector::operator *= ( float f )
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

vector& vector::operator /= ( float f )
{
	float fInv = 1.0f / f;
	x *= fInv;
	y *= fInv;
	z *= fInv;
	return *this;
}

vector vector::operator + ()	
{
	return *this;
}


vector vector::operator - ()	
{
	return vector(-x, -y, -z );
}

vector vector::operator + ( vector v )	
{
	return vector(x + v.x, y + v.y, z + v.z);
}


vector vector::operator - ( vector v )	
{
	return vector(x - v.x, y - v.y, z - v.z);
}


vector vector::operator * ( float f )	
{
	return vector(x * f, y * f, z * f);
}

vector vector::operator / ( float f )	
{
	float fInv = 1.0f / f;
	return vector(x * fInv, y * fInv, z * fInv);
}



vector operator * ( float f,  class vector v )
{
	return vector(f * v.x, f * v.y, f * v.z);
}



int vector::operator == ( vector v )
{
	return x == v.x && y == v.y && z == v.z;
}


int vector::operator != ( vector v )
{
	return x != v.x || y != v.y || z != v.z;
}

vector& vector::operator = ( float f )
{
	x = f;
	y = f;
	z = f;
	return *this;
}
//----------------------------------------------------------
float vector::flen()
//----------------------------------------------------------
{
	return sqrtf( x * x + y * y + z * z );
}

//----------------------------------------------------------
vector& vector::operator = ( vector v )
//----------------------------------------------------------
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return *this;
}

//----------------------------------------------------------
float fscalar( vector v1 )
//----------------------------------------------------------
{
	static	float	ret;
	ret = sqrtf( v1.x * v1.x + v1.y * v1.y + v1.z * v1.z );

	return	ret;
}
//----------------------------------------------------------
float fdot( vector v1,  vector v2  )
//----------------------------------------------------------
{
	float	ret;
	ret = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return	ret;
}
//----------------------------------------------------------
float flen( vector v1  )
//----------------------------------------------------------
{
	return sqrtf( v1.x * v1.x + v1.y * v1.y + v1.z * v1.z );
}

//----------------------------------------------------------
vector vcross( vector v1,  vector v2  )
//----------------------------------------------------------
{
	vector	v0;

	v0.x = v1.y * v2.z - v2.y*v1.z;
	v0.y = v1.z * v2.x - v2.z*v1.x;
	v0.z = v1.x * v2.y - v2.x*v1.y;

	return	v0;
}
//----------------------------------------------------------
vector vnormal( vector v1  )
//----------------------------------------------------------
{
//	vector	v0;

	float	s = fscalar(v1);

	v1.x /= s;
	v1.y /= s;
	v1.z /= s;

	return	v1;
}

//----------------------------------------------------------
vector vector::operator * ( matrix m1 )
//----------------------------------------------------------
{
	return vector(
		x * m1.m[0][0] + y* m1.m[1][0] + z*m1.m[2][0] + m1.m[3][0],
		x * m1.m[0][1] + y* m1.m[1][1] + z*m1.m[2][1] + m1.m[3][1],
		x * m1.m[0][2] + y* m1.m[1][2] + z*m1.m[2][2] + m1.m[3][2],
		x * m1.m[0][3] + y* m1.m[1][3] + z*m1.m[2][3] + m1.m[3][3]
	);
}

//----------------------------------------------------------
vector	vlen_point_line( vector& P, vector& V )
//----------------------------------------------------------
{
	vector	q;
	q = P + ((fdot( -P , V) + fdot( P, V ))  /  fscalar(V) * fscalar(V)) * V; 
	return	q;
}







