#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <set>
#include <stack>

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

        //TODO: cache these
		const uint numerator() const {
			if (hasContinuation()) {
				return front * continuation->numerator() + continuation->denominator();
			} else {
				return front;
			}
		}

		const uint denominator() const {
			if (hasContinuation()) {
				return continuation->numerator();
			} else {
				return 1;
			}
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

void populateWithPrimeProducts(const set<uint>& primes, set<uint>& o_primeProducts) {
    uint totalProduct = 1;
    for (uint prime : primes) {
        totalProduct *= prime;
        o_primeProducts.insert(totalProduct);

        if (totalProduct > maxInput) {
            return;
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

template<typename T>
T nextBiggestElement(const set<T> items, const T item) {
    return *items.lower_bound(item);
}

void factorCF(const uint num, const set<uint>& primes, const set<uint>& primeProducts, list<uint>& o_factors) {
    if (num == 1) {
        return;
    }

    if (setContains(primes, num)) {
        o_factors.push_back(num);
        return;
    }

    stack<uint> quotients;

    uint top = num;
    uint bottom = nextBiggestElement(primeProducts, num);
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

    factorCF(outerFraction->numerator(), primes, primeProducts, o_factors);
    factorCF(num / outerFraction->numerator(), primes, primeProducts, o_factors);

    delete outerFraction;
}

int main() {
    cout << "Generating primes..." << endl;
	set<uint> primes;
	populateWithPrimes(primes);
    cout << "Generated primes!" << endl;

    cout << "Generating prime products..." << endl;
    set<uint> primeProducts;
    populateWithPrimeProducts(primes, primeProducts);
    cout << "Generated prime products! Let's roll!" << endl;

	list<uint> factors;
	factorCF(3392, primes, primeProducts, factors);
	print(factors);

	return 0;
}
