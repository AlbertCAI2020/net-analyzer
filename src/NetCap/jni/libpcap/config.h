#ifndef _CONFIG_H_
#define _CONFIG_H_


#define HAVE_STRLCPY 1
#define HAVE_STRERROR 1
#define HAVE_LIMITS_H 1
#define HAVE_SNPRINTF 1
#define HAVE_VSNPRINTF 1

#define __aligned_u64 __u64 __attribute__((aligned(8)))
#define __aligned_be64 __be64 __attribute__((aligned(8)))
#define __aligned_le64 __le64 __attribute__((aligned(8)))


#endif /* _CONFIG_H_ */