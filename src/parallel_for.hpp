#include <algorithm>
#include <thread>
#include <functional>
#include <vector>
#include <mutex>
#include <iostream>

#ifndef THREADS_BY_CORE
#define THREADS_BY_CORE 1
#endif

/// @param[in] nb_elements : size of your for loop
/// @param[in] functor(start, end) :
/// your function processing a sub chunk of the for loop.
/// "start" is the first index to process (included) until the index "end"
/// (excluded)
/// @code
///     for(int i = start; i < end; ++i)
///         computation(i);
/// @endcode
/// @param use_threads : enable / disable threads.
///
///
static
void parallel_for(unsigned nb_elements,
                  unsigned nb_threads,
                  std::function<void (int start, int end)> functor,
                  bool use_threads = true)
{
    // -------
    // unsigned nb_threads_hint = THREADS_BY_CORE * std::thread::hardware_concurrency();
    // // unsigned nb_threads = nb_threads_hint == 0 ? 8 : (nb_threads_hint);
    // unsigned nb_threads = 2;

    unsigned batch_size = nb_elements / nb_threads;
    unsigned batch_remainder = nb_elements % nb_threads;

    std::vector< std::thread > my_threads(nb_threads);

    if( use_threads )
    {
        // Multithread execution
        for(unsigned i = 0; i < nb_threads; ++i)
        {
            int start = i * batch_size;
            my_threads[i] = std::thread(functor, start, start+batch_size);

            // Create a cpu_set_t object representing a set of CPUs. Clear it and mark
            // only CPU i as set.
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(i / THREADS_BY_CORE, &cpuset);
            int rc = pthread_setaffinity_np(my_threads[i].native_handle(),
                                            sizeof(cpu_set_t), &cpuset);
            if (rc != 0)
            {
                std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
            }

            
        }
    }
    else
    {
        // Single thread execution (for easy debugging)
        for(unsigned i = 0; i < nb_threads; ++i){
            int start = i * batch_size;
            functor( start, start+batch_size );
        }
    }

    // Deform the elements left
    int start = nb_threads * batch_size;
    functor( start, start+batch_remainder);

    // Wait for the other thread to finish their task
    if( use_threads )
        std::for_each(my_threads.begin(), my_threads.end(), std::mem_fn(&std::thread::join));
}