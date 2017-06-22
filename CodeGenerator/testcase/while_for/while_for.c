void main() {
	int i = 1, answer = 0;
    int j,k,w;
	
	while( i <= 1000 ) {
		answer = answer + i;
		
		i = i + 1;
	}
    for(j = 0;j < 3;j = j+1)
    {
        for(k = 0;k < 3;k = k+1)
        {
            for(w = 0;w < 3;w = w+1)
            {
                print "w = ";
                print w;
                print "\n";
            }
            print "k = ";
            print k;
            print "\n";
        }
        print "j = ";
        print j;
        print "\n";
    }
	
	print i;
	print " ";
	print answer;
	print "\n";
}


/*
Input:
No input

Output:
w = 0
w = 1
w = 2
k = 0
w = 0
w = 1
w = 2
k = 1
w = 0
w = 1
w = 2
k = 2
j = 0
w = 0
w = 1
w = 2
k = 0
w = 0
w = 1
w = 2
k = 1
w = 0
w = 1
w = 2
k = 2
j = 1
w = 0
w = 1
w = 2
k = 0
w = 0
w = 1
w = 2
k = 1
w = 0
w = 1
w = 2
k = 2
j = 2
1001 500500
*/
