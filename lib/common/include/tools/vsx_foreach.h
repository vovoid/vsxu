#ifndef VSX_FOREACH_H
#define VSX_FOREACH_H

#define foreach(v,i) for (size_t i = 0; i < v.size(); i++)
#define foreach_p(v,i) for (size_t i = 0; i < v->size(); i++)
#define for_n(i, start, max) for (size_t i = start; i < max; i++)
#define forever for(;;)

#endif
