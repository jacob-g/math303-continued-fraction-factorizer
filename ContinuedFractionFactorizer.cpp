#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <set>
#include <stack>
#include <cstdlib>

using namespace std;

typedef unsigned long long int uint;

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
    set<uint> composites;
    for (uint primeCandidate = 2; primeCandidate < maxInput; primeCandidate++) {
        if (!setContains(composites, primeCandidate)) {
            o_primes.insert(primeCandidate);
            for (uint composite = primeCandidate * 2; composite < maxInput; composite += primeCandidate) {
                composites.insert(composite);
            }
        }
    }
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

const uint factorCandidate(const uint num, const set<uint>& primes) {
    uint product = 1;
    for (uint prime : primes) {
        if (rand() % prime == 0) {
            product *= prime;
            if (product > num * num * num) {
                return product;
            }
        }
    }

    return 0;
}

void factorCF(const uint num, const set<uint>& primes, list<uint>& o_factors) {
    if (num == 1) {
        return;
    }

    if (setContains(primes, num)) {
        o_factors.push_back(num);
        return;
    }

    stack<uint> quotients;

    const uint primeProduct = factorCandidate(num, primes);

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

    factorCF(outerFraction->denominator(), primes, o_factors);
    factorCF(num / outerFraction->denominator(), primes, o_factors);


    delete outerFraction;
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
