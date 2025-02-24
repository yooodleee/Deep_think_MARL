
#ifndef INFOSYSTEM_H_
#define INFOSYSTEM_H_


#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>


#if defined(__APPLE__) && defined(__MACH__)


#include <sys/time.h>
#include <sys/resource.h>


#endif


class InfoSystem {
private:
    static double totalTime;

    static int parseLine(char* line)
    {
        // This assumes that a digit will be found and the line ends in " kb".
        int i = strlen(line);
        const char* p = line;
        while (*p < '0' || *p > '9')
            p++;
        line[i - 3] = '\0';
        i = atoi(p);
        return i;
    }


public:
    /// Return the ram used by the program in KB
    static int getMemUsed()
    {
        #if defined(__APPLE__) && defined(__MACH__)
            struct rusage usage;
            if (0 == getrusage(RUSAGE_SELF, &usage))
                return usage.ru_maxrss / 1000000;   // MB
            else
                return -1;
        #else
            FILE* file = fopen("/proc/self/status", "r");
            int result = -1;
            char line[128];

            while (fgets(line, 128, file) != NULL) {
                if (strcmp(line, "VmRSS:", 6) == 0) {
                    result = parseLine(line);
                    break;
                }
            }
            fclose(file);
            return result;
        #endif
    }

    /// Return total time elapsed
    static inline double timeElapsed() { return (double)clock() / CLOCKS_PER_SEC; }

    /// Return the time elapsed since last call
    static inline double elapsed_seconds()
    {
        double oldTime = totalTime;
        totalTime = timeElapsed();
        return totalTime - oldTime;
    }
};



#endif  // INFOSYSTEM_H_