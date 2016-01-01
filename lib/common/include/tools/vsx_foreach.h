#ifndef VSX_FOREACH_H
#define VSX_FOREACH_H

#define foreach(v,i) for (size_t i = 0; i < v.size(); i++)
#define foreach_p(v,i) for (size_t i = 0; i < v->size(); i++)
#define for_n(n,i) for (size_t i = 0; i < n; i++)

#endif
