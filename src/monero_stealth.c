/* Copyright 2017 Cedric Mesnil <cslashm@gmail.com>, Ledger SAS
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
 */

#include "os.h"
#include "cx.h"
#include "monero_types.h"
#include "monero_api.h"
#include "monero_vars.h"
#include "usbd_ccid_impl.h"


/* ----------------------------------------------------------------------- */
/* ---                                                                 --- */
/* ----------------------------------------------------------------------- */
int monero_apdu_stealth() {
    int i ;
    unsigned char Dout[33];
    unsigned char payID[8];
    unsigned char mask[32];
    
    //fetch Aout
    monero_io_fetch(Dout,32);
    //fetch paymentID
    monero_io_fetch(payID,8);

    monero_io_discard(0);

    //Compute Dout
    monero_gerenrate_key_derivation(Dout, Dout, (void*)&G_monero_vstate.r);
    
    //compute mask
    Dout[32] = ENCRYPTED_PAYMENT_ID_TAIL;
    monero_keccak_F(Dout,33,mask);
    
    //stealth!
    for (i=0; i<8;i++) {
        payID[i] = payID[i] ^ mask[i];
    }
    
    monero_io_insert(payID,8);
#ifdef LEDGERDEBUG    
    monero_io_insert(Dout,32);
    monero_io_insert(mask,32);
#endif

    return SW_OK;
}