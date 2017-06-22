#pragma symbol off


bool func(float a, double b)
{
	string c = "hello world";
	return true ;//(b <= 1.0);
}

void f4(int x, int z[5][5][5], int t[4]) 
{
	x = f1(z[0][1]); // legal
	x = f1(t); // illegal : size mismatch
	x = f1(z[0][0][1]); // illegal
	x = f2(z[0][0][1]); // legal
	x = f3(t); // legal : type coercion (see below)
}

//bool func(int a, float b);
//int func2(int a, int b) ;
const int d = 1;

void noDefined(int a[5][100], string c, float k)
{
	print "Hi" ;
	test = "QQ" ;
}

int foo(int a[3][3], int b[5][3], int i) 
{
	return 1 ;
	//a[0][0] = i; // legal
	//i = a[1][1]; // legal
	//a[0][1] = b[1][2]; // legal
	//int k = a[3][i]; // legal : note that bounds are not checked
	//i = 3+a[0]; // illegal : array arithmetic
	//a = b; // illegal : array assignment
	//a[1] = b[2]; // illegal : array assignment
	//return a[0][0]; // legal : 'a[0][0]' is a scalar type, but 'a' is an array type.
}


void main()
{
	
	
	
	
	/*
	int x = 123;
	int y[10];

	const string XD = "123" ;
	
	XD = "124" ;

	string a[2][2] = {"1","2","3","7","8"};
	
	int GAAO[5][5][100] = {1};
	
	noDefined(GAAO[1], "String", 3) ;
	func(123e5,5) ;
	
	int x = -5;

	if(y[0] == 0)
	{
		float x; //outer 'x' has been hidden in this scope
		double z;
		z = 0.1;
	}
	*/
}




