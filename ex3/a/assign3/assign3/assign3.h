/*
 *  assign3.h
 *  assign3
 *
 *  Created by Jerry Webb on 11/3/14.
 *  Copyright (c) 2014 Jerry Webb. All rights reserved.
 *
 */

extern "C" {
#include <CoreFoundation/CoreFoundation.h>

#pragma GCC visibility push(default)

/* External interface to the assign3, C-based */

CFStringRef assign3UUID(void);

#pragma GCC visibility pop
}
