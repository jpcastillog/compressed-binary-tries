#include <algorithm>
#include <thread>
#include <functional>
#include <vector>
#include <mutex>
#include <iostream>
#include <sys/sysctl.h>
#include <mach/thread_policy.h>
#include <mach/thread_act.h>

#ifndef THREADS_BY_CORE
#define THREADS_BY_CORE 1
#define SYSCTL_CORE_COUNT   "machdep.cpu.core_count"
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
// typedef struct cpu_set {
//   uint32_t    count;
// } cpu_set_t;

// static inline void
// CPU_ZERO(cpu_set_t *cs) { cs->count = 0; }

// static inline void
// CPU_SET(int num, cpu_set_t *cs) { cs->count |= (1 << num); }

// static inline int
// CPU_ISSET(int num, cpu_set_t *cs) { return (cs->count & (1 << num)); }


// int sched_getaffinity(pid_t pid, size_t cpu_size, cpu_set_t *cpu_set)
// {
//   int32_t core_count = 0;
//   size_t  len = sizeof(core_count);
//   int ret = sysctlbyname(SYSCTL_CORE_COUNT, &core_count, &len, 0, 0);
//   if (ret) {
//     printf("error while get core count %d\n", ret);
//     return -1;
//   }
//   cpu_set->count = 0;
//   for (int i = 0; i < core_count; i++) {
//     cpu_set->count |= (1 << i);
//   }

//   return 0;
// }

// int pthread_setaffinity_np(pthread_t thread, size_t cpu_size,
//                            cpu_set_t *cpu_set)
// {
//   thread_port_t mach_thread;
//   int core = 0;

//   for (core = 0; core < 8 * cpu_size; core++) {
//     if (CPU_ISSET(core, cpu_set)) break;
//   }
// //   printf("binding to core %d\n", core);
//   thread_affinity_policy_data_t policy = { core };
//   mach_thread = pthread_mach_thread_np(thread);
//   thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY,
//                     (thread_policy_t)&policy, 1);
//   return 0;
// }

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