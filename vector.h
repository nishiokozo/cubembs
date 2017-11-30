//----------------------------------------------------------
class vector
//----------------------------------------------------------
{
public:
	union
	{
		float x;
		float r;
	};
	union
	{
		float y;
		float g;
	};
	union
	{
		float z;
		float b;
	};
	union
	{
		float w;
		float a;
	};

	vector();
	vector( float x, float y, float z );
	vector( float x, float y, float z, float w );
	
	vector& operator += (  vector );
	vector& operator -= (  vector );
	vector& operator *= ( float );
	vector& operator /= ( float );

	vector operator + () ;
	vector operator - () ;

	vector operator + (  vector ) ;
	vector operator - (  vector ) ;
	vector operator * ( float ) ;
	vector operator / ( float ) ;

	friend vector operator * ( float,  class vector );

	int operator == (  vector ) ;
	int operator != (  vector ) ;

	vector& operator = ( float );
	vector& operator = ( vector );

//	vector operator * ( vector v, class matrix a);
	vector operator * ( class matrix );
//	vector& operator *= ( matrix );
//	friend vector operator * ( vector, class matrix );

	float	flen();

};

extern	float fdot( vector a,  vector b  );
extern	vector vcross( vector a,  vector b  );
extern	float fscalar( vector a );

vector vnormal( vector v1  );
float flen( vector v1  );
