#include	<math.h>
#include	<string.h>
#include	"vector.h"
#include	"matrix.h"

//##########################################################
// matrix
//##########################################################
//----------------------------------------------------------
matrix::matrix()
//----------------------------------------------------------
{
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;

	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = 0.0f;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}
//----------------------------------------------------------
matrix::matrix(
//----------------------------------------------------------
	float a0, float a1, float a2, float a3,
	float b0, float b1, float b2, float b3,
	float c0, float c1, float c2, float c3,
	float d0, float d1, float d2, float d3
)
{	
	m[0][0] = a0;
	m[0][1] = a1;
	m[0][2] = a2;
	m[0][3] = a3;
	m[1][0] = b0;
	m[1][1] = b1;
	m[1][2] = b2;
	m[1][3] = b3;
	m[2][0] = c0;
	m[2][1] = c1;
	m[2][2] = c2;
	m[2][3] = c3;
	m[3][0] = d0;
	m[3][1] = d1;
	m[3][2] = d2;
	m[3][3] = d3;
}

//----------------------------------------------------------
matrix matrix::mrot()
//----------------------------------------------------------
{
	matrix	m0;
	m0 = *this;
	m0.m[3][0] = 0.0f;
	m0.m[3][1] = 0.0f;
	m0.m[3][2] = 0.0f;
	m0.m[3][3] = 1.0f;

	return	m0;
}
//----------------------------------------------------------
vector matrix::vx()
//----------------------------------------------------------
{
	vector v0;
	v0.x = m[0][0];
	v0.y = m[0][1];
	v0.z = m[0][2];

	return	v0;
}
//----------------------------------------------------------
vector matrix::vy()
//----------------------------------------------------------
{
	vector v0;
	v0.x = m[1][0];
	v0.y = m[1][1];
	v0.z = m[1][2];

	return	v0;
}
//----------------------------------------------------------
vector matrix::vz()
//----------------------------------------------------------
{
	vector v0;
	v0.x = m[2][0];
	v0.y = m[2][1];
	v0.z = m[2][2];

	return	v0;
}
//----------------------------------------------------------
vector matrix::vpos()
//----------------------------------------------------------
{
	vector v0;
	v0.x = m[3][0];
	v0.y = m[3][1];
	v0.z = m[3][2];

	return	v0;
}

//----------------------------------------------------------
matrix matrix::operator * ( matrix m1 )
//----------------------------------------------------------
{
	return matrix(
		m[0][0]*m1.m[0][0] + m[0][1]*m1.m[1][0] +m[0][2]*m1.m[2][0] + m[0][3]*m1.m[3][0],
		m[0][0]*m1.m[0][1] + m[0][1]*m1.m[1][1] +m[0][2]*m1.m[2][1] + m[0][3]*m1.m[3][1],
		m[0][0]*m1.m[0][2] + m[0][1]*m1.m[1][2] +m[0][2]*m1.m[2][2] + m[0][3]*m1.m[3][2],
		m[0][0]*m1.m[0][3] + m[0][1]*m1.m[1][3] +m[0][2]*m1.m[2][3] + m[0][3]*m1.m[3][3],
		m[1][0]*m1.m[0][0] + m[1][1]*m1.m[1][0] +m[1][2]*m1.m[2][0] + m[1][3]*m1.m[3][0],
		m[1][0]*m1.m[0][1] + m[1][1]*m1.m[1][1] +m[1][2]*m1.m[2][1] + m[1][3]*m1.m[3][1],
		m[1][0]*m1.m[0][2] + m[1][1]*m1.m[1][2] +m[1][2]*m1.m[2][2] + m[1][3]*m1.m[3][2],
		m[1][0]*m1.m[0][3] + m[1][1]*m1.m[1][3] +m[1][2]*m1.m[2][3] + m[1][3]*m1.m[3][3],
		m[2][0]*m1.m[0][0] + m[2][1]*m1.m[1][0] +m[2][2]*m1.m[2][0] + m[2][3]*m1.m[3][0],
		m[2][0]*m1.m[0][1] + m[2][1]*m1.m[1][1] +m[2][2]*m1.m[2][1] + m[2][3]*m1.m[3][1],
		m[2][0]*m1.m[0][2] + m[2][1]*m1.m[1][2] +m[2][2]*m1.m[2][2] + m[2][3]*m1.m[3][2],
		m[2][0]*m1.m[0][3] + m[2][1]*m1.m[1][3] +m[2][2]*m1.m[2][3] + m[2][3]*m1.m[3][3],
		m[3][0]*m1.m[0][0] + m[3][1]*m1.m[1][0] +m[3][2]*m1.m[2][0] + m[3][3]*m1.m[3][0],
		m[3][0]*m1.m[0][1] + m[3][1]*m1.m[1][1] +m[3][2]*m1.m[2][1] + m[3][3]*m1.m[3][1],
		m[3][0]*m1.m[0][2] + m[3][1]*m1.m[1][2] +m[3][2]*m1.m[2][2] + m[3][3]*m1.m[3][2],
		m[3][0]*m1.m[0][3] + m[3][1]*m1.m[1][3] +m[3][2]*m1.m[2][3] + m[3][3]*m1.m[3][3]
	);
}

//----------------------------------------------------------
matrix& matrix::operator *= ( matrix m1 )
//----------------------------------------------------------
{
		m[0][0] = m[0][0]*m1.m[0][0] + m[0][1]*m1.m[1][0] +m[0][2]*m1.m[2][0] + m[0][3]*m1.m[3][0];
		m[0][1] = m[0][0]*m1.m[0][1] + m[0][1]*m1.m[1][1] +m[0][2]*m1.m[2][1] + m[0][3]*m1.m[3][1];
		m[0][2] = m[0][0]*m1.m[0][2] + m[0][1]*m1.m[1][2] +m[0][2]*m1.m[2][2] + m[0][3]*m1.m[3][2];
		m[0][3] = m[0][0]*m1.m[0][3] + m[0][1]*m1.m[1][3] +m[0][2]*m1.m[2][3] + m[0][3]*m1.m[3][3];
		m[1][0] = m[1][0]*m1.m[0][0] + m[1][1]*m1.m[1][0] +m[1][2]*m1.m[2][0] + m[1][3]*m1.m[3][0];
		m[1][1] = m[1][0]*m1.m[0][1] + m[1][1]*m1.m[1][1] +m[1][2]*m1.m[2][1] + m[1][3]*m1.m[3][1];
		m[1][2] = m[1][0]*m1.m[0][2] + m[1][1]*m1.m[1][2] +m[1][2]*m1.m[2][2] + m[1][3]*m1.m[3][2];
		m[1][3] = m[1][0]*m1.m[0][3] + m[1][1]*m1.m[1][3] +m[1][2]*m1.m[2][3] + m[1][3]*m1.m[3][3];
		m[2][0] = m[2][0]*m1.m[0][0] + m[2][1]*m1.m[1][0] +m[2][2]*m1.m[2][0] + m[2][3]*m1.m[3][0];
		m[2][1] = m[2][0]*m1.m[0][1] + m[2][1]*m1.m[1][1] +m[2][2]*m1.m[2][1] + m[2][3]*m1.m[3][1];
		m[2][2] = m[2][0]*m1.m[0][2] + m[2][1]*m1.m[1][2] +m[2][2]*m1.m[2][2] + m[2][3]*m1.m[3][2];
		m[2][3] = m[2][0]*m1.m[0][3] + m[2][1]*m1.m[1][3] +m[2][2]*m1.m[2][3] + m[2][3]*m1.m[3][3];
		m[3][0] = m[3][0]*m1.m[0][0] + m[3][1]*m1.m[1][0] +m[3][2]*m1.m[2][0] + m[3][3]*m1.m[3][0];
		m[3][1] = m[3][0]*m1.m[0][1] + m[3][1]*m1.m[1][1] +m[3][2]*m1.m[2][1] + m[3][3]*m1.m[3][1];
		m[3][2] = m[3][0]*m1.m[0][2] + m[3][1]*m1.m[1][2] +m[3][2]*m1.m[2][2] + m[3][3]*m1.m[3][2];
		m[3][3] = m[3][0]*m1.m[0][3] + m[3][1]*m1.m[1][3] +m[3][2]*m1.m[2][3] + m[3][3]*m1.m[3][3];
	return *this;
}

//----------------------------------------------------------
static	int	matrix_invers( matrix& dstM, matrix srcM )
//----------------------------------------------------------
{
	int		i;
	int		j;
	int		k;
	int		r;
	float	a;
	float	M[4][8];

	// 拡大行列 M を作る。
	// A
	for ( i = 0 ; i < 4 ; i++ )
	{
		for ( j = 0 ; j < 4 ; j++ )
		{
			M[i][j] = srcM.m[i][j];
		}
		for ( j = 4 ; j < 8 ; j++ )
		{
			M[i][j] = 0;
		}
	}
	M[0][4] = 1;
	M[1][5] = 1;
	M[2][6] = 1;
	M[3][7] = 1;

	// 逆行列を求める。

	// B
	for ( j = 0 ; j < 4 ; j++ )
	{

		// M[i][j] != 0 を満たす最初の行 i (i>=j)を見つける。	
		// C
		for ( i = j ; i < 4 ; i++ )
		{
			if ( M[i][j] != 0 ) break;
		}
		// みつからなかったら、は存在しない。＞エラー
		if ( i >= 4 || j >= 4 ) 
		{
//			printf("error : there is no invers matrix\n");
//			exit(0);
			return	false;
		}

		// i != j ならば、i行とj行を入れ替える。
		// D
		if ( i != j ) 
		{
			int	k;
			for ( k = 0 ; k < 8 ; k++ )
			{
				a = M[i][k];
				M[i][k] = M[j][k];
				M[j][k] = a;
			}
		}
		
		// j行に1/M[j][j]をかける。
		//E
		a = 1.0/M[j][j];
		for ( k = 0 ; k < 8 ; k++ )
		{
			M[j][k] *= a;
		}

		// 0 < r < n かつ r != j である各行 r について、j行の -M[r][j] 倍を r 行に加える。
		// F
		for ( r = 0 ; r < 4 ; r++ )
		{
			if ( r != j )
			{
				a = -M[r][j];
				for ( k = 0 ; k < 8 ; k++ )
				{
					M[r][k] += M[j][k] * a;
				}
			}
		}

	}
#if 0
	printf( "out\n");
	for ( i = 0 ; i < 4 ; i++ )
	{
		printf( "%f %f %f %f | %f %f %f %f \n", M[i][0], M[i][1], M[i][2], M[i][3], M[i][4], M[i][5], M[i][6], M[i][7] );
	}
	printf( "---\n");
#endif
	
	for ( i = 0 ; i < 4 ; i++ )
	{
		for ( j = 0 ; j < 4 ; j++ )
		{
			dstM.m[i][j] = M[i][j+4] ;
		}
	}

	return	true;
}
/*
//----------------------------------------------------------
static	void matrix_identity( matrix& mat )
//----------------------------------------------------------
{
	mat.m[0][0] = 1.0;	mat.m[0][1] = 0.0;	mat.m[0][2] = 0.0;	mat.m[0][3] = 0.0;
	mat.m[1][0] = 0.0;	mat.m[1][1] = 1.0;	mat.m[1][2] = 0.0;	mat.m[1][3] = 0.0;
	mat.m[2][0] = 0.0;	mat.m[2][1] = 0.0;	mat.m[2][2] = 1.0;	mat.m[2][3] = 0.0;
	mat.m[3][0] = 0.0;	mat.m[3][1] = 0.0;	mat.m[3][2] = 0.0;	mat.m[3][3] = 1.0;
}
*/

//----------------------------------------------------------
void	multiple_v_m( vector& a, vector& b, matrix& c )
//----------------------------------------------------------
{
	vector d;

	d.x = b.x * c.m[0][0] + b.y* c.m[1][0] + b.z*c.m[2][0] + 1.0*c.m[3][0];
	d.y = b.x * c.m[0][1] + b.y* c.m[1][1] + b.z*c.m[2][1] + 1.0*c.m[3][1];
	d.z = b.x * c.m[0][2] + b.y* c.m[1][2] + b.z*c.m[2][2] + 1.0*c.m[3][2];
	d.w	= b.x * c.m[0][3] + b.y* c.m[1][3] + b.z*c.m[2][3] + 1.0*c.m[3][3];

	a = d;
}
//----------------------------------------------------------
void multiple_m_v( vector& a, matrix& b, vector& c )
//----------------------------------------------------------
{
	vector d;
	d.x = b.m[0][0] * c.x + b.m[0][1] * c.y	+ b.m[0][2] * c.z	+ b.m[0][3] * 1.0;
	d.y = b.m[1][0] * c.x + b.m[1][1] * c.y	+ b.m[1][2] * c.z	+ b.m[1][3] * 1.0;
	d.z = b.m[2][0] * c.x + b.m[2][1] * c.y	+ b.m[2][2] * c.z	+ b.m[2][3] * 1.0;
	d.w = b.m[3][0] * c.x + b.m[3][1] * c.y	+ b.m[3][2] * c.z	+ b.m[3][3] * 1.0;

	a = d;
}

//----------------------------------------------------------
vector matrix::operator * ( vector v1 )
//----------------------------------------------------------
{
	return vector(
		v1.x * m[0][0] + v1.y* m[1][0] + v1.z*m[2][0] + m[3][0],
		v1.x * m[0][1] + v1.y* m[1][1] + v1.z*m[2][1] + m[3][1],
		v1.x * m[0][2] + v1.y* m[1][2] + v1.z*m[2][2] + m[3][2],
		v1.x * m[0][3] + v1.y* m[1][3] + v1.z*m[2][3] + m[3][3]
	);
}

//----------------------------------------------------------
vector	vmulmv( matrix m1, vector v1 )
//----------------------------------------------------------
{
	vector v0;
	v0.x = m1.m[0][0] * v1.x + m1.m[0][1] * v1.y	+ m1.m[0][2] * v1.z	+ m1.m[0][3] * 1.0;
	v0.y = m1.m[1][0] * v1.x + m1.m[1][1] * v1.y	+ m1.m[1][2] * v1.z	+ m1.m[1][3] * 1.0;
	v0.z = m1.m[2][0] * v1.x + m1.m[2][1] * v1.y	+ m1.m[2][2] * v1.z	+ m1.m[2][3] * 1.0;
	v0.w = m1.m[3][0] * v1.x + m1.m[3][1] * v1.y	+ m1.m[3][2] * v1.z	+ m1.m[3][3] * 1.0;

	return	v0;
}

//----------------------------------------------------------
vector	vmulvm( vector v1, matrix m1 )
//----------------------------------------------------------
{
	vector v0;

	v0.x = v1.x * m1.m[0][0] + v1.y* m1.m[1][0] + v1.z*m1.m[2][0] + 1.0*m1.m[3][0];
	v0.y = v1.x * m1.m[0][1] + v1.y* m1.m[1][1] + v1.z*m1.m[2][1] + 1.0*m1.m[3][1];
	v0.z = v1.x * m1.m[0][2] + v1.y* m1.m[1][2] + v1.z*m1.m[2][2] + 1.0*m1.m[3][2];
	v0.w	= v1.x * m1.m[0][3] + v1.y* m1.m[1][3] + v1.z*m1.m[2][3] + 1.0*m1.m[3][3];

	return v0;
}

//###############################################################
//	functions
//###############################################################
//-----------------------------------------------------------------------------
float	getDistance( 
//-----------------------------------------------------------------------------

	//	捕捉点		問題となる点
	float	qx
	,float	qy
	,float	qz

	//	遠点V		注視点
	,float ax0
	,float ay0
	,float az0

	//	原点P0		視点など
	,float ax1
	,float ay1
	,float az1
)
{
	//	点と直線の位置
/*
	ベクトル v = q<qx,qy,qz> - a1<ax1,ay1,az1>
	ベクトル w = normal( a0<ax0,ay0,az0> - a1<ax1,ay1,az1> )
	n = v・w
	w * = n;
	w += a1
	w -= q
*/
	{
		float	x0 = ax1;	//	-1
		float	y0 = ay1;	//	2
		float	z0 = az1;	//	
		float	x1 = ax0;	//	2
		float	y1 = ay0;	//	0
		float	z1 = az0;	//	
		{
			float	vx = qx-x0;
			float	vy = qy-y0;
			float	vz = qz-z0;

			float	wx = x1-x0;
			float	wy = y1-y0;
			float	wz = z1-z0;
			// 正規化
			float	wl = sqrtf( wx*wx + wy*wy +wz*wz );
			wx /= wl;
			wy /= wl;
			wz /= wl;

			float	n = vx*wx + vy*wy + vz*wz;

			wx *= n;
			wy *= n;
			wz *= n;

			wx	+= x0;
			wy	+= y0;
			wz	+= z0;
			float	lx = wx-qx;
			float	ly = wy-qy;
			float	lz = wz-qz;
			float	k = sqrtf( lx*lx + ly*ly +lz*lz );
			return	k;
		}
	}
}
//----------------------------------------------------------------------
matrix	mnormal( matrix m0 )
//----------------------------------------------------------------------
{
	float	xx = m0.m[0][0];
	float	xy = m0.m[0][1];
	float	xz = m0.m[0][2];
	float	yx = m0.m[1][0];
	float	yy = m0.m[1][1];
	float	yz = m0.m[1][2];
	float	zx = m0.m[2][0];
	float	zy = m0.m[2][1];
	float	zz = m0.m[2][2];
	float	max;

	// Zを正規化
	max = sqrtf( zx*zx + zy*zy + zz*zz );
	zx = zx/max;
	zy = zy/max;
	zz = zz/max;

	// Xを外積から求める
	xx = (yy*zz - yz*zy);
	xy = (yz*zx - yx*zz);
	xz = (yx*zy - yy*zx);

	// Xを正規化
	max = sqrtf( xx*xx + xy*xy + xz*xz );
	xx = xx/max;
	xy = xy/max;
	xz = xz/max;

	// Yを外積から求める
	yx = (zy*xz - zz*xy);
	yy = (zz*xx - zx*xz);
	yz = (zx*xy - zy*xx);

	m0.m[2][0] = zx;
	m0.m[2][1] = zy;
	m0.m[2][2] = zz;
	m0.m[0][0] = xx;
	m0.m[0][1] = xy;
	m0.m[0][2] = xz;
	m0.m[1][0] = yx;
	m0.m[1][1] = yy;
	m0.m[1][2] = yz;

	return m0;
}
//----------------------------------------------------------------------
//	y軸基準で正規化	
matrix	mnormaly( matrix m0 )	
//----------------------------------------------------------------------
{
	float	xx = m0.m[0][0];
	float	xy = m0.m[0][1];
	float	xz = m0.m[0][2];
	float	yx = m0.m[1][0];
	float	yy = m0.m[1][1];
	float	yz = m0.m[1][2];
	float	zx = m0.m[2][0];
	float	zy = m0.m[2][1];
	float	zz = m0.m[2][2];
	float	max;

	// Yを正規化
	max = sqrtf( yx*yx + yy*yy + yz*yz );
	yx = yx/max;
	yy = yy/max;
	yz = yz/max;

	// Xを外積から求める
	xx = (yy*zz - yz*zy);
	xy = (yz*zx - yx*zz);
	xz = (yx*zy - yy*zx);

	// Xを正規化
	max = sqrtf( xx*xx + xy*xy + xz*xz );
	xx = xx/max;
	xy = xy/max;
	xz = xz/max;

	// Zを外積から求める
	zx = (yz*xy - yy*xz);
	zy = (yx*xz - yz*xx);
	zz = (yy*xx - yx*xy);

	m0.m[2][0] = zx;
	m0.m[2][1] = zy;
	m0.m[2][2] = zz;
	m0.m[0][0] = xx;
	m0.m[0][1] = xy;
	m0.m[0][2] = xz;
	m0.m[1][0] = yx;
	m0.m[1][1] = yy;
	m0.m[1][2] = yz;

	return m0;
}

//----------------------------------------------------------
matrix	minv( matrix m1 )
//----------------------------------------------------------
{
	matrix	m0;
	matrix_invers( m0, m1 );
	return m0;
}
//----------------------------------------------------------
matrix	mtrans( float x,	float y, float z )
//----------------------------------------------------------
{
	return matrix(
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		x,y,z,1
	);
}
//----------------------------------------------------------
matrix	mident()
//----------------------------------------------------------
{
	return matrix(
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	);
}
//----------------------------------------------------------
matrix	mscale( float n )
//----------------------------------------------------------
{
	return matrix(
		n,0,0,0,
		0,n,0,0,
		0,0,n,0,
		0,0,0,1
	);
}
//----------------------------------------------------------
matrix	mscale( float x, float y, float z )
//----------------------------------------------------------
{
	return matrix(
		x,0,0,0,
		0,y,0,0,
		0,0,z,0,
		0,0,0,1
	);
}
//----------------------------------------------------------
matrix	mrotx( float n )
//----------------------------------------------------------
{
	float	c = cosf( n );
	float	s = sinf( n );

	return matrix(
		 1, 0, 0, 0,
		 0, c, s, 0,
		 0,-s, c, 0,
		 0, 0, 0, 1
	);
}
//----------------------------------------------------------
matrix	mroty( float n )
//----------------------------------------------------------
{
	float	c = cosf( n );
	float	s = sinf( n );

	return matrix(
		 c, 0,-s, 0,
		 0, 1, 0, 0,
		 s, 0, c, 0,
		 0, 0, 0, 1
	);
}
//----------------------------------------------------------
matrix	mrotz( float n )
//----------------------------------------------------------
{
	float	c = cosf( n );
	float	s = sinf( n );

	return matrix(
		 c, s, 0, 0,
		-s, c, 0, 0,
		 0, 0, 1, 0,
		 0, 0, 0, 1
	);
}
//-----------------------------------------------------------------------------
matrix mrotv( vector v1, float r )
//-----------------------------------------------------------------------------
{
	matrix m0;

    float s = sinf( r / 2.0f );
    float x = v1.x * s;
    float y = v1.y * s;
    float z = v1.z * s;
    float w = cosf( r / 2.0f );
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float ww = w * w;

    m0.m[0][0] = ww + xx - yy - zz;
    m0.m[0][1] = 2 * ( ( x * y ) + ( w * z ) );
    m0.m[0][2] = 2 * ( ( x * z ) - ( w * y ) );
    m0.m[0][3] = 0.0f;

    m0.m[1][0] = 2 * ( ( x * y ) - ( w * z ) );
    m0.m[1][1] = ww - xx + yy - zz;
    m0.m[1][2] = 2 * ( ( y * z ) + ( w * x ) );
    m0.m[1][3] = 0.0f;

    m0.m[2][0] = 2 * ( ( x * z ) + ( w * y ) );
    m0.m[2][1] = 2 * ( ( y * z ) - ( w * x ) );
    m0.m[2][2] = ww - xx - yy + zz;
    m0.m[2][3] = 0.0f;

    m0.m[3][0] = 0.0f;
    m0.m[3][1] = 0.0f;
    m0.m[3][2] = 0.0f;
    m0.m[3][3] = 1.0f;

	return	m0;
}

/*
//-----------------------------------------------------------------------------
// Name: math_MatrixMultiply()
// Desc: Does the matrix operation: [Q] = [A] * [B].
//-----------------------------------------------------------------------------
void math_MatrixMultiply( matrix& q, matrix& a, matrix& b )
{
		float* pA = (float*)&a;
		float* pB = (float*)&b;
		float	pM[16];

//		ZeroMemory( pM, sizeof(matrix) );
	 memset( pM, 0, sizeof(matrix) );

		for( int i=0; i<4; i++ ) 
				for( int j=0; j<4; j++ ) 
						for( int k=0; k<4; k++ ) 
								pM[4*i+j] += pA[4*k+j] * pB[4*i+k];

		memcpy( &q, pM, sizeof(matrix) );
}
*/



//-----------------------------------------------------------------------------
// Name: math_MatrixInvert()
// Desc: Does the matrix operation: [Q] = inv[A]. Note: this function only
//			 works for matrices with [0 0 0 1] for the 4th column.
//-----------------------------------------------------------------------------
int math_MatrixInvert( matrix& q, matrix& a )
{
		if( fabs(a.m[3][3] - 1.0f) > .001f)
				return false;
		if( fabs(a.m[0][3]) > .001f || fabs(a.m[1][3]) > .001f || fabs(a.m[2][3]) > .001f )
				return false;

		float fDetInv = 1.0f / ( a.m[0][0] * ( a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1] ) -
														 a.m[0][1] * ( a.m[1][0] * a.m[2][2] - a.m[1][2] * a.m[2][0] ) +
														 a.m[0][2] * ( a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0] ) );

		q.m[0][0] =	fDetInv * ( a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1] );
		q.m[0][1] = -fDetInv * ( a.m[0][1] * a.m[2][2] - a.m[0][2] * a.m[2][1] );
		q.m[0][2] =	fDetInv * ( a.m[0][1] * a.m[1][2] - a.m[0][2] * a.m[1][1] );
		q.m[0][3] = 0.0f;

		q.m[1][0] = -fDetInv * ( a.m[1][0] * a.m[2][2] - a.m[1][2] * a.m[2][0] );
		q.m[1][1] =	fDetInv * ( a.m[0][0] * a.m[2][2] - a.m[0][2] * a.m[2][0] );
		q.m[1][2] = -fDetInv * ( a.m[0][0] * a.m[1][2] - a.m[0][2] * a.m[1][0] );
		q.m[1][3] = 0.0f;

		q.m[2][0] =	fDetInv * ( a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0] );
		q.m[2][1] = -fDetInv * ( a.m[0][0] * a.m[2][1] - a.m[0][1] * a.m[2][0] );
		q.m[2][2] =	fDetInv * ( a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0] );
		q.m[2][3] = 0.0f;

		q.m[3][0] = -( a.m[3][0] * q.m[0][0] + a.m[3][1] * q.m[1][0] + a.m[3][2] * q.m[2][0] );
		q.m[3][1] = -( a.m[3][0] * q.m[0][1] + a.m[3][1] * q.m[1][1] + a.m[3][2] * q.m[2][1] );
		q.m[3][2] = -( a.m[3][0] * q.m[0][2] + a.m[3][1] * q.m[1][2] + a.m[3][2] * q.m[2][2] );
		q.m[3][3] = 1.0f;

		return true;
}




//-----------------------------------------------------------------------------
// Name: math_VectorMatrixMultiply()
// Desc: Multiplies a vector by a matrix
//-----------------------------------------------------------------------------
int math_VectorMatrixMultiply( vector& vDest, vector& vSrc,
																			matrix& mat)
{
		float x = vSrc.x*mat.m[0][0] + vSrc.y*mat.m[1][0] + vSrc.z* mat.m[2][0] + mat.m[3][0];
		float y = vSrc.x*mat.m[0][1] + vSrc.y*mat.m[1][1] + vSrc.z* mat.m[2][1] + mat.m[3][1];
		float z = vSrc.x*mat.m[0][2] + vSrc.y*mat.m[1][2] + vSrc.z* mat.m[2][2] + mat.m[3][2];
		float w = vSrc.x*mat.m[0][3] + vSrc.y*mat.m[1][3] + vSrc.z* mat.m[2][3] + mat.m[3][3];
		
		const float g_EPSILON	=	1.0e-5f;								 // Tolerance for FLOATs

		if( fabs( w ) < g_EPSILON )
				return false;

		vDest.x = x/w;
		vDest.y = y/w;
		vDest.z = z/w;

		return true;
}




/*
//-----------------------------------------------------------------------------
// Name: math_VertexMatrixMultiply()
// Desc: Multiplies a vertex by a matrix
//-----------------------------------------------------------------------------
int math_VertexMatrixMultiply( D3DVERTEX& vDest, D3DVERTEX& vSrc,
																			matrix& mat )
{
		int		hr;
		vector* pSrcVec	= (vector*)&vSrc.x;
		vector* pDestVec = (vector*)&vDest.x;

		if( SUCCEEDED( hr = math_VectorMatrixMultiply( *pDestVec, *pSrcVec,
																											mat ) ) )
		{
				pSrcVec	= (vector*)&vSrc.nx;
				pDestVec = (vector*)&vDest.nx;
				hr = math_VectorMatrixMultiply( *pDestVec, *pSrcVec, mat );
		}
		return hr;
}
*/


//----------------------------------------------------------
loc::loc()
//----------------------------------------------------------
{
	x = 0.0f;
	y = 1.0f;
	z = 0.0f;
	w = 0.0f;
}
//----------------------------------------------------------
loc::loc( float x1, float y1, float z1, float w1 )
//----------------------------------------------------------
{
	x = x1;
	y = y1;
	z = z1;
	w = w1;
}

//----------------------------------------------------------
quat::quat()
//----------------------------------------------------------
{
	w = 1.0f;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}
//----------------------------------------------------------
quat::quat( float w1, float x1, float y1, float z1 )
//----------------------------------------------------------
{
	x = x1;
	y = y1;
	z = z1;
	w = w1;
}

//-----------------------------------------------------------------------------
// Name: math_QuaternionFromRotation()
// Desc: Converts a normalized axis and angle to a unit quaternion.
//-----------------------------------------------------------------------------
void math_QuaternionFromRotation( float& x, float& y, float& z, float& w,
																		 vector& v, float fTheta )
{
		x = sinf(fTheta/2) * v.x;
		y = sinf(fTheta/2) * v.y;
		z = sinf(fTheta/2) * v.z;
		w = cosf(fTheta/2);
}




//-----------------------------------------------------------------------------
// Name: math_RotationFromQuaternion()
// Desc: Converts a normalized axis and angle to a unit quaternion.
//-----------------------------------------------------------------------------
void math_RotationFromQuaternion( vector& v, float& fTheta,
																		 float x, float y, float z, float w )
																			
{
		fTheta = (float)( acos(w) * 2 );
		v.x		= (float)( x / sin(fTheta/2) );
		v.y		= (float)( y / sin(fTheta/2) );
		v.z		= (float)( z / sin(fTheta/2) );
}




//-----------------------------------------------------------------------------
// Name: math_QuaternionFromAngles()
// Desc: Converts euler angles to a unit quaternion.
//-----------------------------------------------------------------------------
void math_QuaternionFromAngles( float& x, float& y, float& z, float& w,
																	 float fYaw, float fPitch, float fRoll )
																				
{
		float fSinYaw	 = sinf(fYaw/2);
		float fSinPitch = sinf(fPitch/2);
		float fSinRoll	= sinf(fRoll/2);
		float fCosYaw	 = cosf(fYaw/2);
		float fCosPitch = cosf(fPitch/2);
		float fCosRoll	= cosf(fRoll/2);

		x = fSinRoll * fCosPitch * fCosYaw - fCosRoll * fSinPitch * fSinYaw;
		y = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
		z = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
		w = fCosRoll * fCosPitch * fCosYaw + fSinRoll * fSinPitch * fSinYaw;
}






/*
//----------------------------------------------------------
matrix	mq( quat q1 )
//----------------------------------------------------------
{
	matrix	m0;
#if 0
	math_MatrixFromQuaternion( 
		m0,
		q1.x, q1.y, q1.z, q1.w
	);
#else
	m0 = QuatConvertToMatrix( q1 );
#endif

	return	m0;
}
*/
//-----------------------------------------------------------------------------
// Name: math_QuaternionFromMatrix()
// Desc: Converts a rotation matrix into a unit quaternion.
//-----------------------------------------------------------------------------
void math_QuaternionFromMatrix( float& x, float& y, float& z, float& w, matrix& mat )
{
		if( mat.m[0][0] + mat.m[1][1] + mat.m[2][2] > 0.0f )
		{
				float s = sqrtf( mat.m[0][0] + mat.m[1][1] + mat.m[2][2] + mat.m[3][3] );

				x = (mat.m[1][2]-mat.m[2][1]) / (2*s);
				y = (mat.m[2][0]-mat.m[0][2]) / (2*s);
				z = (mat.m[0][1]-mat.m[1][0]) / (2*s);
				w = 0.5f * s;
		}
		else
		{


		}
		float xx = x*x; float yy = y*y; float zz = z*z;
		float xy = x*y; float xz = x*z; float yz = y*z;
		float wx = w*x; float wy = w*y; float wz = w*z;
		
		mat.m[0][0] = 1 - 2 * ( yy + zz ); 
		mat.m[0][1] =		 2 * ( xy - wz );
		mat.m[0][2] =		 2 * ( xz + wy );

		mat.m[1][0] =		 2 * ( xy + wz );
		mat.m[1][1] = 1 - 2 * ( xx + zz );
		mat.m[1][2] =		 2 * ( yz - wx );

		mat.m[2][0] =		 2 * ( xz - wy );
		mat.m[2][1] =		 2 * ( yz + wx );
		mat.m[2][2] = 1 - 2 * ( xx + yy );

		mat.m[0][3] = mat.m[1][3] = mat.m[2][3] = 0.0f;
		mat.m[3][0] = mat.m[3][1] = mat.m[3][2] = 0.0f;
		mat.m[3][3] = 1.0f;
}

//-----------------------------------------------------------------------------
quat QuatConvertFromMatrix( matrix m)
//-----------------------------------------------------------------------------
{
	quat	q0;

		float s;
		float tr = m.m[0][0] + m.m[1][1] + m.m[2][2] + 1.0;
		if (tr >= 1.0) 
		{
				s = 0.5 / sqrtf(tr);

				q0.w = 0.25 / s,
				q0.x = (m.m[1][2] - m.m[2][1]) * s;
				q0.y = (m.m[2][0] - m.m[0][2]) * s;
				q0.z = (m.m[0][1] - m.m[1][0]) * s;
		} 
		else 
		{
				float max;
				if(m.m[1][1] > m.m[2][2])		max = m.m[1][1];
				else								 max = m.m[2][2];
				
				if (max < m.m[0][0]) 
				{
						s = sqrtf(m.m[0][0] - (m.m[1][1] + m.m[2][2]) + 1.0);
						float x = s * 0.5;
						s = 0.5 / s;
 
						q0.x = x;
						q0.y = (m.m[0][1] + m.m[1][0]) * s;
						q0.z = (m.m[2][0] + m.m[0][2]) * s;
						q0.w = (m.m[1][2] - m.m[2][1]) * s;
				} 
				else 
				if (max == m.m[1][1]) 
				{
						s = sqrtf(m.m[1][1] - (m.m[2][2] + m.m[0][0]) + 1.0);
						float y = s * 0.5;
						s = 0.5 / s;
 
						q0.x = (m.m[0][1] + m.m[1][0]) * s;
						q0.y = y;
						q0.z = (m.m[1][2] + m.m[2][1]) * s;
						q0.w = (m.m[2][0] - m.m[0][2]) * s;
				} 
				else 
				{
						s = sqrtf(m.m[2][2] - (m.m[0][0] + m.m[1][1]) + 1.0);
						float z = s * 0.5;
						s = 0.5 / s;
 
						q0.x = (m.m[2][0] + m.m[0][2]) * s;
						q0.y = (m.m[1][2] + m.m[2][1]) * s;
						q0.z = z;
						q0.w = (m.m[0][1] - m.m[1][0]) * s;
				}
		}
		return q0;

}

//----------------------------------------------------------
quat	qm( matrix m1 )
//----------------------------------------------------------
{
	quat	q0;
#if 0
	math_QuaternionFromMatrix( 
		q0.x, q0.y, q0.z, q0.w,
		m1
	);
#else
	q0 = QuatConvertFromMatrix( m1 );
#endif

	return	q0;
}








//-----------------------------------------------------------------------------
// Name: math_SlerpQuaternions()
// Desc: Compute a quaternion which is the spherical linear interpolation
//			 between two other quaternions by dvFraction.
//-----------------------------------------------------------------------------
void math_QuaternionSlerp( float& Qx, float& Qy, float& Qz, float& Qw,
															float Ax, float Ay, float Az, float Aw,
															float Bx, float By, float Bz, float Bw,
															float fAlpha )
{
		float fScale1;
		float fScale2;

		// Compute dot product, aka cos(theta):
		float fCosTheta = Ax*Bx + Ay*By + Az*Bz + Aw*Bw;

		if( fCosTheta < 0.0f )
		{
				// Flip start quaternion
				Ax = -Ax; 
				Ay = -Ay; 
				Ax = -Az; 
				Aw = -Aw;
				fCosTheta = -fCosTheta;
		}

		if( fCosTheta + 1.0f > 0.05f )
		{
				// If the quaternions are close, use linear interploation
				if( 1.0f - fCosTheta < 0.05f )
				{
						fScale1 = 1.0f - fAlpha;
						fScale2 = fAlpha;
				}
				else // Otherwise, do spherical interpolation
				{
						float fTheta		= acosf( fCosTheta );
						float fSinTheta = sinf( fTheta );
						
						fScale1 = sinf( fTheta * (1.0f-fAlpha) ) / fSinTheta;
						fScale2 = sinf( fTheta * fAlpha ) / fSinTheta;
				}
		}
		else
		{
				Bx = -Ay;
				By =	Ax;
				Bz = -Aw;
				Bw =	Az;
				fScale1 = sinf( M_PI * (0.5f - fAlpha) );
				fScale2 = sinf( M_PI * fAlpha );
		}

		Qx = fScale1 * Ax + fScale2 * Bx;
		Qy = fScale1 * Ay + fScale2 * By;
		Qz = fScale1 * Az + fScale2 * Bz;
		Qw = fScale1 * Aw + fScale2 * Bw;
}

/*
//----------------------------------------------------------
quat	qslerp( quat q1, quat q2, float fAng )
//----------------------------------------------------------
{
	quat	q0;
	
	math_QuaternionSlerp( 
		q0.x, q0.y, q0.z, q0.w,
		q1.x, q1.y, q1.z, q1.w,
		q2.x, q2.y, q2.z, q2.w,
		fAng
	);
	return	q0;
}
*/



//-----------------------------------------------------------------------------
// Name: math_QuaternionMultiply()
// Desc: Mulitples two quaternions together as in {Q} = {A} * {B}.
//-----------------------------------------------------------------------------
void math_QuaternionMultiply( float& Qx, float& Qy, float& Qz, float& Qw,
																	float Ax, float Ay, float Az, float Aw,
																	float Bx, float By, float Bz, float Bw )
{
		float Dx = Bw * Ax + Bx * Aw + By * Az + Bz * Ay;
		float Dy = Bw * Ay + By * Aw + Bz * Ax + Bx * Az;
		float Dz = Bw * Az + Bz * Aw + Bx * Ay + By * Ax;
		float Dw = Bw * Aw + Bx * Ax + By * Ay + Bz * Az;

		Qx = Dx; 
		Qy = Dy; 
		Qz = Dz; 
		Qw = Dw;
}
	
/*
** 回転の変換行列 r <- クォータニオン q
*/
static	void qrot(float r[], float q[])
{
#define	W	0
#define	X	1
#define	Y	2
#define	Z	3

#define	XX	0
#define	XY	1
#define	XZ	2
#define	XW	3
#define	YX	4
#define	YY	5
#define	YZ	6
#define	YW	7
#define	ZX	8
#define	ZY	9
#define	ZZ	10
#define	ZW	11
#define	WX	12
#define	WY	13
#define	WZ	14
#define	WW	15

/*
また， quaternion を 3x3 変換行列 M に変換することもできます．
もし quaternion q の要素が [ w, (x,y,z) ] の時， 変換行列は

M =
				 { 1-2(yy + zz),	 2(xy - wz),	 2(xz + wy)}
				 {	 2(xy + wz), 1-2(xx + zz),	 2(yz - wx)}
				 {	 2(xz - wy),	 2(yz + wx), 1-2(xx + yy)}	

となります．
*/

#if 0
	float xx = q[X] * q[X] ;
	float yy = q[Y] * q[Y] ;
	float zz = q[Z] * q[Z] ;
	float xy = q[X] * q[Y] ;
	float yz = q[Y] * q[Z] ;
	float zx = q[Z] * q[X] ;
	float xw = q[X] * q[W] ;
	float yw = q[Y] * q[W] ;
	float zw = q[Z] * q[W] ;
	
	r[XX] = 1.0 - 2.0*(yy + zz);
	r[XY] =			 2.0*(xy + zw);
	r[XZ] =			 2.0*(zx - yw);

		r[YX] =			 2.0*(xy - zw);
		r[YY] = 1.0 - 2.0*(zz + xx);
		r[YZ] =			 2.0*(yz + xw);

			r[ZX] =			 2.0*(zx + yw);
			r[ZY] =			 2.0*(yz - xw);
			r[ZZ] = 1.0 - 2.0*(xx + yy);
#else
	float xx = q[X] * q[X] ;
	float yy = q[Y] * q[Y] ;
	float zz = q[Z] * q[Z] ;
	float xy = q[X] * q[Y] ;
	float yz = q[Y] * q[Z] ;
	float xz = q[Z] * q[X] ;
	float wx = q[X] * q[W] ;
	float wy = q[Y] * q[W] ;
	float wz = q[Z] * q[W] ;
	
	r[XX] = 1.0 - 2*(yy + zz);
	r[XY] =			 2*(xy + wz);
	r[XZ] =			 2*(xz - wy);

		r[YX] =			 2*(xy - wz);
		r[YY] = 1.0 - 2*(xx + zz);
		r[YZ] =			 2*(yz + wx);

			r[ZX] =			 2*(xz + wy);
			r[ZY] =			 2*(yz - wx);
			r[ZZ] = 1.0 - 2*(xx + yy);
#endif

	r[XW] = 0.0;
	r[YW] = 0.0;
	r[ZW] = 0.0;
	r[WX] = 0.0;
	r[WY] = 0.0;
	r[WZ] = 0.0;
	r[WW] = 1.0;
}
/*
** 球面線形補間 p ← q と r を t で補間したクォータニオン
*/
void slerp(float p[], const float q[], const float r[],const float t)
{
	float qr = q[0] * r[0] + q[1] * r[1] + q[2] * r[2] + q[3] * r[3];
	float ss = 1.0 - qr * qr;
	
	if (ss == 0.0) 
	{
		p[0] = q[0];
		p[1] = q[1];
		p[2] = q[2];
		p[3] = q[3];
	}
	else 
	{
		float sp = sqrt(ss);
		float ph = acos(qr);
		float pt = ph * t;
		float t1 = sinf(pt) / sp;
		float t0 = sinf(ph - pt) / sp;
		
		p[0] = q[0] * t0 + r[0] * t1;
		p[1] = q[1] * t0 + r[1] * t1;
		p[2] = q[2] * t0 + r[2] * t1;
		p[3] = q[3] * t0 + r[3] * t1;
	}

}
quat qslerp( quat q1, quat q2, float o )
{
	quat q0;
	
	float p[4];
	float q[4];
	float r[4];
	float t = o;
	
	q[X] = q1.x;
	q[Y] = q1.y;
	q[Z] = q1.z;
	q[W] = q1.w;
	
	r[X] = q2.x;
	r[Y] = q2.y;
	r[Z] = q2.z;
	r[W] = q2.w;
	
	slerp( p, q, r, t );
/*	math_QuaternionSlerp( 
		q0.x, q0.y, q0.z, q0.w,
		q1.x, q1.y, q1.z, q1.w,
		q2.x, q2.y, q2.z, q2.w,
		t
	);
*/

	q0.x = p[X];
	q0.y = p[Y];
	q0.z = p[Z];
	q0.w = p[W];

	return	q0;
}
/*
** クォータニオンの積 r <- p x q
*/
static	void qmul(float r[], const float B[], const float A[])
{
/*
qaqb
	= (a0 + a1i + a2j + a3k)(b0 + b1i + b2j + b3k)

	= a0b0 - a1b1 - a2b2 - a3b3
	+ (a0b1 + a1b0 + a2b3 - a3b2) i
	+ (a0b2 + a2b0 - a1b3 + a3b1) j
	+ (a0b3 + a3b0 + a1b2 - a2b1) k
*/
/*
q1･q2 = (a1 + b1i + c1j + d1k)･(a2 + b2i + c2j + c2j)
			= (a1 + b1i + c1j + d1k)･a2
			+ (a1 + b1i + c1j + d1k)･b2i
			+ (a1 + b1i + c1j + d1k)･c2j
			+ (a1 + b1i + c1j + d1k)･d2k
																		↓基底の積の規則を使って
			= (a1	+ b1i + c1j + d1k)･a2
			+ (a1i - b1	- c1k + d1j)･b2
			+ (a1j + b1k - c1	- d1i)･c2
			+ (a1k - b1j + c1i - d1 )･d2
																		↓それぞれの基底でまとめて
			= (a1a2 - b1b2 - c1c2 - d1d2)
			+ (a1b2 + b1a2 + c1d2 - d1c2)i
			+ (a1c2 - b1d2 + c1a2 + d1b2)j
			+ (a1d2 + b1c2 - c1b2 + d1a2)k
*/


#if 0
	r[0] = B[0] * A[0] - B[1] * A[1] - B[2] * A[2] - B[3] * A[3];
	r[1] = B[0] * A[1] + B[1] * A[0] + B[2] * A[3] - B[3] * A[2];
	r[2] = B[0] * A[2] - B[1] * A[3] + B[2] * A[0] + B[3] * A[1];
	r[3] = B[0] * A[3] + B[1] * A[2] - B[2] * A[1] + B[3] * A[0];
#else
	r[0] = B[W] * A[W] - B[X] * A[X] - B[Y] * A[Y] - B[Z] * A[Z];
	r[1] = B[W] * A[X] + B[X] * A[W] + B[Y] * A[Z] - B[Z] * A[Y];
	r[2] = B[W] * A[Y] - B[X] * A[Z] + B[Y] * A[W] + B[Z] * A[X];
	r[3] = B[W] * A[Z] + B[X] * A[Y] - B[Y] * A[X] + B[Z] * A[W];
#endif

}
//----------------------------------------------------------
quat quat::operator * ( quat q1 )
//----------------------------------------------------------
{
	quat	q0;

	float r[4];
	float p[4];
	float q[4];

	p[0] = w;
	p[1] = x;
	p[2] = y;
	p[3] = z;

	q[0] = q1.w;
	q[1] = q1.x;
	q[2] = q1.y;
	q[3] = q1.z;

	qmul( r, p, q );

	q0.w = r[0];
	q0.x = r[1];
	q0.y = r[2];
	q0.z = r[3];

	return	q0;
}
	
/*
** 回転の変換行列 r <- クォータニオン q
*/
//void mq(matrix r, float q[])
//-----------------------------------------------------------------------------
matrix mq( quat q1)
//-----------------------------------------------------------------------------
{
	matrix	m0;
	float	r[16];
	float	q[4];

	q[0] = q1.w;
	q[1] = q1.x;
	q[2] = q1.y;
	q[3] = q1.z;

	qrot( r, q );

	m0.m[0][0] = r[0];
	m0.m[0][1] = r[1];
	m0.m[0][2] = r[2];
	m0.m[0][3] = r[3];
	m0.m[1][0] = r[4];
	m0.m[1][1] = r[5];
	m0.m[1][2] = r[6];
	m0.m[1][3] = r[7];
	m0.m[2][0] = r[8];
	m0.m[2][1] = r[9];
	m0.m[2][2] = r[10];
	m0.m[2][3] = r[11];
	m0.m[3][0] = r[12];
	m0.m[3][1] = r[13];
	m0.m[3][2] = r[14];
	m0.m[3][3] = r[15];

	return	m0;
}

#if 0
//-----------------------------------------------------------------------------
// Name: math_MatrixFromQuaternion()
// Desc: Converts a unit quaternion into a rotation matrix.
//-----------------------------------------------------------------------------
void math_MatrixFromQuaternion( matrix& m0, float q.x, float q.y, float q.z, float q.w )
{
		float xx = q.x * q.x *2.0f; 
		float yy = q.y * q.y *2.0f; 
		float zz = q.z * q.z *2.0f;

		float xy = q.x * q.y *2.0f; 
		float yz = q.y * q.z *2.0f;
		float zx = q.z * q.x *2.0f; 

		float wx = q.w * q.x *2.0f; 
		float wy = q.w * q.y *2.0f; 
		float wz = q.w * q.z *2.0f;
		
		m0.m[0][0] = 1.0 -	yy + zz ; 
		m0.m[0][1] =				xy - wz ;
		m0.m[0][2] =				xz + wy ;
		m0.m[0][3] = 0.0f;

		m0.m[1][0] =				xy + wz ;
		m0.m[1][1] = 1.0 -	xx + zz ;
		m0.m[1][2] =				yz - wx ;
		m0.m[1][3] = 0.0f;

		m0.m[2][0] =				xz - wy ;
		m0.m[2][1] =				yz + wx ;
		m0.m[2][2] = 1.0 -	xx + yy ;
		m0.m[2][3] = 0.0f;

		m0.m[3][0] = 0.0f;
		m0.m[3][1] = 0.0f;
		m0.m[3][2] = 0.0f;
		m0.m[3][3] = 1.0f;
}
#endif
//----------------------------------------------------------
matrix QuatConvertToMatrix(quat q)
//----------------------------------------------------------
{
	matrix	m0;

		float xx = q.x * q.x * 2.0f;
		float yy = q.y * q.y * 2.0f;
		float zz = q.z * q.z * 2.0f;
		float yz = q.y * q.z * 2.0f;
		float xz = q.x * q.z * 2.0f;
		float xy = q.x * q.y * 2.0f;
		float wx = q.w * q.x * 2.0f;
		float wy = q.w * q.y * 2.0f;
		float wz = q.w * q.z * 2.0f;

		m0.m[0][0] = 1.0 - yy + zz;
		m0.m[0][1] =			 xy + wz;
		m0.m[0][2] =			 xz - wy;

		m0.m[1][0] =			 xy - wz;
		m0.m[1][1] = 1.0 - xx + zz;
		m0.m[1][2] =			 yz + wx;

		m0.m[2][0] =			 xz + wy;
		m0.m[2][1] =			 yz - wx;
		m0.m[2][2] = 1.0 - xx + yy;

		m0.m[3][0] = 0.0;
		m0.m[3][1] = 0.0;
		m0.m[3][2] = 0.0;

	return	m0;
}
