#include <iostream>

using namespace std;

int recursiveSum(int n)
{
	if (n == 1)
		return 1;
	return n + recursiveSum(n-1);
}

int main()
{
	int n=0;
	cout << "����? : ";
	cin >> n;
	cout << "SUM : " << recursiveSum(n) << endl;
	
	return 0;
}