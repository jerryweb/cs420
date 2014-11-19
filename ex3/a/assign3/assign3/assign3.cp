/*
 *  assign3.cp
 *  assign3
 *
 *  Created by Jerry Webb on 11/3/14.
 *  Copyright (c) 2014 Jerry Webb. All rights reserved.
 *
 */

#include "assign3.h"
#include "assign3Priv.h"

CFStringRef assign3UUID(void)
{
	Cassign3* theObj = new Cassign3;
	return theObj->UUID();
}

CFStringRef Cassign3::UUID()
{
	return CFSTR("0001020304050607");
}
