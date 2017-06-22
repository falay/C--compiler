#pragma symbol off

//bool bugger(float a[10], string b[1][2], int c[2]);

void tester(int y, double zzz[2][3][4]) ;

int func2(int a, float b) ;


int f(int a) ;

int f(int n)
{
	bool FALAY ;
	//int XXX;
	bool XXX = (FALAY == true)  ;

	if( n == 0 )
		return 1 ;
	else 
		return f(n)*n ;
}


int foo(int a[3][3], int b[5][3], int i) 
{
    a[0][0] = i;       // legal
    i = a[1][1];       // legal
    a[0][1] = b[1][2]; // legal
	int k = a[3][i];// legal : note that bounds are not checked
	//i = 3+a[0];// illegal : array arithmetic
	//a=b;// illegal : array assignment
	//a[1] = b[2]; // illegal : array assignment
	return a[0][0];// legal : ’a[0][0]’ is a scalar type, but ’a’ is an array type.
}

//const string d = "1",a="-5", CDF="66";

//float K[5] = {1,2,-3.5,"XD",1} ;


void f1(int a) {print a ;}

void f2(double a[10]){ print "123" ;}

int bug(){int XD = 8;return XD ;}
int f3() ;

int f3()
{
	return 1 + f(1+2+3*5) ;
}

int f4(int a, float b, double c, int d[10]) 
{
    b = a; // a is converted from int to float
    c = a; // a is converted from int to double
  	c = b; // b is converted from float to double
    c = a + b; // a is converted to float before addition,
                        // which produces a float value that is converted to double.
    f1(a); // legal, a is converted from int to float
    c = f3(); // legal, the return type int is converted to double
    f2(d); // legal, d is converted from an array of 10 int’s to an array of 10 double’s
    //a = c; // illegal, a double value cannot be converted to an integer

    bool falay = true ;
    bool test = false ;
	while ( falay == (!falay && false) ) {continue ;}

	// legal, b is converted to double before comparison
    return a; // legal, the return value is implicitly converted to double
}

void main(){

//string K = a ;
	//int XDDDDbug = f4(1,2,3,4) ;
	const int XDD = 5, C = 6 ,Bugger = 7;
	//XDDDDbug = Bugger+5*6-XDD/C ;
	
	//int QAO = 3 % 5.5 ;
	//f1() ;
	int QQ = 1-2-3-4-bug()+1 ;
	
	int XD ;
	
	//XD = (1+Hi-5)*6-XDD ;
	
	int x, aa[1], GARO[2][3], bv[100] ;
	string ZZ, MyName ;
	double y[10], yy[125][10],array[5][1][2] ;
	
	int buggg = 10 ;
	int module = (1+2*buggg+30) % (3+2*1-x);
	
	const bool flag = false ;
	const string bug = "outer 'x' has been hidden in this scope" ;

	
	if(y[0] == 0)
	{
		float x; //outer 'x' has been hidden in this scope
		const double z=0.5;
		int i ; 
		while(i<5){
			int k,a ;
		}
		int k =50;
	}
	
	if( x == y[0]+5 && ( y[1] < 10 || !flag) && x+10 >= y[2] )
		print "Hi" ;
	
}


int QQ(int AAA,int B[2],string C[10][11][2][2][2][2])
{
	int Dog, Eva ;
	return 0 ;
}

void tester(int y, double zzz[2][3][4])
{
	const string QAO = "123" ;
	print QAO ;
}


int func2(int a, float b)
{
	const double myGod = -12E+8;
	string c = "hello world";
	
	return 15 ;
	//return (b <= 1.0);
}