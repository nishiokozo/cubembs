#define	pi		3.14159265
#define	deg(n)	((n)*180.0f/M_PI)
#define	rad(n)	((n)*M_PI/180.0f)

//----------------------------------------------------------
class matrix
//----------------------------------------------------------
{
public:
	float	m[4][4];

	matrix();
	matrix(
		float a0, float a1, float a2, float a3,
		float b0, float b1, float b2, float b3,
		float c0, float c1, float c2, float c3,
		float d0, float d1, float d2, float d3
	);

	matrix operator * ( matrix );

	vector operator * ( vector );

	matrix& operator *= ( matrix );

	vector vx();
	vector vy();
	vector vz();
	vector vpos();
	matrix mrot();
};

extern	float	getDistance( 

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
);
//----------------------------------------------------------
class quat
//----------------------------------------------------------
{
public:
	float w;
	float x;
	float y;
	float z;

	quat();
	quat( float w, float x, float y, float z );
	quat operator * ( quat q1 );

};

//----------------------------------------------------------
class loc
//----------------------------------------------------------
{
public:
	float x;
	float y;
	float z;
	float w;

	loc();
	loc( float x, float y, float z, float w );
	loc operator * ( loc l1 );

};

//extern	float fdot( vector a,  vector b  );
//extern	vector vcross( vector a,  vector b  );
//extern	float fscalar( vector a );


//extern	vector	vlen_point_line( vector& P, vector& V );
matrix	mrotx( float n );
matrix	mroty( float n );
matrix	mrotz( float n );
matrix	mscale( float n );
matrix	mscale( float x,float y,float z );
matrix	mident();
matrix	mtrans( float x,  float y, float z );
matrix	minv( matrix m1 );
matrix	mnormal( matrix m0 );
matrix	mnormalz( matrix m0 );
matrix	mnormaly( matrix m0 );
matrix	mnormalz( matrix m0 );

quat	qslerp( quat q1, quat q2, float fAng );
quat	qm( matrix m1 );
matrix	mq( quat q1 );

matrix mrotv( vector v1, float r );

vector	vmulvm( vector b, matrix c );
vector	vmulmv( matrix b, vector c );
