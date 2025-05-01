/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define PACK(size, alloc) ((size) | (alloc))

#define GET(p)          (*(unsigned int *)(p))
#define PUT(p, val)     (*(unsigned int *)(p) = (val))

#define GET_SIZE(p)     (GET(p) & ~0x7)
#define GET_ALLOC(p)    (GET(p) & 0x1)

#define HDRP(bp)        ((char *)(bp) - WSIZE)
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

#define PRED(bp)        ((char *)(bp))
#define SUCC(bp)        (*(char **)(bp)+WSIZE)

static char* next_point_bp;
static char* heap_listp;
/* LIFO 방식으로써 
*/
// static char* first_free;
// static char* last_free;

// static void add_fake_free_block(void* bp){

//     PUT(bp + (1*WSIZE), PACK(2*DSIZE, 0));
//     PUT(bp + (2*WSIZE), 0); // 이전 가용 블록
//     PUT(bp + (3*WSIZE), 0); // 다음 가용 블록
//     PUT(bp + (4*WSIZE), PACK(2*DSIZE, 0));
// }

// void new_free_block(void *bp){
//     // head가 null일때랑 아닐때 분기 나누기
//     // PUT(SUCC(bp), 0);
//     // PUT(PRED(bp), PRED(first_free));
//     // PUT(PRED(first_free), bp);
//     // PUT(SUCC(PRED(bp)), bp);
//     PUT(SUCC(bp), first_free);
//     PUT(PRED(bp), 0);
//     PUT(PRED(first_free), bp);
//     first_free = bp;
// }

// void remove_free_block(void *bp){
//     if(bp == first_free){
//         first_free = SUCC(bp);
//         PUT(PRED(first_free),0);
//     }
//     else{
//         PUT(SUCC(PRED(bp)), SUCC(bp));
//         PUT(PRED(SUCC(bp)), PRED(bp));
//     }
// }

/**/
/*
 * mm_init - initialize the malloc package.
 */


int mm_init(void)
{
    if((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
    // if((heap_listp = mem_sbrk(7*WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));
    // PUT(heap_listp + (3*WSIZE), PACK(2*DSIZE, 0));
    // PUT(heap_listp + (4*WSIZE), 0); // 이전 가용 블록
    // PUT(heap_listp + (5*WSIZE), 0); // 다음 가용 블록
    // PUT(heap_listp + (6*WSIZE), PACK(2*DSIZE, 0));
    // add_fake_free_block(heap_listp + (2*WSIZE));
    PUT(heap_listp + (7*WSIZE), PACK(0, 1)); 
    heap_listp += (2*WSIZE);
    // next_point_bp = 
    // first_free = heap_listp + (2*WSIZE);
    if(extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;
    return 0;
}



/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

static void *extend_heap(size_t words){
    char *bp;
    size_t size;

    size = (words % 2) ? (words + 1) : words;
    size *= WSIZE;

    if((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));

    // if((long)(bp = mem_sbrk(4*WSIZE)) == -1)
    //     return NULL;
        // 종료를 위한 fake_free_block 생성
    
    // add_fake_free_block(bp);
    
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
    

    return coalesce(bp);
}

static void *coalesce(void *bp){
    size_t prev_alloc;
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));
    
    if ((char *)bp == heap_listp) {
        prev_alloc = 1;
    }
    else {
        prev_alloc = GET_ALLOC(HDRP(PREV_BLKP(bp)));
    }

    if(prev_alloc && next_alloc){
        return bp;
    }
    else if(prev_alloc && !next_alloc){
        // remove_free_block(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    else if(!prev_alloc && next_alloc){
        // remove_free_block(PREV_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    else{
        // remove_free_block(PREV_BLKP(bp));
        // remove_free_block(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    // new_free_block(bp);

    return bp;
}


/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */


//  void *next_fit(size_t asize, void *bp){
//     if(GET_SIZE(HDRP(bp)) == 0){
//         // return next_fit(asize, next_point_bp, bp);
//         return NULL;
//     }
//     if (!GET_ALLOC(HDRP(bp)) && (GET_SIZE(HDRP(bp)) >= asize))
//         return bp;
//     return first_fit(asize, NEXT_BLKP(bp));
//  } 

 void *first_fit(size_t asize, void *bp){
    if(GET_SIZE(HDRP(bp)) == 0){
        return NULL;
    }
    if (!GET_ALLOC(HDRP(bp)) && (GET_SIZE(HDRP(bp)) >= asize))
        return bp;
    return first_fit(asize, NEXT_BLKP(bp));
 }


 static void *find_fit(size_t asize)
 {
    return first_fit(asize, heap_listp);
 }
 
 /* split one block into free and allocated blocks */
 static void place(void *bp, size_t asize){
    
    size_t csize = GET_SIZE(HDRP(bp));
    if ((csize-asize) >= (2*DSIZE)){
        // remove_free_block(bp);
        PUT(HDRP(bp), PACK(asize,1));
        PUT(FTRP(bp), PACK(asize,1));
        // void * next_bp = NEXT_BLKP(bp);
        bp = NEXT_BLKP(bp);
        // PUT(HDRP(next_bp), PACK(csize-asize,0));
        // PUT(FTRP(next_bp), PACK(csize-asize,0));
        PUT(HDRP(bp), PACK(csize-asize,0));
        PUT(FTRP(bp), PACK(csize-asize,0));
        // new_free_block(next_bp);
    }
    else{
        // remove_free_block(bp);
        PUT(HDRP(bp), PACK(csize,1));
        PUT(FTRP(bp), PACK(csize,1));
    }
 }


void *mm_malloc(size_t size)
{
    size_t asize;
    size_t extendsize;
    char *bp;

    if (size == 0) return NULL;

    if (size <= DSIZE){
        asize = 2*DSIZE;
    }
    else {
        asize = DSIZE* ( (size + (DSIZE)+ (DSIZE-1)) / DSIZE );
    }

    if ((bp = find_fit(asize)) != NULL){
        place(bp,asize);
        return bp; 
    }

    extendsize = MAX(asize,CHUNKSIZE);
    if ((bp=extend_heap(extendsize/WSIZE)) == NULL){ 
        return NULL;
    }
    place(bp,asize);
    return bp;
}


/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
        copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}