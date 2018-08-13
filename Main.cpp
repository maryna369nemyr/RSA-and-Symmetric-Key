#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

using namespace std;

const int inf = 0x7FFFFFFF;

int PowMod(int x, int n, int mod)
{
   if (n==0)
	   return 1;
	if (n%2==0) {
		long long q = PowMod(x,n/2,mod);
		q = q*q;
		return q%mod;
	}
	else {
		return (x*1ll*PowMod(x,n-1,mod))%mod;
	}
}

int  NSD ( int A, int B)
{
	if(A<0) A=-A;
	if(B<0) B=-B;
    while((A!= 0)&&(B!=0))
    {
        if(A>B) A= A%B;
        else B=B%A;
    }
    return (A+B);
}


long long WideEvklid (long long a, long long b, long long & x, long long & y) {

	if (a == 0) {
		x = 0; y = 1;
		return b;
	}
	long long x1, y1;
	long long d = WideEvklid (b%a, a, x1, y1);
	x = y1 - (b / a) * x1;
	y = x1;
	return d;
}


long long getRandNumber(int max)
{
	return (rand()*1ll*RAND_MAX+rand())%max;
}

class Cryptographer
{
public:
	virtual void encrypt(string filePath, string outFile, string keyFile) = 0;
	virtual void decrypt(string filePath, string outFile, string keyFile) = 0;
};

class SymetricCryptographer : public Cryptographer
{
public:
	void encrypt(string filePath, string outFile, string keyFile) {
		long long key = initKey();

		ifstream in(filePath);
		ofstream out(outFile);
		
		while (in) { 
			string current; 
			getline(in, current);
			for (int i = 0; i < current.length(); ++i) {
				out << int(current[i])+key << " ";
			}
			out << inf << endl;
		}
		in.close();
		out.close();

		// write key
		out.open(keyFile);
		out << key << endl;
		out.close();
	}

	void decrypt(string encryptedFile, string outFile, string keyFile)
	{
		long long key;
		// red key
		ifstream in(keyFile);
		in >> key;
		in.close();
		
		in.open(encryptedFile);
		ofstream out(outFile);
		int encrypted;
		while (in >> encrypted) { 
			if (encrypted == inf) {
				out << endl;
			}
			else {
				out << char(encrypted - key);
			}
		}
		in.close();
		out.close();
	}

	long long initKey()
	{
		return getRandNumber(50);
	}
};



class RSACryptographer : public Cryptographer
{
private:
	long long privateKey;
	long long publicKey;
	long long module;

	bool isPrime(long long x) {
		for (int i = 2; i <= (int)sqrt(x+.0); ++i) {
			if (x%i==0)
				return false;
		}
		return true;
	}



	long long getRandPrime(int max)
	{
		long long x = getRandNumber(max);
		while (!isPrime(x)) {
			x++;
		}
		return x;
	}
	void initKey()
	{
		long long p = getRandPrime(100);
		long long q = getRandPrime(100);
		long long n = p*q;
		long long phi = (p-1)*(q-1);
		long long e = 1+rand()%99;
		while (NSD(e,phi) != 1) {
			e++;
		}
		long long y;
		long long d;
		WideEvklid(e,phi,d,y);
		d = (d % phi + phi) % phi;

		//cout << (d*e) % phi << endl;
		privateKey = d;
		publicKey = e;
		module = n;
	}
public:
	
	void encrypt(string textFile, string encryptedFile, string keyFile) {
		initKey();
		ifstream in(textFile);
		ofstream out(encryptedFile);
		
		while (in) { 
			string current; 
			getline(in, current);
			for (int i = 0; i < current.length(); ++i) {
				int x = current[i]+1000;
				int enc = PowMod(x, publicKey, module);
				out << enc << " ";
			}
			out << inf << endl;
		}
		in.close();
		out.close();

		// write key
		out.open(keyFile);
		out << publicKey << endl;
		out << privateKey << endl;
		out << module << endl;
		out.close();
	}

	void decrypt(string encryptedFile, string outFile, string keyFile)
	{
		ifstream in(keyFile);
		in >> publicKey >> privateKey >> module;
		in.close();
		
		in.open(encryptedFile);
		ofstream out(outFile);
		int encrypted;
		while (in >> encrypted) {
			if (encrypted == inf)
				out << endl;
			else {
				int x = PowMod(encrypted,privateKey,module);
				out << char(x-1000);
			}
		}
		in.close();
		out.close();
	}
private:
	
};


int main()
{
	srand(time(0));
	
	Cryptographer* cryptographer = new SymetricCryptographer();
	cryptographer->encrypt("text.txt", "encryptedSym.txt", "keySym.txt");
	cryptographer->decrypt("encryptedSym.txt", "outputSym.txt", "keySym.txt");
	delete cryptographer;
	
	cryptographer = new RSACryptographer();
	cryptographer->encrypt("text.txt", "encryptedRSA.txt", "keyRSA.txt");
	cryptographer->decrypt("encryptedRSA.txt", "outputRSA.txt", "keyRSA.txt");



	return 0;
}