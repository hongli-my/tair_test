/* 
 * GST	time
#include <stdio.h>
#include <time.h>

int main()
{
	time_t t;
	struct tm *p;
	t=1408413451;
	p=gmtime(&t);
	char s[80];
	strftime(s, 80, "%Y-%m-%d %H:%M:%S::%Z", p);
	printf("%d : %s\n", (int)t, s);
	return 0;
}
*/
//  CST	time
#include <stdio.h>
#include <time.h>

int main()
{    
    time_t t;
    struct tm *p;
    t=1408413451;
    p=gmtime(&t);
    char s[80];
    strftime(s, 80, "%Y-%m-%d %H:%M:%S::%Z", p);
    printf("%d: %s\n", (int)t, s);

    t=1408413451 + 28800;
    p=gmtime(&t);
    strftime(s, 80, "%Y-%m-%d %H:%M:%S", p);
    printf("%d : %s\n", (int)t, s);
    return 0;
}
