// Copyright (c) 2019, The Monero Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "defyx.h"
#include "c_threads.h"
#include "hash-ops.h"

#if defined(_MSC_VER)
#define THREADV __declspec(thread)
#else
#define THREADV __thread
#endif

typedef struct dx_state {
  CTHR_MUTEX_TYPE rs_mutex;
  char rs_hash[HASH_SIZE];
  uint64_t  rs_height;
  defyx_cache *rs_cache;
} dx_state;

static CTHR_MUTEX_TYPE dx_mutex = CTHR_MUTEX_INIT;
static CTHR_MUTEX_TYPE dx_dataset_mutex = CTHR_MUTEX_INIT;

static dx_state dx_s[2] = {{CTHR_MUTEX_INIT,{0},0,0},{CTHR_MUTEX_INIT,{0},0,0}};

static defyx_dataset *dx_dataset;
static uint64_t dx_dataset_height;
static THREADV defyx_vm *dx_vm = NULL;

static void local_abort(const char *msg)
{
  fprintf(stderr, "%s\n", msg);
#ifdef NDEBUG
  _exit(1);
#else
  abort();
#endif
}

/**
 * @brief uses cpuid to determine if the CPU supports the AES instructions
 * @return true if the CPU supports AES, false otherwise
 */

static inline int force_software_aes(void)
{
  static int use = -1;

  if (use != -1)
    return use;

  const char *env = getenv("MONERO_USE_SOFTWARE_AES");
  if (!env) {
    use = 0;
  }
  else if (!strcmp(env, "0") || !strcmp(env, "no")) {
    use = 0;
  }
  else {
    use = 1;
  }
  return use;
}

static void cpuid(int CPUInfo[4], int InfoType)
{
#if defined(__x86_64__)
    __asm __volatile__
    (
    "cpuid":
        "=a" (CPUInfo[0]),
        "=b" (CPUInfo[1]),
        "=c" (CPUInfo[2]),
        "=d" (CPUInfo[3]) :
            "a" (InfoType), "c" (0)
        );
#endif
}
static inline int check_aes_hw(void)
{
#if defined(__x86_64__)
    int cpuid_results[4];
    static int supported = -1;

    if(supported >= 0)
        return supported;

    cpuid(cpuid_results,1);
    return supported = cpuid_results[2] & (1 << 25);
#else
    return 0;
#endif
}

static volatile int use_dx_jit_flag = -1;

static inline int use_dx_jit(void)
{
#if defined(__x86_64__)

  if (use_dx_jit_flag != -1)
    return use_dx_jit_flag;

  const char *env = getenv("MONERO_USE_RX_JIT");
  if (!env) {
    use_dx_jit_flag = 1;
  }
  else if (!strcmp(env, "0") || !strcmp(env, "no")) {
    use_dx_jit_flag = 0;
  }
  else {
    use_dx_jit_flag = 1;
  }
  return use_dx_jit_flag;
#else
  return 0;
#endif
}

#define SEEDHASH_EPOCH_BLOCKS	2048	/* Must be same as BLOCKS_SYNCHRONIZING_MAX_COUNT in cryptonote_config.h */
#define SEEDHASH_EPOCH_LAG		64

void dx_reorg(const uint64_t split_height) {
  int i;
  CTHR_MUTEX_LOCK(dx_mutex);
  for (i=0; i<2; i++) {
    if (split_height < dx_s[i].rs_height)
      dx_s[i].rs_height = 1;	/* set to an invalid seed height */
  }
  CTHR_MUTEX_UNLOCK(dx_mutex);
}

uint64_t dx_seedheight(const uint64_t height) {
  uint64_t s_height =  (height <= SEEDHASH_EPOCH_BLOCKS+SEEDHASH_EPOCH_LAG) ? 0 :
                       (height - SEEDHASH_EPOCH_LAG - 1) & ~(SEEDHASH_EPOCH_BLOCKS-1);
  return s_height;
}

void dx_seedheights(const uint64_t height, uint64_t *seedheight, uint64_t *nextheight) {
  *seedheight = dx_seedheight(height);
  *nextheight = dx_seedheight(height + SEEDHASH_EPOCH_LAG);
}

typedef struct seedinfo {
  defyx_cache *si_cache;
  unsigned long si_start;
  unsigned long si_count;
} seedinfo;

static CTHR_THREAD_RTYPE dx_seedthread(void *arg) {
  seedinfo *si = arg;
  defyx_init_dataset(dx_dataset, si->si_cache, si->si_start, si->si_count);
  CTHR_THREAD_RETURN;
}

static void dx_initdata(defyx_cache *rs_cache, const int miners, const uint64_t seedheight) {
  if (miners > 1) {
    unsigned long delta = defyx_dataset_item_count() / miners;
    unsigned long start = 0;
    int i;
    seedinfo *si;
    CTHR_THREAD_TYPE *st;
    si = malloc(miners * sizeof(seedinfo));
    if (si == NULL)
      local_abort("Couldn't allocate RandomX mining threadinfo");
    st = malloc(miners * sizeof(CTHR_THREAD_TYPE));
    if (st == NULL) {
      free(si);
      local_abort("Couldn't allocate RandomX mining threadlist");
    }
    for (i=0; i<miners-1; i++) {
      si[i].si_cache = rs_cache;
      si[i].si_start = start;
      si[i].si_count = delta;
      start += delta;
    }
    si[i].si_cache = rs_cache;
    si[i].si_start = start;
    si[i].si_count = defyx_dataset_item_count() - start;
    for (i=1; i<miners; i++) {
      CTHR_THREAD_CREATE(st[i], dx_seedthread, &si[i]);
    }
    defyx_init_dataset(dx_dataset, rs_cache, 0, si[0].si_count);
    for (i=1; i<miners; i++) {
      CTHR_THREAD_JOIN(st[i]);
    }
    free(st);
    free(si);
  } else {
    defyx_init_dataset(dx_dataset, rs_cache, 0, defyx_dataset_item_count());
  }
  dx_dataset_height = seedheight;
}

void dx_slow_hash(const uint64_t mainheight, const uint64_t seedheight, const char *seedhash, const void *data, size_t length,
  char *hash, int miners, int is_alt) {
  uint64_t s_height = dx_seedheight(mainheight);
  int toggle = (s_height & SEEDHASH_EPOCH_BLOCKS) != 0;
  defyx_flags flags = RANDOMX_FLAG_DEFAULT;
  dx_state *dx_sp;
  defyx_cache *cache;

  CTHR_MUTEX_LOCK(dx_mutex);

  if (is_alt) {
    if (s_height == seedheight && !memcmp(dx_s[toggle].rs_hash, seedhash, HASH_SIZE))
      is_alt = 0;
  } else {

  /* RPC could request an earlier block on mainchain */
    if (s_height > seedheight)
      is_alt = 1;
    /* miner can be ahead of mainchain */
    else if (s_height < seedheight)
      toggle ^= 1;
  }

  toggle ^= (is_alt != 0);

  dx_sp = &dx_s[toggle];
  CTHR_MUTEX_LOCK(dx_sp->rs_mutex);
  CTHR_MUTEX_UNLOCK(dx_mutex);

  cache = dx_sp->rs_cache;
  if (cache == NULL) {
    if (use_dx_jit())
      flags |= RANDOMX_FLAG_JIT;
    if (cache == NULL) {
      cache = defyx_alloc_cache(flags | RANDOMX_FLAG_LARGE_PAGES);
      if (cache == NULL) {
        cache = defyx_alloc_cache(flags);
      }
      if (cache == NULL)
        local_abort("Couldn't allocate RandomX cache");
    }
  }
  if (dx_sp->rs_height != seedheight || dx_sp->rs_cache == NULL || memcmp(seedhash, dx_sp->rs_hash, HASH_SIZE)) {
    defyx_init_cache(cache, seedhash, HASH_SIZE);
    dx_sp->rs_cache = cache;
    dx_sp->rs_height = seedheight;
    memcpy(dx_sp->rs_hash, seedhash, HASH_SIZE);
  }
  if (dx_vm == NULL) {
    defyx_flags flags = RANDOMX_FLAG_DEFAULT;
    if (use_dx_jit()) {
      flags |= RANDOMX_FLAG_JIT;
    }
    if(!force_software_aes() && check_aes_hw())
      flags |= RANDOMX_FLAG_HARD_AES;
    if (miners) {
      CTHR_MUTEX_LOCK(dx_dataset_mutex);
      if (dx_dataset == NULL) {
        dx_dataset = defyx_alloc_dataset(RANDOMX_FLAG_LARGE_PAGES);
        if (dx_dataset == NULL) {
          dx_dataset = defyx_alloc_dataset(RANDOMX_FLAG_DEFAULT);
        }
        if (dx_dataset != NULL)
          dx_initdata(dx_sp->rs_cache, miners, seedheight);
      }
      if (dx_dataset != NULL)
        flags |= RANDOMX_FLAG_FULL_MEM;
      else {
        miners = 0;
      }
      CTHR_MUTEX_UNLOCK(dx_dataset_mutex);

    }

    dx_vm = defyx_create_vm(flags | RANDOMX_FLAG_LARGE_PAGES, dx_sp->rs_cache, dx_dataset);
    if(dx_vm == NULL) { //large pages failed
      dx_vm = defyx_create_vm(flags, dx_sp->rs_cache, dx_dataset);
    }
    if(dx_vm == NULL) {//fallback if everything fails
      flags = RANDOMX_FLAG_DEFAULT | (miners ? RANDOMX_FLAG_FULL_MEM : 0);
      dx_vm = defyx_create_vm(flags, dx_sp->rs_cache, dx_dataset);
    }
    if (dx_vm == NULL)
      local_abort("Couldn't allocate RandomX VM");
  } else if (miners) {
    CTHR_MUTEX_LOCK(dx_dataset_mutex);
    if (dx_dataset != NULL && dx_dataset_height != seedheight)
      dx_initdata(cache, miners, seedheight);
    CTHR_MUTEX_UNLOCK(dx_dataset_mutex);
  } else {
    defyx_vm_set_cache(dx_vm, dx_sp->rs_cache);
  }
  /* mainchain users can run in parallel */
  if (!is_alt)
    CTHR_MUTEX_UNLOCK(dx_sp->rs_mutex);
  defyx_calculate_hash(dx_vm, data, length, hash);
  /* altchain slot users always get fully serialized */
  if (is_alt)
    CTHR_MUTEX_UNLOCK(dx_sp->rs_mutex);
}

void dx_slow_hash_allocate_state(void) {
}

void dx_slow_hash_free_state(void) {
  if (dx_vm != NULL) {
    defyx_destroy_vm(dx_vm);
    dx_vm = NULL;
  }
}

void dx_stop_mining(void) {
  CTHR_MUTEX_LOCK(dx_dataset_mutex);
  if (dx_dataset != NULL) {
    defyx_dataset *rd = dx_dataset;
    dx_dataset = NULL;
    defyx_release_dataset(rd);
  }
  CTHR_MUTEX_UNLOCK(dx_dataset_mutex);
}
