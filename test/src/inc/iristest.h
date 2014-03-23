/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris-test.h

Abstract:

    This file contains the common includes the test suite.

--*/

#ifndef _IRIS_TEST_HEADER_
#define _IRIS_TEST_HEADER_

#include <UnitTest++.h>
#include <iostream>
#include <cstdio>
#include <cmath>

extern "C" {
    #include <iristoolkit.h>
}

#include "utility.h"
#include "pfm.h"

using namespace std;

#endif // _IRIS_TEST_HEADER_