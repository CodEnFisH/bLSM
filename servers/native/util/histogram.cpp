/*
 * histogram.cpp
 *
 * Copyright 2010-2012 Yahoo! Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *  Created on: Mar 2, 2010
 *      Author: sears
 */

#include "../tcpclient.h"
#include "../network.h"
#include "../datatuple.h"

void usage(char * argv[]) {
	fprintf(stderr, "usage %s [n] [host [port]]\n", argv[0]);
}
#include "util_main.h"
int main(int argc, char * argv[]) {
	unsigned long long count = 3;
	if(argc > 1) {
		char * end;
		unsigned long long n = strtoull(argv[1], &end, 10);
		if(argv[1][0] && !*end) {
			count = n;
			argc--;
			argv++;
		}
	}
	logstore_handle_t * l = util_open_conn(argc, argv);

//    datatuple * ret
    uint8_t rcode = logstore_client_op_returns_many(l, OP_STAT_HISTOGRAM, NULL, NULL, count);

    if(opiserror(rcode)) {
    	fprintf(stderr, "Histogram request returned logstore error code %d\n", rcode);
    	perror("Histogram generation failed."); return 3;
    } else {
    	dataTuple *ret;
    	bool first = true;
    	while(( ret = logstore_client_next_tuple(l) )) {
    		if(first) {
    			assert(ret->rawkeylen() == sizeof(uint64_t));
    			uint64_t stride = *(uint64_t*)ret->rawkey();
    			printf("Stride: %lld\n", (long long)stride);
    			first = false;
    		} else {
    			assert(ret->strippedkey()[ret->strippedkeylen()-1] == 0); // check for null terminator.
    			printf("\t%s\n", (char*)ret->strippedkey());
    		}
    		dataTuple::freetuple(ret);
    	}
    }

    logstore_client_close(l);
    return 0;
}
