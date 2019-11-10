#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <set>
#include <stack>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <gmpxx.h>

using namespace std;

typedef mpz_class uint;

const uint maxInput = 10000;

template<typename T>
void print(const list<T>& items) {
	for (T item : items) {
		cout << item << " ";
	}
	cout << endl;
}

class ContinuedFraction {
	public:
		ContinuedFraction(const uint i_front, const ContinuedFraction* i_continuation) : front(i_front), continuation(i_continuation) {
		}

		ContinuedFraction(const uint i_front) : ContinuedFraction(i_front, NULL) {}

		bool hasContinuation() const {
			return continuation != NULL;
		}

		operator string() const {
			stringstream ss;
			ss << numerator() << "/" << denominator();
			return ss.str();
		}

		~ContinuedFraction() {
			if (hasContinuation()) {
				delete continuation;
			}
		}

        //TODO: cache these values
		const uint numerator() const {
            return hasContinuation() ? front * continuation->numerator() + continuation->denominator() :  front;
		}

		const uint denominator() const {
			return hasContinuation() ?  continuation->numerator() : 1;
		}

	private:
		const uint front;
		const ContinuedFraction* continuation;
};

template<typename T>
bool setContains(const set<T> s, const T val) {
    return s.find(val) != s.end();
}

void populateWithPrimes(set<uint>& o_primes) {
    ifstream primeReader("primes.txt");
    while (!primeReader.eof()) {
        uint num;
        primeReader >> num;
        o_primes.insert(num);
    }
    primeReader.close();
}

void factorNaive(const uint num, const set<uint>& primes, list<uint>& o_factors) {
    if (num == 1) {
        return;
    }

    for (uint factorCandidate : primes) {
        if (num % factorCandidate == 0) {
            o_factors.push_back(factorCandidate);
            factorNaive(num / factorCandidate, primes, o_factors);
            break;
        }
    }
}

const uint factorCandidate(const uint num, const set<uint>& primes, const bool skip = false, const int skip_prob = 1) {
    uint product = 1;

    for (set<uint>::iterator it = primes.begin(); it != primes.end() && *it < num; ++it) {
        if (!skip || rand() % skip_prob == 0) {
            product *= *it;
        }
    }

    return product;
}

void factorCF(const uint num, const set<uint>& primes, list<uint>& o_factors, const bool skip = false, const int skip_prob = 1) {
    if (num == 1) {
        return;
    }

    if (setContains(primes, num)) {
        o_factors.push_back(num);
        return;
    }

    stack<uint> quotients;

    const uint primeProduct = factorCandidate(num, primes, skip, skip_prob);

    uint top = primeProduct;
    uint bottom = num;
    while (bottom > 0) {
        quotients.push(top / bottom);
        uint rem = top % bottom;
        top = bottom;
        bottom = rem;
    }

    ContinuedFraction* outerFraction = new ContinuedFraction(quotients.top());
    quotients.pop();

    while (quotients.size() > 0) {
        outerFraction = new ContinuedFraction(quotients.top(), outerFraction);
        quotients.pop();
    }

    uint quotient = outerFraction->denominator();
    uint divisor = num / quotient;

    delete outerFraction;

    if (quotient == 1) {
        factorCF(divisor, primes, o_factors, true, skip_prob + 1);
    } else {
        factorCF(quotient, primes, o_factors, false);
        factorCF(divisor, primes, o_factors, false);
    }
}

int main() {
    cout << "Generating primes..." << endl;
	set<uint> primes;
	populateWithPrimes(primes);
    cout << "Generated primes!" << endl;

    while (true) {
        cout << "Enter a number to factor: ";
        uint toFactor;
        cin >> toFactor;

        list<uint> factors;
        factorCF(toFactor, primes, factors);
        print(factors);
    }

	return 0;
}
