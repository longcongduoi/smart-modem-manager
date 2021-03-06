/*
 * Copyright (c) 2016, Intel Corporation, Intel Mobile Communication
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <smm_core/smm_log.h>
#include <smm_core/AtResponse.h>
#include <smm_core/AtResponseParser.h>
#include <smm_core/DcContext.h>
#include <smm_core/NetregContext.h>
#include <smm_core/SharedEnvInterface.h>
#include "AtsampleAtResponseParser.h"

AtsampleAtResponseParser::AtsampleAtResponseParser(SharedEnvInterface* env)
    : AtResponseParser()
{
    assert(env != nullptr);
    mEnv = env;

    registerMessage(new CCIDResponse(env));
}

AtsampleAtResponseParser::CCIDResponse::CCIDResponse(SharedEnvInterface* env)
    : AtResponse("+CCID")
{
    assert(env != nullptr);
    mEnv = env;
}

AtsampleAtResponseParser::CCIDResponse::~CCIDResponse()
{
    if (mCcid != nullptr) {
        delete mCcid;
        mCcid = nullptr;
    }
}

bool AtsampleAtResponseParser::CCIDResponse::executeAsReply(OutputWriterInterface* output, int argc,
                                                         const char** argv)
{
    // +CCID: <ccid>
    if (errorIfArgsCountLessThan(1, argc, output)) {
        return false;
    }

    mCcid = strdup(argv[0]);

    SMM_TRACE_D("CCIDResponse { ccid = %s }", mCcid);

    // Translating back into ^SCID only is SCID command is pending
    SmmMessage* pendingCommand = mEnv->dequeuePendingMessage();
    if (pendingCommand != nullptr) {
        return pendingCommand->onSuccess(this, output);
    } else {
        SMM_TRACE_E("No message to dequeue ?");
    }
    // we have not consumed this response
    return false;
}
