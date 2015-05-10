/* Suppose we have the following code:
 *
 * ~~~
 * class Foo {
 * public:
 *     Foo() { ... }
 *     void first(void) { ... }
 *     void second(void) { ... }
 *     void third(void) { ... }
 * };
 * ~~~
 *
 * The same instance of `Foo` will be passed to three different threads. `ThreadA` will
 * call `first()`, `threadB` will call `second()`, and `threadC` will call `third()`. Design a
 * mechanism to ensure that `first` is called before `second` and `second` is called
 * before `third`.
 *
 * Source: Cracking the Coding Interview, page 160, question 16.5 (Code adapted to C++)
 */
#include <iostream>
#include <thread>
#include <semaphore.h>
#include <system_error>

using namespace std;

class Foo {
private:
	sem_t semaphores[2];
public:
	Foo() {
		int err = 0;
		size_t i;
		for (i = 0; i < sizeof(semaphores)/sizeof(semaphores[0]) && err == 0; i++) {
			err = sem_init(&semaphores[i], 0, 1);
			if (err == 0) {
				sem_wait(&semaphores[i]);
			}
		}
		if (err != 0) {
			for (size_t j = 0; j < i; j++) {
				sem_destroy(&semaphores[j]);
			}
			throw system_error(EFAULT, system_category());
		}
	}

	void first(void) {
		cout << "Executing first()..." << endl;
		sem_post(&semaphores[0]);
	}

	void second(void) {
		sem_wait(&semaphores[0]);
		cout << "Executing second()..." << endl;
		sem_post(&semaphores[0]);
		sem_post(&semaphores[1]);
	}

	void third(void) {
		sem_wait(&semaphores[1]);
		cout << "Executing third()..." << endl;
		sem_post(&semaphores[1]);
	}

	~Foo() {
		for (size_t i = 0; i < sizeof(semaphores)/sizeof(semaphores[0]); i++) {
			sem_destroy(&semaphores[i]);
		}
	}
};

void thread1(Foo *foo) {
	foo->first();
}

void thread2(Foo *foo) {
	foo->second();
}

void thread3(Foo *foo) {
	foo->third();
}

int main(void) {
	Foo foo;

	thread first(thread1, &foo);
	thread second(thread2, &foo);
	thread third(thread3, &foo);

	first.join();
	second.join();
	third.join();

	cout << "All threads executed, exiting now..." << endl;

	return 0;
}
